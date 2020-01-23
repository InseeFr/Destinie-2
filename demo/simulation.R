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
library(openxlsx)
library(stringr)

config=''
lib=Sys.getenv('DESTINIE_LIB_PATH')

args = commandArgs(trailingOnly = FALSE)

with_config_path = FALSE
prefixIndex = which(args == "--config")
if (str_length(config) > 0 || length(prefixIndex) && prefixIndex < length(args)) {
  from_args=length(prefixIndex) && prefixIndex < length(args)
  config_path = ifelse(from_args, args[prefixIndex+1], config)
  with_config_path = TRUE
  sourcepath = config_path
}

with_input_path = FALSE
if (!with_config_path) {
  with_input_path = TRUE
  prefixIndex = which(args == "--file")
  if (length(prefixIndex) && prefixIndex < length(args)) {
    input_path = args[prefixIndex+1]
  } else {
    input_path = "server/example.xlsx"
  }
  sourcepath = input_path
}

with_infla=1
if (length(which(args == "--no-infla"))) {
  with_infla=0
}

#detach(package:destinie, unload=T)
prefixIndex = which(args == "--library")
if (str_length(lib) > 0 || (length(prefixIndex) && prefixIndex < length(args))) {
  print('load separated lib...')
  from_args=length(prefixIndex) && prefixIndex < length(args)
  separate_lib=ifelse(from_args, args[prefixIndex+1], lib)
  print(separate_lib)
  .libPaths(separate_lib)
}

## Regimes
# 1 ACTUEL
# 2 ACTUEL_MODIF
# 3 DELEVOYE
# 4 COMM_PM
regime = 1
regimes = c(ACTUEL=1, ACTUEL_MODIF=2, DELEVOYE=3, COMM_PM=4)
prefixIndex = which(args == "--regime")
if (length(prefixIndex) && prefixIndex < length(args)) {
  regime=regimes[args[prefixIndex+1]]
  if(is.na(regime)) {
    regime=1
  }
}
library(destinie)

data("test")


# 0 = TauxPlein ; 3 = EXO
comportement=0


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

  if(length(which(simul$ech$age_exo > 0))) {
    comportement=3
  }
}

age_exo=0
prefixIndex = which(args == "--age-exo")
if (length(prefixIndex) && prefixIndex < length(args)) {
  age_exo=as.integer(args[prefixIndex+1])
  comportement=3
  simul$ech$age_exo=age_exo
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
    
    simul$options <- list(tp=F,anLeg=2019,pas1=3/12,pas2=1,
                      AN_MAX=as.integer(fin_simul),champ,
                      NoAccordAgircArrco=F, NoRegUniqAgircArrco=T, 
                      SecondLiq=F,mort_diff_dip=T,effet_hrzn=T,
                      comp=comportement,# 0 = TP ; 3 = EXO
                      ecrit_dr_test=T,
                      NoCoeffTemp=T,
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
  library(dplyr)

  data("eco_cho_7_prod13")
  eco=eco_cho_7_prod13

  # Supprime  l'augmentation de la CSG prévue en 2018
  eco$macro$TauxCSGRetFort[eco$macro$annee==2018] = NaN

  #rm(eco_cho_7_prod13)
  eco$macro <-eco$macro%>%
    mutate( 
      SMPTp = ifelse(is.na(SMPTp),0,SMPTp),
      SMICp = ifelse(is.na(SMICp),0,SMICp),
      PIBp = ifelse(is.na(PIBp),0,PIBp),
      PlafondSSp = ifelse(is.na(PlafondSSp),0,PlafondSSp),
      Prixp = with_infla*ifelse(is.na(Prixp),0,Prixp),
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

  # Indexation des valeurs de points de la réforme
  total = length(eco$macro$annee)
  duree = 17 # année
  debut = 2025 # croissance de 2025 appliquée en 2026 cf lag dans seriep
  base = 'Prixp'
  dest = 'SMPTp'

  idebut = debut - 1900 +1
  transition = 0:duree/duree
  propBase = c(rep(0, idebut -1),1-transition,rep(0,total-duree-idebut))
  propDest = c(rep(0, idebut -1),transition,rep(1,total-duree-idebut))
  seriep = cumprod(1 + lag(eco$macro[[base]] * propBase + eco$macro[[dest]] * propDest, default=0))

  eco$macro <-eco$macro%>% mutate(
    IndexationBaseReforme = propBase,
    IndexationDestReforme = propDest,
    CotisationReforme = 0.2531,
    ValeurAchatReforme = 10 * seriep,
    ValeurVenteReforme = 0.55 * seriep,
    AgeEquilibre = 65
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
  carrieres_path=input$carrieres_path[1]
  meta = read.xlsx(carrieres_path, 'meta')
  series = read.xlsx(carrieres_path, 'serie')

  naissance = as.integer(input$naissance[1])
  age_mort=80
  debut=as.integer(input$debut[1])
  p_proportion = input$proportion
  profil_id=input$carriere
  base_id=id=meta$base[meta$id == profil_id]

  duree_carriere=age_mort-debut

  profil = series[[profil_id]][(debut):(debut+duree_carriere-1)]
  if (is.null(profil)) {
    profil = rep(1, duree_carriere)
  }
  base = eco$macro[[base_id]]

  statut = c(rep(63,debut),rep(1, age_mort-debut))
  salaire = c(
    rep(0,debut),
    p_proportion * profil * base[(naissance+debut-1900+1):((naissance+debut-1900+1)+duree_carriere-1)],
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
                        dipl=as.integer(0),
                        age_exo=age_exo
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

  if(!is.null(input$age[1])) {
    age_value = as.integer(input$age[1])
    if(age_value) {
      simulation$options$comp = 3
      simulation$ech$age_exo=age_value
    }
  }
}

#destinieDemographie(simulation)
demoSimulation = as.environment(simulation)
#library(destinie)
destinieSim(demoSimulation)

# détermination des derniers salaires
age_depart = demoSimulation$retraites %>% group_by(Id) %>% filter(pension != 0) %>%
  summarise(age=min(age)) %>% select(Id,age)
dernier_salaire_net_b = demoSimulation$salairenet %>% inner_join(age_depart, by=c('Id', 'age')) %>%
  select(Id, age, dernier_salaire_net=salaires_net)
naiss = demoSimulation$ech %>% select(Id, anaiss)
derniers_salaires_nets = dernier_salaire_net_b %>% left_join(by='Id', naiss) %>%
  mutate(annee=age+anaiss) %>% select(Id, annee, dernier_salaire_net)

# desindexation infla
base=2019
adjust_infla = demoSimulation$macro %>% select(annee, Prix)
adjust_infla$ref = adjust_infla$Prix / adjust_infla$Prix[which(adjust_infla$annee == base)]

deflate <- function(df, ...) {
  name_it <- function(n) {
    paste0(quo_name(n), '_neut')
  }
  expr_it <- function(n) {
    quo((!! n) / ref)
  }
  exprs <- lapply(enquos(...), expr_it)
  names(exprs) <- lapply(enquos(...), name_it)
  return(
    df %>%
      left_join(adjust_infla %>% select(annee, ref), by="annee") %>%
      mutate(!!! exprs) %>% select(-ref)
  )
}
dernier_salaire_net_n = derniers_salaires_nets %>% deflate(dernier_salaire_net) %>%
  select(Id, dernier_salaire_net_neut)
demoSimulation$retraites <- demoSimulation$retraites %>%
  deflate(pension, retraite_nette) %>%
  left_join(by='Id', dernier_salaire_net_n) %>%
  mutate(
    pension_m = pension / 12,
    pension_neut_m = pension_neut / 12,
    retraite_nette_m = retraite_nette / 12,
    retraite_nette_neut_m = retraite_nette_neut / 12,
    TR_net_neut = retraite_nette_neut / dernier_salaire_net_neut
  )
#print(demoSimulation$retraites %>% select(annee, pension_neut_m))

demoSimulation$taux_remplacement = demoSimulation$retraites %>% group_by(Id) %>% mutate(debut=min(annee)) %>% filter(annee == debut) %>% select(Id, TR_net_neut)
if(T) {
  ## Create a new workbook
  wb <- createWorkbook("fullhouse")
  for (field in c("ech", "emp", "fam", "liquidations", "retraites", "salairenet", "taux_remplacement", "cotisations", "macro")){
    addWorksheet(wb, field)
    writeData(wb, field, demoSimulation[[field]])
  }
  ## Save workbook
  outputfile = str_c(str_sub(sourcepath,end=-6), 'results.xlsx', sep=".")
  saveWorkbook(wb, outputfile, overwrite = TRUE)
}
