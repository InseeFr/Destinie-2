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
library(jsonlite)
.libPaths("~/R-tests")
library(openxlsx)
library(dplyr)
library(stringr)


with_input_path = FALSE
args = commandArgs(trailingOnly = FALSE)
prefixIndex = which(args == "--file")
if (length(prefixIndex) && prefixIndex < length(args)) {
  input_path = args[prefixIndex+1]
  with_input_path = TRUE
} else {
  input_path = "server/example.xlsx"
  #with_input_path = TRUE
}

with_config_path = FALSE
if (with_input_path) {
  sourcepath = input_path
} else {
  prefixIndex = which(args == "--config")
  if (length(prefixIndex) && prefixIndex < length(args)) {
    config_path = args[prefixIndex+1]
    with_config_path = TRUE
  } else {
    config_path = "/var/log/destinie/files/2019-12-14--19-42-20.027783-config"
    with_config_path = TRUE
    # input_path = "server/example.xlsx"
    # with_input_path = TRUE
  }
  sourcepath = config_path
}

with_infla=1
if (length(which(args == "--no-infla"))) {
  with_infla=0
}


## Regimes
# 1 ACTUEL
# 2 ACTUEL_MODIF
# 3 DELEVOYE
regime = 1
regimes = c(ACTUEL=1, ACTUEL_MODIF=2, DELEVOYE=3)
prefixIndex = which(args == "--regime")
if (length(prefixIndex) && prefixIndex < length(args)) {
  regime=regimes[args[prefixIndex+1]]
  if(is.na(regime)) {
    regime=1
  }
}
#detach(package:destinie, unload=T)
library(destinie)

data("test")


simul=test

if (with_input_path) {

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
}
#simul$ech$age_exo = 64

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
                      SecondLiq=F,mort_diff_dip=T,effet_hrzn=T,
                      comp=0,# 0 = TP ; 3 = EXO
                      ecrit_dr_test=F,
                      codeRegime=as.integer(regime)
                      )

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
      Prixp = with_infla*ifelse(is.na(Prixp),0,Prixp), # Pour supprimer l'
      MinPRp = 1.02,
      RevaloRG = ifelse(is.na(RevaloRG),1+Prixp,RevaloRG),
      RevaloFP = ifelse(is.na(RevaloFP),1+Prixp,RevaloFP),
      RevaloSPC = ifelse(is.na(RevaloSPC),1+Prixp,RevaloSPC),
      ValeurPointReforme = 1/(42*12)
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


if (with_config_path) {
  input = fromJSON(config_path)
  naissance = as.integer(input$naissance[1])
  age_mort=80
  duree_carriere=43
  debut=input$debut[1]
  statut = c(rep(63,debut),rep(1, age_mort-debut))
  salaire = c(
    rep(0,debut),
    input$proportion*eco$macro[[input$base]][(naissance+debut-1900+1):((naissance+debut-1900+1)+duree_carriere-1)],
    rep(0, age_mort-debut-duree_carriere)
  )

  simulation$ech = data.frame(Id=as.integer(1),
                        sexe=as.integer(1),
                        findet=as.integer(0),
                        typeFP=as.integer(20),
                        anaiss=naissance,
                        neFrance=as.integer(1),
                        k=0,
                        taux_prim=0,
                        moisnaiss=as.integer(1),
                        emigrant=as.integer(0),
                        tresdip=as.integer(0),
                        peudip=as.integer(0),
                        dipl=as.integer(0)
                        )

  simulation$emp = data.frame(Id=as.integer(1),
                        age=as.integer(0:(age_mort-1)),
                        statut=as.integer(statut),
                        salaire=salaire)

  simulation$fam = data.frame(Id=as.integer(1),
                        pere=as.integer(0),
                        mere=as.integer(0),
                        enf1=as.integer(0),
                        enf2=as.integer(0),
                        enf3=as.integer(0),
                        enf4=as.integer(0),
                        enf5=as.integer(0),
                        enf6=as.integer(0),
                        matri=as.integer(1),
                        annee=as.integer(2019),
                        conjoint=as.integer(0))
}

#destinieDemographie(simulation)
demoSimulation = as.environment(simulation)
#library(destinie)
destinieSim(demoSimulation)


## Create a new workbook
wb <- createWorkbook("fullhouse")
for (field in c("ech", "emp", "fam", "liquidations", "retraites", "salairenet", "cotisations", "macro")){
  addWorksheet(wb, field)
  writeData(wb, field, demoSimulation[[field]])
}
print(demoSimulation$retraites)
## Save workbook
outputfile = str_c(str_sub(sourcepath,end=-6), 'results.xlsx', sep=".")
saveWorkbook(wb, outputfile, overwrite = TRUE)
