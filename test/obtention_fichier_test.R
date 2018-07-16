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

#########################################################################################
#########################################################################################
#########################################################################################

#Ce fichier est fourni pour information uniquement : il indique comment
#l'échantillon test non représentatif, non confidentiel, a été construit.

  #on choisit son directory
  setwd("D:/NJ1DX9/Destinie/destinie")
##############################################
# Paramètres de la simulation 
  rep_rebasage <- "Z:\\"
  champ<-"FE" # ou  FM
  fin_simul<-2133 #en effet les nouveaux Destiniens naissent a partir de 2010; 
  #comme les Destiniens vivent au plus 118 ans, en 2130, que des Destiniens entierement simulés.

  
#########################################################################################  
######################################################################################### 
#########################################################################################
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
    find_rtools()
    #Rcpp::compileAttributes()
    devtools::load_all(".")

    
    
####################################
#échantillon de depart
######################################
load(paste0(rep_rebasage,"simul_patrimoine.Rdata"))    

emp0=simul$emp
ech0=simul$ech
fam0=simul$fam
    
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
# puis garder constants apres cette date
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
feuille=feuilles[[1]]
xlsx::addDataFrame(sc_demo,sheet = feuille,row.names = T,col.names=T, startRow = 1,startColumn = 1)
xlsx::saveWorkbook(wb, ".\\parametres\\Param_demo\\sc_demo.xls") 
#openXL(paste0(".\\parametres\\Param_demo\\ciblesDemographie_",champ,".xls"))
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

# destinieImputSal(simul)
# destinieCalageSalaires(simul)
# destinieSim(simul)

















#########################################################################################  
######################################################################################### 
#########################################################################################

#1- on sort de simul:
    fam=simul$fam
    emp=simul$emp
    ech=simul$ech
    
    #sauvegarde de simul en cas de problèmes
    #simul3=new.env()
    #simul3$fam%>%View()

#2- on ne garde que les gens vivants en 2130
    #pour une raison de modulo1900, on enleve aussi les gens vivants en 2130 mais nés avant 2025
    #(on limite la duree de vie de l'echantillon -non representatif- à 105 ans)
    #dernier problème : un simulé né en 2130 mais décédé la même année ; donc pas créé dans emp, donc erreur.
    #pour éviter cela, on vérifie que les personnes sont bien dans emp.
    toto = fam%>%
      select(Id,annee)%>%
      mutate(present2130=ifelse(annee==2130,1,0))%>%
      left_join(ech%>%select(Id,anaiss), by="Id")%>%
      mutate(present2130=ifelse(anaiss>2025,present2130,0))%>%
      mutate(present2130=ifelse(anaiss>2130,0,present2130))%>%
      group_by(Id)%>%
      summarise(present2130=sum(present2130))%>%
      ungroup()%>%
      mutate(present2130=ifelse(present2130>0,1,0))%>%
      #verification que les personnes apparaissent bien dans emp (pas le cas des morts-nés)
      left_join(emp%>%
                   group_by(Id)%>%
                   summarise(presentemp=1)%>%
                   ungroup(),
                 by="Id")%>%
      mutate(present2130=ifelse(presentemp==1,present2130,0))%>%
      select(-presentemp)
   # summary(toto$present2130)
    #  toto%>%View()
    
    fam=fam%>%
      filter(annee==2130)%>%
      #les trois lignes pour d'eventuelles personnes nés avant 2025 et survivantes en 2130
      left_join(toto,by="Id")%>%
      filter(present2130==1)%>%
      select(-present2130)
      #fam%>%left_join(ech%>%select(Id,anaiss))%>%View()
    emp=emp%>%
      left_join(toto,by="Id")%>%
      filter(present2130==1)%>%
      #on pense aussi a enlever les annees 2131-2133
      left_join(ech%>%select(Id,anaiss), by="Id")%>%
      mutate(annee=anaiss+age)%>%
      filter(annee<2131)%>%
      select(-present2130,-annee,-anaiss)
      #emp%>%left_join(ech%>%select(Id,anaiss))%>%View()
    ech=ech%>%
      left_join(toto,by="Id")%>%
      filter(present2130==1)%>%
      select(-present2130)
      #ech%>%View()

    
#3- on mets conjoint, pere, mere, enf1-6 à 0 lorsque pas present en 2130
    #fam0%>%View()

    fam=fam%>%
      #d abord je met à 0 les conjoints non survivants en 2130
      #en faisant attention que des personnes n'ont jamais eu de conjoints
      left_join(toto%>%
                  rename(conjoint=Id),
                by="conjoint")%>%
      mutate(present2130=ifelse(is.na(present2130),1,present2130))%>%
      mutate(conjoint=ifelse(present2130==0,0,conjoint))%>%
      select(-present2130)%>%
      #puis je met à 0 les meres non presentes en 2130
      left_join(toto%>%
                  rename(mere=Id),
                by="mere")%>%
      mutate(present2130=ifelse(is.na(present2130),1,present2130))%>%
      mutate(mere=ifelse(present2130==0,0,mere))%>%
      select(-present2130)%>%
      #puis je met à 0 les peres non presents en 2130
      left_join(toto%>%
                  rename(pere=Id),
                by="pere")%>%
      mutate(present2130=ifelse(is.na(present2130),1,present2130))%>%
      mutate(pere=ifelse(present2130==0,0,pere))%>%
      select(-present2130)%>%
      #puis je met à 0 les enf6 non presents en 2130
      left_join(toto%>%
                  rename(enf6=Id),
                by="enf6")%>%
      mutate(present2130=ifelse(is.na(present2130),1,present2130))%>%
      mutate(enf6=ifelse(present2130==0,0,enf6))%>%
      select(-present2130)%>%
    #puis je met à 0 les enf5 non presents en 2130
    #si jamais je remets à 0, je décale avec enf6 pour éviter un trou dans les enfants
    left_join(toto%>%
                rename(enf5=Id),
              by="enf5")%>%
      mutate(present2130=ifelse(is.na(present2130),1,present2130))%>%
      mutate(enf5=ifelse(present2130==0, enf6, enf5))%>%
      mutate(enf6=ifelse(present2130==0, 0,    enf6))%>%
      select(-present2130)%>%
      #puis je met à 0 les enf4 non presents en 2130
      #si jamais je remets à 0, je décale avec enf5 et enf6 pour éviter un trou dans les enfants
      left_join(toto%>%
                  rename(enf4=Id),
                by="enf4")%>%
      mutate(present2130=ifelse(is.na(present2130),1,present2130))%>%
      mutate(enf4=ifelse(present2130==0,  enf5, enf4))%>%
      mutate(enf5=ifelse(present2130==0,  enf6, enf5))%>%
      mutate(enf6=ifelse(present2130==0,  0,    enf6))%>%
      select(-present2130)%>%
      #puis je met à 0 les enf3 non presents en 2130
      #si jamais je remets à 0, je décale avec enf4, enf5 et enf6 pour éviter un trou dans les enfants
      left_join(toto%>%
                  rename(enf3=Id),
                by="enf3")%>%
      mutate(present2130=ifelse(is.na(present2130),1,present2130))%>%
      mutate(enf3=ifelse(present2130==0,  enf4, enf3))%>%
      mutate(enf4=ifelse(present2130==0,  enf5, enf4))%>%
      mutate(enf5=ifelse(present2130==0,  enf6, enf5))%>%
      mutate(enf6=ifelse(present2130==0,  0,    enf6))%>%
      select(-present2130)%>%
      #puis je met à 0 les enf2 non presents en 2130
      #si jamais je remets à 0, je décale avec enf3, enf4, enf5 et enf6 pour éviter un trou dans les enfants
      left_join(toto%>%
                  rename(enf2=Id),
                by="enf2")%>%
      mutate(present2130=ifelse(is.na(present2130),1,present2130))%>%
      mutate(enf2=ifelse(present2130==0,  enf3, enf2))%>%
      mutate(enf3=ifelse(present2130==0,  enf4, enf3))%>%
      mutate(enf4=ifelse(present2130==0,  enf5, enf4))%>%
      mutate(enf5=ifelse(present2130==0,  enf6, enf5))%>%
      mutate(enf6=ifelse(present2130==0,  0,    enf6))%>%
      select(-present2130)%>%
      #puis je met à 0 les enf1 non presents en 2130
      #si jamais je remets à 0, je décale avec enf2, enf3, enf4, enf5 et enf6 pour éviter un trou dans les enfants
      left_join(toto%>%
                  rename(enf1=Id),
                by="enf1")%>%
      mutate(present2130=ifelse(is.na(present2130),1,present2130))%>%
      mutate(enf1=ifelse(present2130==0,  enf2, enf1))%>%
      mutate(enf2=ifelse(present2130==0,  enf3, enf2))%>%
      mutate(enf3=ifelse(present2130==0,  enf4, enf3))%>%
      mutate(enf4=ifelse(present2130==0,  enf5, enf4))%>%
      mutate(enf5=ifelse(present2130==0,  enf6, enf5))%>%
      mutate(enf6=ifelse(present2130==0,  0,    enf6))%>%
      select(-present2130)
    
#4- on retire 121 ans aux différentes variables d'années (ech et fam)
    fam=fam%>%
      mutate(annee=annee-121)
    ech=ech%>%
      mutate(anaiss=anaiss-121)


    # ech%>%
    #  left_join(emp%>%
    #                select(age,Id)%>%
    #                group_by(Id)%>%
    #                summarise(age=max(age))%>%
    #                ungroup(),
    #              by="Id")%>%
    #    mutate(annee=anaiss+age)%>%
    #   filter(annee!=2009)%>%
    #   View()
    # 
    #  ech0%>%
    #     left_join(emp0%>%
    #                 select(age,Id)%>%
    #                 group_by(Id)%>%
    #                 summarise(age=max(age))%>%
    #                 ungroup(),
    #               by="Id")%>%
    #     mutate(annee=anaiss+age)%>%
    #    filter(annee!=2009)%>%
    #    View()
    #  
    #  fam%>%filter(annee!=2009)%>%
    #    View()
    #  fam0%>%filter(annee!=2009)%>%
    #    View()


#5- on re-indexe de 1 à N l'echantillon, et on change l'id...
    #puis dans fam: conjoint,pere, mere, enf1,enf2,enf3,enf4,enf5,enf6
    
    toto2= fam%>%
      mutate(Id2=row_number())%>%
      select(Id,Id2)
    
    ech=ech%>%
      left_join(toto2,by="Id")%>%
      select(-Id)%>%
      rename(Id=Id2)
    
    emp=emp%>%
      left_join(toto2,by="Id")%>%
      select(-Id)%>%
      rename(Id=Id2)
    
    fam=fam%>%
      left_join(toto2,by="Id")%>%
      select(-Id)%>%
      rename(Id=Id2)
    
    fam=fam%>%
      #on renomme le conjoint
      left_join(toto2%>%
                  rename(conjoint=Id),
                by="conjoint")%>%
      select(-conjoint)%>%
      rename(conjoint=Id2)%>%
      mutate(conjoint=ifelse(is.na(conjoint),0,conjoint))%>%
      #on renomme la mère
      left_join(toto2%>%
                  rename(mere=Id),
                by="mere")%>%
      select(-mere)%>%
      rename(mere=Id2)%>%
      mutate(mere=ifelse(is.na(mere),0,mere))%>%
      #on renomme le pere
      left_join(toto2%>%
                  rename(pere=Id),
                by="pere")%>%
      select(-pere)%>%
      rename(pere=Id2)%>%
      mutate(pere=ifelse(is.na(pere),0,pere))%>%
      #on renomme enf1
      left_join(toto2%>%
                  rename(enf1=Id),
                by="enf1")%>%
      select(-enf1)%>%
      rename(enf1=Id2)%>%
      mutate(enf1=ifelse(is.na(enf1),0,enf1))%>%
      #on renomme enf2
      left_join(toto2%>%
                  rename(enf2=Id),
                by="enf2")%>%
      select(-enf2)%>%
      rename(enf2=Id2)%>%
      mutate(enf2=ifelse(is.na(enf2),0,enf2))%>%
      #on renomme enf3
      left_join(toto2%>%
                  rename(enf3=Id),
                by="enf3")%>%
      select(-enf3)%>%
      rename(enf3=Id2)%>%
      mutate(enf3=ifelse(is.na(enf3),0,enf3))%>%
      #on renomme enf4
      left_join(toto2%>%
                  rename(enf4=Id),
                by="enf4")%>%
      select(-enf4)%>%
      rename(enf4=Id2)%>%
      mutate(enf4=ifelse(is.na(enf4),0,enf4))%>%
      #on renomme enf5
      left_join(toto2%>%
                  rename(enf5=Id),
                by="enf5")%>%
      select(-enf5)%>%
      rename(enf5=Id2)%>%
      mutate(enf5=ifelse(is.na(enf5),0,enf5))%>%
      #on renomme enf6
      left_join(toto2%>%
                  rename(enf6=Id),
                by="enf6")%>%
      select(-enf6)%>%
      rename(enf6=Id2)%>%
      mutate(enf6=ifelse(is.na(enf6),0,enf6))
    
#6- on fait en sorte que toutes les variables aient le bon type
    emp=emp[,c("age","statut","Id","salaire")]
    ech=ech[,c("sexe","Id","findet","typeFP","anaiss","neFrance",
                "k","taux_prim","moisnaiss","emigrant","tresdip","peudip","dipl")]
      ech=ech%>%
        mutate(anaiss=as.integer(anaiss))
    fam=fam[,c("pere","mere","enf1","enf2","enf3","enf4","enf5","enf6","matri","Id","annee","conjoint")]
      fam=fam%>%
        mutate(pere=as.integer(pere),
               mere=as.integer(mere),
               enf1=as.integer(enf1),
               enf2=as.integer(enf2),
               enf3=as.integer(enf3),
               enf4=as.integer(enf4),
               enf5=as.integer(enf5),
               enf6=as.integer(enf6),
               annee=as.integer(annee),
               conjoint=as.integer(conjoint))
#fin: on re-crée simul:
simul=new.env()  
simul$fam <- fam
simul$emp <- emp
simul$ech <- ech

#on enregistre le fichier sous son nom de test
save(simul,file="test\\test_RIENconfidentiel_NONrepresentatif.Rda")
















# 
# 
# #########################################################################################
# #########################################################################################
# #########################################################################################
# #et on vérifie que cela marche bien:
# #on choisit son directory
# setwd("D:/NJ1DX9/Destinie/destinie")
# ##############################################
# # Paramètres de la simulation
# rep_rebasage <- "Z:\\"
# champ<-"FE" # ou  FM
# fin_simul<-2070 #en effet les nouveaux Destiniens naissent a partir de 2010;
# #comme les Destiniens vivent au plus 118 ans, en 2130, que des Destiniens entierement simulés.
# #########################################################################################
# #########################################################################################
# #########################################################################################
# # Simulation ---------------------------------
# #chargement des librairies nécessaires
# library(devtools)
# library(Rcpp)
# library(openxlsx)
# library(xlsx)
# library(dplyr)
# library(tidyr)
# library(reshape2)
# library(ggplot2)
# #chargement du package
# find_rtools()
# #Rcpp::compileAttributes()
# devtools::load_all(".")
# ####################################
# #échantillon de depart
# ######################################
# #load(paste0(rep_rebasage,"simul_patrimoine.Rdata"))
# load("D:\\NJ1DX9\\Destinie\\destinie\\test_RIENconfidentiel_NONrepresentatif2.Rda")
# # simul$ech%>%View()
# simul$options_salaires <- list()
# simul$options <- list("tp",anLeg=2016,pas1=3/12,pas2=1,
#                       AN_MAX=as.integer(fin_simul),champ,
#                       NoAccordAgircArrco=F, NoRegUniqAgircArrco=T,
#                       SecondLiq=F,mort_diff_dip=T,effet_hrzn=T)
# # pas1=1/12 (pas mensuel pour proj COR)
# ################
# #choix du scenario demographique
# ################
# #dans le cas d'une simulation FM les paramètres démo sont renseignés sur la projection jusqu'en 2060
# # puis garder constants apres cette date
# Cent=c("*","","","")
# Bas=c("","*","","")
# Haut=c("","","*","")
# Travail=c("","","","*")
# x=data.frame(fecondite=Cent,esp_vie=Cent,migration=Cent)
# row.names(x)<-c("Cent","Bas","Haut","Travail")
# sc_demo=t(x)
# rm(x)
# wb=xlsx::loadWorkbook(".\\parametres\\Param_demo\\sc_demo.xls")
# feuilles=xlsx::getSheets(wb)
# feuille=feuilles[[1]]
# xlsx::addDataFrame(sc_demo,sheet = feuille,row.names = T,col.names=T, startRow = 1,startColumn = 1)
# xlsx::saveWorkbook(wb, ".\\parametres\\Param_demo\\sc_demo.xls")
# #openXL(paste0(".\\parametres\\Param_demo\\ciblesDemographie_",champ,".xls"))
# #################
# # on accepte les modifications, on enregistre on ferme et on reprend
# #################
# ############
# #chargement des paramètres demo +equations transitions marche du travail
# ################
# morta <- xlsx::loadWorkbook(paste0(".\\parametres\\Param_demo\\ciblesDemographie_",champ,".xls"))
# simul$Survie_H <- as.matrix(xlsx::readColumns(morta$getSheet("SH"),2,122,2,colClasses = "numeric"))
# simul$Survie_F <- as.matrix(xlsx::readColumns(morta$getSheet("SF"),2,122,2,colClasses = "numeric"))
# simul$Qmort_H <- as.matrix(xlsx::readColumns(morta$getSheet("QH"),2,122,2,colClasses = "numeric"))
# simul$Qmort_F <- as.matrix(xlsx::readColumns(morta$getSheet("QF"),2,122,2,colClasses = "numeric"))
# simul$espvie_H<- as.matrix(xlsx::readColumns(morta$getSheet("espvieH"),2,122,2,colClasses = "numeric"))
# simul$espvie_F<- as.matrix(xlsx::readColumns(morta$getSheet("espvieF"),2,122,2,colClasses = "numeric"))
# simul$mortadiff_dip_F <- xlsx::read.xlsx(paste0(".\\parametres\\Param_demo\\MORTA_DIP.xls"),sheetName ="morta_dif_F",startRow = 1)
# simul$mortadiff_dip_H <- xlsx::read.xlsx(paste0(".\\parametres\\Param_demo\\MORTA_DIP.xls"),sheetName ="morta_dif_H",startRow = 1)
# NA0<-function (x)
# {return (ifelse(is.na(x),0,x))}
# simul$CiblesDemo <- xlsx::read.xlsx(paste0(".\\parametres\\Param_demo\\ciblesDemographie_",champ,".xls"),sheetName ="CiblesDemo",startRow = 2,colClasses=rep("numeric",48))%>%
#   mutate_all(funs(NA0))
# simul$FinEtudeMoy <-  xlsx::read.xlsx(".\\parametres\\PARAM_etude.xls",sheetName = "TABLEFINDET0", startRow = 2)
# simul$mortalite_diff <-  xlsx::read.xlsx(".\\parametres\\PARAM_Mortalite_diff.xls",sheetName = "Mortalite_diff",startRow = 2)
# simul$EqSalaires <-  xlsx::read.xlsx(".\\parametres\\EqSalaires.xls",sheetName="eq_salaires")
# simul$CStructSexeAge <-  xlsx::read.xlsx(".\\parametres\\EqSalaires.xls",sheetName="CorrectStructSalSexeAge")
# simul$EqTrans <-  xlsx::read.xlsx(".\\parametres\\EqTrans.xls","EqTrans") %>%
#   mutate(indic=as.integer(indic)) %>%
#   mutate(ordre=as.factor(paste0("TRANS",ordre+1))) %>%
#   arrange(type_trans,origine,ordre)
# simul$EqSante <-  xlsx::read.xlsx(".\\parametres\\sante.xls",sheetName="adl")
# ###################
# #chargement des paramètres eco
# ##################
# scencho <- "7%" # ou 4,5 ou 10
# scenprod <- "1,3%" #ou 1,0 ou 1,5 ou 1,8
# ciblesTrans <- ".\\parametres\\Projection_COR_2018\\PARAM_transitions.xls"
# param_eco <- ".\\parametres\\Projection_COR_2018\\ParamSociaux_2018.xls"
# simul$macro <-
#   read_xls(param_eco,c("ParamGene","ParamRetrBase","ParamRetrComp","ParamRev","ParamAutres","ParamFam",paste0("cho",scencho,"_pib",scenprod)),annee=1900:fin_simul) %>%
#   mutate(
#     SMPTp = ifelse(is.na(SMPTp),0,SMPTp),
#     SMICp = ifelse(is.na(SMICp),0,SMICp),
#     PIBp = ifelse(is.na(PIBp),0,PIBp),
#     PlafondSSp = ifelse(is.na(PlafondSSp),0,PlafondSSp),
#     Prixp = ifelse(is.na(Prixp),0,Prixp),
#     MinPRp = 1.02,
#     RevaloRG = ifelse(is.na(RevaloRG),1+Prixp,RevaloRG),
#     RevaloFP = ifelse(is.na(RevaloFP),1+Prixp,RevaloFP),
#     RevaloSPC = ifelse(is.na(RevaloSPC),1+Prixp,RevaloSPC)
#   ) %>%
#   projection(
#     SMPT ~ cumprod((1+SMPTp)*(1+Prixp)),
#     PIB ~ cumprod((1+PIBp)*(1+Prixp)),
#     PlafondSS ~ cumprod((1+PlafondSSp)*(1+Prixp)),
#     SMIC ~ cumprod((1+SMICp)*(1+Prixp)),
#     Prix ~ cumprod(1+Prixp),
#     PointFP|PlafRevRG ~ SMPT,
#     SalValid ~ SMIC,
#     PlafARS1|PlafARS2|PlafARS3|PlafARS4|PlafARS5|PlafCF3|PlafCF4|PlafCF5|MajoPlafCF|sGMP|BMAF|SeuilPauvrete ~ SMPT,
#     MaxRevRG ~ PlafondSS,
#     MinPR ~ cumprod(MinPRp*(1+Prixp)),
#     MinVieil1|MinVieil2|Mincont1|Mincont2 ~ lag(cumprod(1+Prixp)), # indexation standard. En évolution, indexation sur l'inflation de t-1.
#     SalRefAGIRC_ARRCO|SalRefARRCO|SalRefAGIRC ~  cumprod(ifelse(annee%in%c(2016,2017,2018),(1+SMPTp+0.02)*(1+Prixp),(1+SMPTp)*(1+Prixp))),
#     ValPtAGIRC|ValPtARRCO|ValPtAGIRC_ARRCO ~ cumprod(1+ifelse(annee%in%c(2016,2017,2018),pmax(Prixp-0.01,0),Prixp)),
#     MinRevRG|SeuilExoCSG|SeuilExoCSG2|SeuilTxReduitCSG|SeuilTxReduitCSG2 ~cumprod(1+Prixp),
#     .~1
#   )
# simul$CiblesTrans <-  left_join(simul$macro %>% select(annee),  xlsx::read.xlsx(ciblesTrans,sheetName = paste0("cho",scencho),startRow = 3,colIndex=1:36))
# ############
# #simulation
# ###########
# destinieDemographie(simul)
# destinieTransMdt(simul)
# destinieImputSal(simul)
# destinieCalageSalaires(simul)
# destinieSim(simul)
# 
# ##############################################
# # Resultats ----------------------------------
# simul$Indicateurs_an %>% filter(regime=="tot"& sexe=="ens" & annee > 2010& annee<=2070)%>%
#   left_join(simul$macro)%>% ggplot(aes(x=annee,y=M_Pensions_ma/10/PIB,color=sexe)) + geom_line()+
#   theme_bw()
