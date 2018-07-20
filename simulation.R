# Destinie 2
# Copyright © 2005-2018, Institut national de la statistique et des études économiques
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

####################################
#échantillon de départ
######################################
source(file="C:/Destinie/param_perso.R")

##############################################
  #on indique que le working directory est le dossier clone
  setwd(dirname(rstudioapi::getActiveDocumentContext()$path))
  getwd()

##############################################
# Champ de la simulation 
  champ<-"FE" # ou  FM
  fin_simul<-2070 #2110 au maximum ou 2070 plus classiquement

##############################################
# Simulation ---------------------------------


  #chargement des librairies nécessaires
    library(devtools)
    library(Rcpp)
    library(openxlsx)
    library(xlsx)
    library(dplyr)
    library(tidyr)
    library(reshape2)
    library(ggplot2)

  
  #chargement du package
    assignInNamespace("version_info", 
                    c(devtools:::version_info, list("3.5" = list(version_min = "3.3.0", version_max = "99.99.99", path = "bin")
                                                    )), "devtools")
    find_rtools()
    #Rcpp::compileAttributes()
    devtools::load_all(".")
   

####################################
#choix d'options
######################################
    simul$options_salaires <- list()    
    
    simul$options <- list("tp",anLeg=2016,pas1=3/12,pas2=1,
                      AN_MAX=as.integer(fin_simul),champ,
                      NoAccordAgircArrco=F, NoRegUniqAgircArrco=T, 
                      SecondLiq=F,mort_diff_dip=T,effet_hrzn=T) 
                      # pas1=1/12 (pas mensuel pour proj COR)

################
#choix du scenario demographique 
################
#dans le cas d'une simulation FM les paramètres démo sont renseignés sur la projection jusqu'en 2060
# puis gardés constants apres cette date
Cent=c("*","","","")
Bas=c("","*","","")
Haut=c("","","*","")
Travail=c("","","","*")
x=data.frame(fecondite=Cent,esp_vie=Cent,migration=Cent)

row.names(x)<-c("Cent","Bas","Haut","Travail")
sc_demo=t(x)
rm(x)

wb=xlsx::loadWorkbook(".\\parametres\\Param_demo\\sc_demo.xls")
feuilles=xlsx::getSheets(wb)
feuille=feuilles[[2]]
xlsx::addDataFrame(sc_demo,sheet = feuille,row.names = T,col.names=T, startRow = 1,startColumn = 1)


xlsx::saveWorkbook(wb, ".\\parametres\\Param_demo\\sc_demo.xls") 

openXL(paste0(".\\parametres\\Param_demo\\ciblesDemographie_",champ,".xls"))
#################
# on accepte les modifications, on enregistre on ferme et on reprend 
#################
  ############
  #chargement des paramètres demo +equations transitions marche du travail
  ################
  morta <- xlsx::loadWorkbook(paste0(".\\parametres\\Param_demo\\ciblesDemographie_",champ,".xls"))
  simul$Survie_H <- as.matrix(xlsx::readColumns(morta$getSheet("SH"),2,122,2,colClasses = "numeric"))
  simul$Survie_F <- as.matrix(xlsx::readColumns(morta$getSheet("SF"),2,122,2,colClasses = "numeric"))
  simul$Qmort_H <- as.matrix(xlsx::readColumns(morta$getSheet("QH"),2,122,2,colClasses = "numeric"))
  simul$Qmort_F <- as.matrix(xlsx::readColumns(morta$getSheet("QF"),2,122,2,colClasses = "numeric"))
  simul$espvie_H<- as.matrix(xlsx::readColumns(morta$getSheet("espvieH"),2,122,2,colClasses = "numeric"))
  simul$espvie_F<- as.matrix(xlsx::readColumns(morta$getSheet("espvieF"),2,122,2,colClasses = "numeric"))
  simul$mortadiff_dip_F <- xlsx::read.xlsx(paste0(".\\parametres\\Param_demo\\MORTA_DIP.xls"),sheetName ="morta_dif_F",startRow = 1)
  simul$mortadiff_dip_H <- xlsx::read.xlsx(paste0(".\\parametres\\Param_demo\\MORTA_DIP.xls"),sheetName ="morta_dif_H",startRow = 1)
  
  
  NA0<-function (x)
  {return (ifelse(is.na(x),0,x))}
  simul$CiblesDemo <- xlsx::read.xlsx(paste0(".\\parametres\\Param_demo\\ciblesDemographie_",champ,".xls"),sheetName ="CiblesDemo",startRow = 2,colClasses=rep("numeric",48))%>%
    mutate_all(funs(NA0))
  simul$FinEtudeMoy <-  xlsx::read.xlsx(".\\parametres\\PARAM_etude.xls",sheetName = "TABLEFINDET0", startRow = 2)
  simul$mortalite_diff <-  xlsx::read.xlsx(".\\parametres\\PARAM_Mortalite_diff.xls",sheetName = "Mortalite_diff",startRow = 2)
  simul$EqSalaires <-  xlsx::read.xlsx(".\\parametres\\EqSalaires.xls",sheetName="eq_salaires")
  simul$CStructSexeAge <-  xlsx::read.xlsx(".\\parametres\\EqSalaires.xls",sheetName="CorrectStructSalSexeAge")
  simul$EqTrans <-  xlsx::read.xlsx(".\\parametres\\EqTrans.xls","EqTrans") %>%  
    mutate(indic=as.integer(indic)) %>%
    mutate(ordre=as.factor(paste0("TRANS",ordre+1))) %>%
    arrange(type_trans,origine,ordre)
  simul$EqSante <-  xlsx::read.xlsx(".\\parametres\\sante.xls",sheetName="adl")
  
  ###################
  #chargement des paramètres eco
  ##################
  scencho <- "7%" # ou 4,5 ou 10
  scenprod <- "1,3%" #ou 1,0 ou 1,5 ou 1,8
  ciblesTrans <- ".\\parametres\\Projection_COR_2018\\PARAM_transitions.xls"
  param_eco <- ".\\parametres\\Projection_COR_2018\\ParamSociaux_2018.xls"


simul$macro <-
  read_xls(param_eco,c("ParamGene","ParamRetrBase","ParamRetrComp","ParamRev","ParamAutres","ParamFam",paste0("cho",scencho,"_pib",scenprod)),annee=1900:fin_simul) %>% 
  mutate( 
    SMPTp = ifelse(is.na(SMPTp),0,SMPTp),
    SMICp = ifelse(is.na(SMICp),0,SMICp),
    PIBp = ifelse(is.na(PIBp),0,PIBp),
    PlafondSSp = ifelse(is.na(PlafondSSp),0,PlafondSSp),
    Prixp = ifelse(is.na(Prixp),0,Prixp),
    MinPRp = 1.02,
    RevaloRG = ifelse(is.na(RevaloRG),1+Prixp,RevaloRG),
    RevaloFP = ifelse(is.na(RevaloFP),1+Prixp,RevaloFP),
    RevaloSPC = ifelse(is.na(RevaloSPC),1+Prixp,RevaloSPC)
  ) %>%
  projection(
    SMPT ~ cumprod((1+SMPTp)*(1+Prixp)),
    PIB ~ cumprod((1+PIBp)*(1+Prixp)),
    PlafondSS ~ cumprod((1+PlafondSSp)*(1+Prixp)),
    SMIC ~ cumprod((1+SMICp)*(1+Prixp)),
    Prix ~ cumprod(1+Prixp),
    PointFP|PlafRevRG ~ SMPT,
    SalValid ~ SMIC,
    PlafARS1|PlafARS2|PlafARS3|PlafARS4|PlafARS5|PlafCF3|PlafCF4|PlafCF5|MajoPlafCF|sGMP|BMAF|SeuilPauvrete ~ SMPT,
    MaxRevRG ~ PlafondSS,
    MinPR ~ cumprod(MinPRp*(1+Prixp)),
    MinVieil1|MinVieil2|Mincont1|Mincont2 ~ lag(cumprod(1+Prixp)), # indexation standard. En évolution, indexation sur l'inflation de t-1.
    SalRefAGIRC_ARRCO|SalRefARRCO|SalRefAGIRC ~  cumprod(ifelse(annee%in%c(2016,2017,2018),(1+SMPTp+0.02)*(1+Prixp),(1+SMPTp)*(1+Prixp))),
    ValPtAGIRC|ValPtARRCO|ValPtAGIRC_ARRCO ~ cumprod(1+ifelse(annee%in%c(2016,2017,2018),pmax(Prixp-0.01,0),Prixp)),
    MinRevRG|SeuilExoCSG|SeuilExoCSG2|SeuilTxReduitCSG|SeuilTxReduitCSG2 ~cumprod(1+Prixp),
    .~1
  ) 

simul$CiblesTrans <-  left_join(simul$macro %>% select(annee),  xlsx::read.xlsx(ciblesTrans,sheetName = paste0("cho",scencho),startRow = 3,colIndex=1:36))
############
#simulation
###########
destinieDemographie(simul)
destinieTransMdt(simul)
destinieImputSal(simul)
destinieCalageSalaires(simul)
destinieSim(simul)



##############################################
# Résultats ----------------------------------
#âge moyen de liquidation pour tous et par sexes
simul$Indicateurs_an %>% filter(regime=="tot" & annee > 2000) %>% ggplot(aes(x=annee,y=Age_Ret_Flux,color=sexe)) + geom_line()
#masse despensions sur le Pib
simul$Indicateurs_an %>% filter(regime=="tot"& sexe=="ens" & annee > 2010& annee<=2070)%>%
  left_join(simul$macro)%>% ggplot(aes(x=annee,y=M_Pensions_ma/10/PIB,color=sexe)) + geom_line()+
  theme_bw()
