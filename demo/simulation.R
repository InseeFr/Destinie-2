# Destinie 2
# Copyright © 2005-2018, Institut national de la statistique et des etudes economiques
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
#echantillon de depart
######################################
library(destinie)
library(dplyr)
library(jsonlite)
library(stringr)
data("test")


library(openxlsx)


args = commandArgs(trailingOnly = FALSE)
prefixIndex = which(args == "--file")
if (length(prefixIndex) && prefixIndex < length(args)) {
  input_path = args[prefixIndex+1]
} else {
  input_path = "data.json"
}

## test

# print(input_path)
# input = fromJSON(input_path)
# naissance = as.integer(input$naissance[1])
# debut=input$debut[1]
# sal = input$salaires
# statut = c(rep(63,debut),rep(1, length(sal)-debut))

simul=test

# simul$ech = data.frame(Id=as.integer(1),
#                        sexe=as.integer(1),
#                        findet=as.integer(0),
#                        typeFP=as.integer(20),
#                        anaiss=naissance,
#                        neFrance=as.integer(1),
#                        k=0,
#                        taux_prim=0,
#                        moisnaiss=as.integer(1),
#                        emigrant=as.integer(0),
#                        tresdip=as.integer(0),
#                        peudip=as.integer(0),
#                        dipl=as.integer(0))

# simul$emp = data.frame(Id=as.integer(1),
#                        age=as.integer(0:(length(sal)-1)),
#                        statut=as.integer(statut),
#                        salaire=sal)

###

sourcepath = input_path

conv = list(ech=c('Id', 'sexe','findet',
                  'typeFP',
                  'anaiss',
                  'neFrance',
                  'moisnaiss',
                  'emigrant',
                  'tresdip',
                  'peudip',
                  'dipl'),
            emp=c('Id',
                  'age',
                  'statut'),
            fam=c('pere',
                  'mere',
                  'enf1',
                  'enf2',
                  'enf3',
                  'enf4',
                  'enf5',
                  'enf6',
                  'matri',
                  'Id',
                  'annee',
                  'conjoint'
            ))
for (field in c('ech', 'emp', 'fam')){
  simul[[field]] = read.xlsx(sourcepath, field)
  for (d in conv[[field]]) {
    simul[[field]][[d]] = as.integer(simul[[field]][[d]])
  }
}


#rm(test)
###################################
#remplacer ici test  par l'echantillon disponible sur Quetelet 
###################################


##############################################
# Champ de la simulation 
##############################################
champ<-"FE" # ou  FM
fin_simul<-2070 #2110 au maximum ou 2070 plus classiquement



####################################
#choix d'options
######################################
    simul$options_salaires <- list()    
    
    simul$options <- list("tp",anLeg=2019,pas1=3/12,pas2=1,
                      AN_MAX=as.integer(fin_simul),champ,
                      NoAccordAgircArrco=F, NoRegUniqAgircArrco=T, 
                      SecondLiq=F,mort_diff_dip=T,effet_hrzn=T) 

  ################
  #choix du scenario demographique 
  ################
  # ici la fecondite, l'esperance de vie et le solde migratoire suivent le scenario central des projections de l'Insee
  # pour la France entiere (attention a la coherence avec le champ precedemment choisi)
  # deux autres scenarios sont deja crees le premier ou tous les scenarios sont a bas et ce qui aboutit a une population agee
  # le second tous les scenarios sont a haut et ce qui aboutit a une population jeune    
  # les autres scenarios s'obtiennent en utilisant le programme \data_raw\obtention_hypdemo.R    
  data("fec_Cent_vie_Cent_mig_Cent")
  demo=fec_Cent_vie_Cent_mig_Cent
  #rm(fec_Cent_vie_Cent_mig_Cent)

  ############
  #chargement des equations regissant le marche du travail, de sante 
  ################

  data("eq_struct")
  
  ###################
  #chargement des parametres economiques puis projection des parametres dans le futur
  ##################

  data("eco_cho_7_prod13")
  eco=eco_cho_7_prod13
  #rm(eco_cho_7_prod13)
  eco$macro <-eco$macro%>%
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
      PlafARS1|PlafARS2|PlafARS3|PlafARS4|PlafARS5|PlafCF3|PlafCF4|PlafCF5|
        MajoPlafCF|sGMP|BMAF|SeuilPauvrete ~ SMPT,
      MaxRevRG ~ PlafondSS,
      MinPR ~ cumprod(MinPRp*(1+Prixp)),
      MinVieil1|MinVieil2|Mincont1|Mincont2 ~ lag(cumprod(1+Prixp)), # indexation standard. En evolution, indexation sur l'inflation de t-1.
      SalRefAGIRC_ARRCO|SalRefARRCO|SalRefAGIRC ~  cumprod(ifelse(annee%in%c(2016,2017,2018),(1+SMPTp+0.02)*(1+Prixp),(1+SMPTp)*(1+Prixp))),
      ValPtAGIRC|ValPtARRCO|ValPtAGIRC_ARRCO ~ cumprod(1+ifelse(annee%in%c(2016,2017,2018),pmax(Prixp-0.01,0),Prixp)),
      MinRevRG|SeuilExoCSG|SeuilExoCSG2|SeuilTxReduitCSG|SeuilTxReduitCSG2 ~cumprod(1+Prixp),
      .~1
    ) 

  eco$macro=eco$macro%>%filter(annee<=fin_simul)
  eco$CiblesTrans <-  left_join(eco$macro %>% select(annee), eco$CiblesTrans)
####################
# rassemblement dans un unique environnement
#####################
  eco=as.list(eco)
  demo=as.list(demo)
  simullist=as.list(simul)
  eq_struct=as.list(eq_struct)
  simulation=as.environment(c(demo,eco,simullist,eq_struct))
  #rm(demo,eco,simul,eq_struct)
############
#simulation
###########

#destinieDemographie(simulation)
demoSimulation = as.environment(simulation)
destinieSim(demoSimulation)


## Create a new workbook
wb <- createWorkbook("fullhouse")
for (field in c("ech", "emp", "fam", "liquidations", "retraites", "salairenet", "cotisations")){
  addWorksheet(wb, field)
  writeData(wb, field, demoSimulation[[field]])
}
## Save workbook
outputfile = str_c(sourcepath, 'results.xlsx', sep=".")
saveWorkbook(wb, outputfile, overwrite = TRUE)
