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
library(destinie)
data("test_RIENconfidentiel_NONrepresentatif")
###################################
#remplacer ci par l'echantillon de disponible sur Quetelet 
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
    
    simul$options <- list("tp",anLeg=2016,pas1=3/12,pas2=1,
                      AN_MAX=as.integer(fin_simul),champ,
                      NoAccordAgircArrco=F, NoRegUniqAgircArrco=T, 
                      SecondLiq=F,mort_diff_dip=T,effet_hrzn=T) 
                      # pas1=1/12 (pas mensuel pour proj COR)

  ################
  #choix du scenario demographique 
  ################
  # ici la fecondite, l'espérance de vie et le solde migratoire suive le scénario central des projections de l'Insee
  # pour la France entière (attention à la cohérence avec le champ précédemment choisi)
  # deux autres scénarios sont déjà créés le premier où tous les scénarios sont à bas et ce qui aboutit à une population âgée
  # le second tous les scénarios sont à haut et ce qui aboutit à une population jeune    
  # les autres scénarios s'obtiennent en utilisant le programme \data_raw\obtention_hypdemo.R    
  data("fec_Cent_vie_Cent_mig_Cent")



  ############
  #chargement des equations régissant le marché du travail, de santé 
  ################

  data("eq_struct")
  
  ###################
  #chargement des paramètres economiques puis projection des paramètres dans le futur
  ##################

  data("eco_cho_7_prod13")
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
      PlafARS1|PlafARS2|PlafARS3|PlafARS4|PlafARS5|PlafCF3|PlafCF4|PlafCF5|MajoPlafCF|sGMP|BMAF|SeuilPauvrete ~ SMPT,
      MaxRevRG ~ PlafondSS,
      MinPR ~ cumprod(MinPRp*(1+Prixp)),
      MinVieil1|MinVieil2|Mincont1|Mincont2 ~ lag(cumprod(1+Prixp)), # indexation standard. En évolution, indexation sur l'inflation de t-1.
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
  simul=as.list(simul)
  struct=as.list(struct)
  simulation=as.environment(c(demo,eco,simul,struct))
  rm(demo,eco,simul,struct)
############
#simulation
###########
destinieDemographie(simulation)
destinieTransMdt(simulation)
destinieImputSal(simulation)
destinieCalageSalaires(simulation)
destinieSim(simulation)



##############################################
# Résultats ----------------------------------
#âge moyen de liquidation pour tous et par sexes
simulation$Indicateurs_an %>% filter(regime=="tot" & annee > 2000) %>% ggplot(aes(x=annee,y=Age_Ret_Flux,color=sexe)) + geom_line()
#masse des pensions sur le Pib
simulation$Indicateurs_an %>% filter(regime=="tot"& sexe=="ens" & annee > 2010& annee<=2070)%>%
  left_join(simulation$macro)%>% ggplot(aes(x=annee,y=M_Pensions_ma/10/PIB,color=sexe)) + geom_line()+
  theme_bw()
