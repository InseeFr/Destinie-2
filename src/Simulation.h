#pragma once
/**
  * \file Simulation.h
  * \brief Fichier contenant la structure \ref Simulation, qui importe les paramètres et l'échantillon depuis les objets en R et
  * les rend accessibles en C avec des objets globaux (accessibles dans tout le programme).
  * 
  * Les objets globaux correspondant sont:
  * <ul>
  * <li> pop : vector d'objets individus contenant l'information pour chaque individu (info issue des tables ech, emp et fam) 
  * <li> M : pour les séries macroéconomiques et les séries de paramètres législatifs
  * <li> options : Options spécifiques à une simulation
  * <li> mortalite_diff : table de la mortalité différentielle
  * <li> finEtudeMoy : table des âges de fin d'étude moyens par génération
  * <li> ciblesdemo : table des cibles démographiques
  * <li> AN_FIN : dernière année de simulation  
  * </ul>
  * Lorsque l'objet Simulation est détruit les tables R créées avec les objets Rdout sont exportées en R.
  * Usage :
  * ```
  * // [[Rcpp::export]]
  * void destinie(Environment envSim) {
  *    // double smpt = M->SMPT[109]; << donnerait une erreur
  *    auto S = Simulation(envSim);
  *    double smpt = M->SMPT[109]; // Ok
  *    // A la fin de la fonction les tables R sont exportées et tous les objects C sont détruits
  * }
  * ```
  * 
  */

#include "OutilsBase.h"
#include "range.h" 
#include "Constantes.h"
#include "OutilsRcpp.h"
class Indiv;
class DroitsRetr;




struct CiblesDemo {
  /**
  * \struct CiblesDemo
  * \brief CiblesDemo contient les cibles démographiques de naissances et de solde migratoire 
  *
  * La structure \ref CiblesDemo contient, pour chaque année de projection, le nombre de naissances, ainsi que le solde migratoire ventilé 
  * par sexe et par âge. Attention : le solde migratoire global cible ne correspond pas à la somme des soldes 
  * migratoires déclinés par âge et sexe, car les soldes migratoires négatifs au-delà de 60 ans ne sont pas simulés.
  * 
  * Après 2013, ces cibles par âge fin sont issues de \cite blanpainprojections .
  * En revanche, pour les années 2010 à 2012 inclues, elles ont été fournies aux fins de ces exercices 
  * de modélisation par la division Enquêtes et études démographiques de l'Insee, avec l'avertissement 
  * que certaines peuvent être fragiles. Pour cette raison, elles ne sont pas publiées. Ces valeurs sont utilisées 
  * uniquement pour compléter des séries, et ne doivent l'être qu'à ce titre.
  */
  NumericVector 
  /** 
  * \brief Cibles de naissances
  */
  _(Naissances), 
  /**
  * \brief Solde migratoire global
  */
  _(Soldemig),  
  /**
  * \brief Solde migratoire global des garçons
  */
  _(Soldemig_G) ,
  /**
  * \brief Proportion de migrants garçons dont l'âge est compris entre 1 et 5 ans
  */  
  _(Soldemig_G_1_5),
  /**
  * \brief Proportion de migrants garçons dont l'âge est compris entre 1 et 11 ans
  */  
  _(Soldemig_G_6_11),
  /**
  * \brief Proportion de migrants garçons dont l'âge est compris entre 1 et 17 ans
  */
  _(Soldemig_G_12_17),
  /**
  * \brief Solde migratoire global de filles
  */ 
  _(Soldemig_Fi) ,
  /**
  * \brief Proportion de migrantes filles dont l'âge est compris entre 1 et 5 ans
  */   
  _(Soldemig_F_1_5),
  /**
  * \brief Proportion de migrantes filles dont l'âge est compris entre 1 et 11 ans
  */  
  _(Soldemig_F_6_11),
  /**
  * \brief Proportion de migrantes filles dont l'âge est compris entre 1 et 17 ans
  */
  _(Soldemig_F_12_17),
  /**
  * \brief Solde migratoire global d'hommes adultes d'âge compris enre 27 et 60 ans
  */  
  _(Soldemig_H) ,
  /**
  * \brief Proportion d'hommes migrants dont l'âge est compris entre 27 et 30 ans
  */   
  _(Soldemig_H_27_30) ,
  /**
  * \brief Proportion d'hommes migrants dont l'âge est compris entre 27 et 35 ans
  */ 
  _(Soldemig_H_31_35) ,
  /**
  * \brief Proportion d'hommes migrants dont l'âge est compris entre 27 et 40 ans
  */ 
  _(Soldemig_H_36_40) ,
  /**
  * \brief Proportion d'hommes migrants dont l'âge est compris entre 27 et 45 ans
  */ 
  _(Soldemig_H_41_45) ,
  /**
  * \brief Proportion d'hommes migrants dont l'âge est compris entre 27 et 50 ans
  */ 
  _(Soldemig_H_46_50) ,
  /**
  * \brief Proportion d'hommes migrants dont l'âge est compris entre 27 et 55 ans
  */ 
  _(Soldemig_H_51_55) ,
  /**
  * \brief Proportion d'hommes migrants dont l'âge est compris entre 27 et 60 ans
  */ 
  _(Soldemig_H_56_60),
  /**
  * \brief Solde migratoire global des femmes adultes d'âge compris enre 25 et 59 ans
  */  
  _(Soldemig_Fe) ,
  /**
  * \brief Proportion de femmes migrantes dont l'âge est compris entre 25 et 29 ans
  */   
  _(Soldemig_F_25_29) ,
  /**
  * \brief Proportion de femmes migrantes dont l'âge est compris entre 25 et 34 ans
  */   
  _(Soldemig_F_30_34) ,
  /**
  * \brief Proportion de femmes migrantes dont l'âge est compris entre 25 et 39 ans
  */  
  _(Soldemig_F_35_39) ,
  /**
  * \brief Proportion de femmes migrantes dont l'âge est compris entre 25 et 44 ans
  */  
  _(Soldemig_F_40_44) ,
  /**
  * \brief Proportion de femmes migrantes dont l'âge est compris entre 25 et 49 ans
  */  
  _(Soldemig_F_45_49) ,
  /**
  * \brief Proportion de femmes migrantes dont l'âge est compris entre 25 et 54 ans
  */  
  _(Soldemig_F_50_54) ,
  /**
  * \brief Proportion de femmes migrantes dont l'âge est compris entre 25 et 59 ans
  */  
  _(Soldemig_F_55_59),
  /**
  * \brief Probabilité d'un homme âgé de 18 ans d'émigrer
  */  
  _(proba_H_18),
  /**
  * \brief Probabilité d'un homme âgé de 19 ans d'émigrer
  */ 
  _(proba_H_19),
  /**
  * \brief Probabilité d'un homme âgé de 20 ans d'émigrer
  */ 
  _(proba_H_20),
  /**
  * \brief Probabilité d'un homme âgé de 21 ans d'émigrer
  */ 
  _(proba_H_21),
  /**
  * \brief Probabilité d'un homme âgé de 22 ans d'émigrer
  */ 
  _(proba_H_22),
  /**
  * \brief Probabilité d'un homme âgé de 23 ans d'émigrer
  */ 
  _(proba_H_23),
  /**
  * \brief Probabilité d'un homme âgé de 24 ans d'émigrer
  */ 
  _(proba_H_24),
  /**
  * \brief Probabilité d'un homme âgé de 25 ans d'émigrer
  */ 
  _(proba_H_25),
  /**
  * \brief Probabilité d'un homme âgé de 26 ans d'émigrer
  */ 
  _(proba_H_26),
  /**
  * \brief Probabilité d'une femme âgée de 18 ans d'émigrer
  */ 
  _(proba_F_18),
  /**
  * \brief Probabilité d'une femme âgée de 19 ans d'émigrer
  */
  _(proba_F_19),
  /**
  * \brief Probabilité d'une femme âgée de 20 ans d'émigrer
  */
  _(proba_F_20),
  /**
  * \brief Probabilité d'une femme âgée de 21 ans d'émigrer
  */
  _(proba_F_21),
  /**
  * \brief Probabilité d'une femme âgée de 22 ans d'émigrer
  */
  _(proba_F_22),
  /**
  * \brief Probabilité d'une femme âgée de 23 ans d'émigrer
  */
  _(proba_F_23),
  /**
  * \brief Probabilité d'une femme âgée de 24 ans d'émigrer
  */
  _(proba_F_24),
  /**
  * \brief Probabilité d'une femme âgée de 25 ans d'émigrer
  */
  _(proba_F_25),
  /**
  * \brief Cible totale de femmes émigrant entre 18 et 25 ans
  */  
  _(nbre_emig_f),
  /**
  * \brief Cible totale de hommes émigrant entre 18 et 26 ans
  */  
  _(nbre_emig_h),
  /**
  * \brief Cible totale de filles émigrant avant l'âge d'un an
  */  
  _(emigrant_F_0),
  /**
  * \brief Cible totale de garçons émigrant avant l'âge d'un an
  */
  _(emigrant_H_0)
  ;
};




/**
* \struct FinEtudeMoy
* \brief FinEtudeMoy désigne l'âge moyen de fin d'études par sexe et par génération
* 
* Source : enquête Patrimoine 2003 puis hypothèse de stabilité pour les générations récentes <br>
* Dernière mise à jour avec: <br>
* Feuille : PARAM_etude / TABLEFINDET0
*/
struct FinEtudeMoy {
  NumericVector 
  /**
  * \brief Série des âges de fin d'études moyens des hommes
  */
  _(homme),
  /**
  * \brief Série des âges de fin d'études moyens des femmes
  */
  _(femme);
};




/**
* \struct Mortalite_diff
* \brief Mortalite_diff permet de réaliser l'ancienne mortalité différentielle à la  \cite dubois2015taux .
* 
* Il s'agit des quotients de mortalité différentiel par sexe, par âge (age=a) et par âge de fin d'études relatif:
*  findet1='<-2', findet2=[-2,-1[, findet3=[-1,1[, findet4=[1,+). <br>
* Note: Ces quotients sont appliqués au Solde Migratoire (sexe=2,age=a atteint dans l'année) et à la population totale  1er janvier (age=a-1 au 1er janvier) <br>
* Source : Des travaux de Davezies et Le Minez à partir des projections démographiques Insee 2006 et de tables CS*findet relatif issues de Patrimoine 2003. <br>
* Dernière mise à jour avec: <br>
* Feuille : PARAM_mortalite_diff / mortalite_diff
*/
struct Mortalite_diff {
  NumericVector 
  /**
  * \brief quotient de mortalité des hommes avec un âge de fin d'études relatif inférieur à 2 ans à l'âge de fin d'étude moyen,
  * par âge au delà de 30 ans
  */
  _(findetH1),	
  /**
  * \brief quotient de mortalité des hommes avec un âge de fin d'études relatif inférieur de 2 ans à 1 an à l'âge de fin d'étude moyen,
  * par âge au delà de 30 ans
  */
  _(findetH2),
  /**
  * \brief quotient de mortalité des hommes avec un âge de fin d'études relatif inférieur de au plus 1 an et supérieur d'au plus 1 an à l'âge de fin d'étude moyen,
  * par âge au delà de 30 ans
  */
  _(findetH3),
  /**
  * \brief quotient de mortalité des hommes avec un âge de fin d'études relatif supérieur d'1 an à l'âge de fin d'étude moyen,
  * par âge au delà de 30 ans
  */
  _(findetH4), 
  /**
  * \brief quotient de mortalité des femmes avec un âge de fin d'études relatif inférieur à 2 ans à l'âge de fin d'étude moyen,
  * par âge au delà de 30 ans
  */
  _(findetF1),	
  /**
  * \brief quotient de mortalité des femmes avec un âge de fin d'études relatif inférieur de 2 ans à 1 an à l'âge de fin d'étude moyen,
  * par âge au delà de 30 ans
  */
  _(findetF2),
  /**
  * \brief quotient de mortalité des femmes avec un âge de fin d'études relatif inférieur de au plus 1 an et supérieur d'au plus 1 an à l'âge de fin d'étude moyen,
  * par âge au delà de 30 ans
  */
  _(findetF3),	
  /**
  * \brief quotient de mortalité des femmes avec un âge de fin d'études relatif supérieur d'1 an à l'âge de fin d'étude moyen,
  * par âge au delà de 30 ans
  */
  _(findetF4);
};

/**
* \struct Mortadiff_dip_F
* \brief Quotients de mortalité par âge et niveau de diplôme des femmes observés sur la période 2009-2013
* 
* Le calcul de ces quotients de mortalité a été réalisée par \cite blanpain2016esperance .
* Pour les années 2010 à 2012 inclues, elles ont été fournies aux fins de ces exercices 
* de modélisation par la division Enquêtes et études démographiques de l'Insee, avec l'avertissement 
* que certaines peuvent être fragiles pour certains âges fins. Pour cette raison, elles ne sont pas publiées. 
* Ces valeurs sont utilisées uniquement pour compléter des séries, et ne doivent l'être qu'à ce titre.
*/
struct Mortadiff_dip_F {
  NumericVector 
  /**
  * \brief quotients de mortalité par âge des femmes sans diplôme
  */
  _(sansdip),
  /**
  * \brief quotients de mortalité par âge des femmes de niveau Brevet-CEP
  */
  _(Brevet), 
  /**
  * \brief quotients de mortalité par âge des femmes de niveau CAP-BEP
  */
  _(CAPBEP),
  /**
  * \brief quotients de mortalité par âge des femmes de niveau baccalauréat
  */
  _(BAC),
  /**
  * \brief quotients de mortalité par âge des femmes diplômées de l'enseignement supérieur
  */ 
  _(sup);
};

/**
* \struct Mortadiff_dip_H
* \brief Quotients de mortalité par âge et niveau de diplôme des hommes observés sur la période 2009-2013
* 
*  Le calcul de ces quotients de mortalité a été réalisée par \cite blanpain2016esperance .
* Pour les années 2010 à 2012 inclues, elles ont été fournies aux fins de ces exercices 
* de modélisation par la division Enquêtes et études démographiques de l'Insee, avec l'avertissement 
* que certaines peuvent être fragiles pour certains âges fins. Pour cette raison, elles ne sont pas publiées. 
* Ces valeurs sont utilisées uniquement pour compléter des séries, et ne doivent l'être qu'à ce titre.
*/
struct Mortadiff_dip_H {
  NumericVector 
  /**
  * \brief quotients de mortalité par âge des hommes sans diplôme
  */
  _(sansdip),
  /**
  * \brief quotients de mortalité par âge des hommes de niveau Brevet-CEP
  */
  _(Brevet), 
  /**
  * \brief quotients de mortalité par âge des hommes de niveau CAP-BEP
  */
  _(CAPBEP),
  /**
  * \brief quotients de mortalité par âge des hommes de niveau baccalauréat
  */
  _(BAC),
  /**
  * \brief quotients de mortalité par âge des hommes diplômés de l'enseignement supérieur
  */ 
  _(sup);
};

struct Ech {  
  /**
  * \struct Ech
  * \brief Ech contient les caractéristiques invariantes au cours du temps des individus
  */
  IntegerVector
  /**
  * \brief Identifiant de l'individu
  */
  _(Id),  
  /**
  * \brief Sexe de l'individu
  */
  _(sexe),
  /**
  * \brief Année de naissance de l'individu
  */
  _(anaiss),  
  /**
  * \brief Mois de naissance de l'individu (identifiant modulo 12)
  */
  _(moisnaiss), 
  /**
  * \brief Âge de fin d'étude de l'individu
  */
  _(findet),  
  /**
  * \brief Indicatrice du fait d'être né en France
  */  
  _(neFrance),  
  /**
  * \brief Indicatrice du fait d'avoir émigré
  */  
  _(emigrant),
  /**
  * \brief Type de fonction publique : FPE ou FPTH
  */
  _(typeFP) ,
  /**
  * \brief Indicatrice d'être peu diplômé pour l'imputation de l'état de santé
  */  
  _(peudip),
  /**
  * \brief Indicatrice d'être très diplômé pour l'imputation de l'état de santé
  */  
  _(tresdip),
  /**
  * \brief Niveau de diplôme en cinq catégories
  */
  _(dipl);   
  NumericVector 
    /**
    * \brief Taux de prime (pour les fonctionnaires)
    */  
    _(taux_prim), 
    /**
    * \brief Paramètre pour l'option de départ uinst (utilité instantannée cible)
    */
    _(k);
};





struct Emp {
  /**
  * \struct Emp
  * \brief Emp contient les statuts sur le marché du travail et les revenus d'activité
  */
  IntegerVector  
  /**
  * \brief Identifiant de l'individu
  */
  _(Id), 
  /**
  * \brief Âge
  */
  _(age),
  /**
  * \brief Statut sur le marché du travail
  */
  _(statut);
  NumericVector 
    /**
    * \brief Revenu annuel d'activité
    */  
    _(salaire) ;
};



struct Fam {
  /**
  * \struct Fam
  * \brief Fam contient la chronique des évenements familiaux depuis 2009.
  */
  IntegerVector
  /**
  * \brief Identifiant de l'individu
  */
  _(Id), 
  /**
  * \brief Année
  */    
  _(annee), 
  /**
  * \brief Identifiant du père
  */
  _(pere),
  /**
  * \brief Identifiant du mère
  */
  _(mere),
  /**
  * \brief Statut matrimonial
  */
  _(matri), 
  /**
  * \brief Identifiant du conjoint
  */
  _(conjoint),
  /**
  * \brief Identifiant de l'enfant n°1
  */ 
  _(enf1),
  /**
  * \brief Identifiant de l'enfant n°2
  */
  _(enf2),
  /**
  * \brief Identifiant de l'enfant n°3
  */
  _(enf3),
  /**
  * \brief Identifiant de l'enfant n°4
  */
  _(enf4),
  /**
  * \brief Identifiant de l'enfant n°5
  */
  _(enf5),
  /**
  * \brief Identifiant de l'enfant n°6
  */
  _(enf6),
  /**
  * \brief Année de naissance de l'enfant n°1
  */    
  _(anaiss_enf1,0), 
  /**
  * \brief Année de naissance de l'enfant n°2
  */     
  _(anaiss_enf2,0),
  /**
  * \brief Année de naissance de l'enfant n°3
  */ 
  _(anaiss_enf3,0),
  /**
  * \brief Année de naissance de l'enfant n°4
  */ 
  _(anaiss_enf4,0),
  /**
  * \brief Année de naissance de l'enfant n°5
  */ 
  _(anaiss_enf5,0), 
  /**
  * \brief Année de naissance de l'enfant n°6
  */ 
  _(anaiss_enf6,0);
};




/**
* \struct Options
* \brief Options contient l'ensemble des options de simulation.
* 
* Les options regroupent à la fois les options réglementaires sur le calcul des droits retraites, les hypothèses 
* de comportement de liquidations des individus ou encore le pas de simulation.
*/
struct Options {  
  ///\{ \name Options réglementaires  
  bool
  /**
  *  \brief Supprime le dispositif de retraites anticipées pour les fonctionnaires de catégorie active
  */
  _(NoRetrAntFPA)          , 
  /**
  *  \brief Supprime le dispositif de retraites anticipées pour les fonctionnaires parents de 3 enfants.
  *  Ce dispositif a par ailleurs été supprimé par la réforme de 2010
  */
  _(NoRetrAntFP3Enf)     , 
  /**
  *  \brief Ne tient pas compte des réformes des retraites qui ont porté au-delà de 40 ans la durée 
  *  d'assurance nécessaire à l'atteinte du taux plein.
  */   
  _(BlockDuree40),
  /**
  * \brief Neutralise la prise en compte des périodes assimilées dans le calcul de la durée validée
  */
  _(NoAssimil),
  /**
  * \brief Neutralise les périodes d'AVPF dans le calcul de la pension au régime général (ni prise en compte 
  * de la durée validée, ni des salaires portés au compte).
  */
  _(NoAVPF), 
  /**
  * \brief Neutralise la prise en compte des trimestres de majorations de durée d'assurance (MDA) dans 
  * le calcul de a durée d'assurance
  */
  _(NoMDA), 
  /**
  * \brief Neutralise le dispositif de départ anticipé pour carrières longues
  */
  _(NoRetrAntCarrLongues),
  /**
  * \brief Supprime le dispositif de départ au taux plein pour les inaptes
  */
  _(NoInaptLiqTauxPlein),
  /**
  * \brief Supprime la borne 1 pour le coefficient de proratisation
  */
  _(NoBorneCoeffProrat), 
  /**
  * \brief Neutralise le calcul et la prise en compte des points gratuits dans les régimes complémentaires 
  * du privé (chômage, préretraite et GMP pour l'AGIRC)
  */
  _(NoPtsGratuits),
  /**
  * \brief Force le calcul séparé des SAM au RG et au RSI
  */
  _(SAMSepare)             , 
  /**
  * \brief Supprime les versements forfaitaires uniques
  */
  _(NoVFU)               , 
  /**
  * \brief Neutralise la prise en compte du SMIC comme salaire porté aux comptes pour les périodes
  *d’avpf (ces périodes restent comptabilisées dans la durée validée, comme des périodes
  *assimilées). Les personnes qui n’ont validé que des périodes d’AVPF voient leur pension au régime
  *général portée au niveau du minimum contributif.
  */
  _(NoSpcAVPF) ,
  /**
  * \brief Neutralise l’application du minimum garanti dans la fonction publique.
  */
  _(NoMG)                  , 
  /**
  * \brief Neutralise l’application du minimum contributif au régime général et dans le régime des 
  * indépendants.
  */
  _(NoMC)                , 
  /**
  * \brief Neutralise l’application de la bonification de pension pour trois enfants et plus (dans tous les
  régimes)
  */
  _(NoBonif),  
  /**
  * \brief Applique à la FP un calcul du SAM identique au RG (salaires des 25 meilleurs années)
  */
  _(FPCarTot)              ,  
  /**
  * \brief Calcul du SAM sur l’intégralité de la carrière
  */
  _(SpcToutesAnnees)     , 
  /**
  * \brief Calcul du SAM à partir d’un SAM unique pour le RSI et le RG
  */
  _(SAMRgInUnique),   
  /**
  * \brief Calcul du SAM à partir d’un SAM unique tous régimes
  */
  _(SAMUnique)             , 
  /**
  * \brief Prolonge l’augmentation des durées d’assurance pour pouvoir liquider au taux plein
  */
  _(ProlongDuree) , 
  /**
  * \brief Porte l'âge d'ouverture des droits à 64 ans
  */
  _(age64_67ans)  ,  
  /**
  * \brief Porte l'âge d'ouverture des droits et l'âge d'annulation de la décôte progressivement à 69 ans
  */
  _(age64_69ans)         , 
  /**
  * \brief Maintient les deux bornes d'âge à 60 et 65 ans
  */    
  _(age60_65ans)           , 
  /**
  * \brief Maintient l'âge d'ouvertue des droits à 60 ans mais l'âge d'anulation de la décôte est porté à 67 ans
  */
  _(age60_67ans)         ,
  /**
  * \brief Bloque la durée d'assurance nécessaire à l'obtention du taux plein au niveau de celle appliquée 
  * à la génération 1960
  */
  _(BlockDureegen1960),  
  /**
  * \brief Neutralise l'application de l'accord AGIRC-ARRCO de 2015 (hormis les valeurs des revalorisations 
  * de la valeur de service introduite dans le fichier de simulation)
  */
  _(NoAccordAgircArrco)    ,  
  /**
  * \brief Ne crée pas le régime unique des complémentaires du privé à partir de 2019 
  */
  _(NoRegUniqAgircArrco), 
  /**
  * \brief N'applique pas les coefficients temporaires des régimes complémentaires du privé décidés dans l'accord de 2015
  */
  _(NoCoeffTemp),
  /**
  *  \brief Ne tient pas compte de la majortation de la durée d'assurance pour âge (en y ajoutant l'option NoMDA)
  */
  _(NoMDAAge);
  ///\}
  bool
    ///\{ \name Options sur le mode de liquidation des assurés      
    /**
    * \brief Comportement de départ en fonction d'une utilité mixte
    */
    _(umixt),
    /**
    * \brief Comportement de départ avec utilité instantannée
    */
    _(uinst)   ,     
    /**
    * \brief Liquidation des assurés au taux plein
    */
    _(tp)      , 
    /**
    * \brief Option utilisée dans la méthode DecoteSurcote() afin d'imputer la liquidation pour inaptitude,
    * auquel cas celle-ci se fait sans décote. Les liquidations pour inaptitude concernent les individus dont le
    * taux de préférence (X.k) pour l'inactivité est supérieur à un certain seuil. Lorsque inapte_exo=TRUE, X.k=1,6
    */
    _(inapte_exo)        ;    
  ///\}
  bool 
    _(plafecretMinContSMPT)  ,  _(coeff_demo)          ,    
    _(ecrit_dr_test),  
    _(cotisations)           ,  _(redresseSal)         ,  _(tirage_simple_naiss),
    _(FM)					 ,  _(sante) 			   ,  _(mort_diff_dip),
    _(mort_diff);  
  int 
    _(comp,comp_tp) ,  _(anLeg,2013) ,  _(AN_MAX)     ;
  
  double
    _(pas1,3/12.0),  
    _(pas2,1.0)           ,  _(k_median_ho, 1.6) ,  _(k_median_fe, 1.6),  
    _(k_def_q_ho, 0.01)   ,  _(k_def_q_fe, 0.01) ,  _(k_val_q_ho, 1.0) ,  
    _(k_val_q_fe, 1.0)    ,  _(delta_k, 0.01)    ,  _(seedAlea,12345689.0),
    _(m_incapacite,0.0);
  
  bool
    _(SalNoEffetLinGen60)           ,  _(redresseSalNoFem)   ,
    _(redresseCar)         ,  _(redresseFindet)        ,
    _(prolongeDuree2013)     ,  _(effet_hrzn)          ,  _(super_effet_hrzn)   ,
    _(effet_hrzn_indiv)      ,  _(SalNoEffetGen)       ,  _(transNonCalees)	,
    _(DepInact), _(SecondLiq) ;
};




/**
* \struct Macro
* \brief Macro contient l'ensemble des paramètres macroéconomiques et des paramètres spécifiques aux retraites.
* 
* Macro contient l'ensemble des paramètres macroéconomiques (PIB, inflation, ...) mais aussi des paramètres plus spécifiques
* aux retraites (taux de cotisation, valeur des points dans les complémentaires, ou encore montant du minimum vieillesse).
*/


struct Macro {
  NumericVector 
  /**
  * \brief Indice des Prix à la Consommation (IPC) France entière, y compris tabac 
  * (évolution annuelle appliquée à la série initiale à partir de 2006) <br>
  * Source : Insee <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamGene 
  */  
  _(Prix),
  /**
  * \brief Plafond de la Sécurité Sociale, en euros courants <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamGene 
  */
  _(PlafondSS),
  /**
  * \brief Montant annuel du SMIC, tel qu'utilisé pour déterminer les cotisations AVPF
  * (2028 heures par an (169*12) - 39 heures par semaine), moyenne annuelle <br>
  * Source : Insee <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamGene 
  */  
  _(SMIC),  
  /**
  * \brief Valeur du point d'indice de la Fonction Publique, en euros courants <br>
  * Source :  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamGene
  */  
  _(PointFP),
  /**
  * \brief Salaire Moyen Par Tête (SMPT) dans l'ensemble de l'économie <br>
  * Source : Insee <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamGene
  */  
  _(SMPT),
  /**
  * \brief Produit Intérieur Brut (PIB), en euros courants <br>
  * Source :  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamGene
  */  
  _(PIB), 
  
  
  /**
  *  \brief  Évolution en projection du SMPT réel (ensemble de l'économie) <br>
  * Source : COR, scénario défini en entrée du modèle <br>
  * Feuille : ParamSociauxXXXX / choX%_pibY%
  */
  _(SMPTp),
  /**
  *  \brief Évolution en projection du PIB en volume<br>
  * Source : COR, scénario défini en entrée du modèle <br>
  * Feuille : ParamSociauxXXXX / choX%_pibY%
  */
  _(PIBp),
  /**
  *  \brief Évolution en projection du Plafond de la Sécurité Sociale <br>
  * Source : COR, scénario défini en entrée du modèle <br>
  * Feuille : ParamSociauxXXXX / choX%_pibY%
  */
  _(PlafondSSp),
  /**
  *  \brief Évolution en projection du SMIC <br>
  * Source : COR, scénario défini en entrée du modèle <br>
  * Feuille : ParamSociauxXXXX / choX%_pibY%
  */
  _(SMICp),
  /**
  *  \brief Évolution en projection de l'indice des prix <br>
  * Source : COR, scénario défini en entrée du modèle <br>
  * Feuille : ParamSociauxXXXX / choX%_pibY%
  */
  _(Prixp),
  /**
  * \brief Pour l'année N, ratio (coefficent de revalorisation du Salaire Porté au Compte (SPC) de l'année N / 
  * coefficent de revalorisation du SPC de l'année N+1) <br>
  * Source : CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(RevaloSPC), 
  /** 
  * \brief Revalorisation des pensions liquidées au Régime Général 
  * Si plusieurs valeurs au cours d'une année N, on chaîne les coefficient en les multipliant <br>
  * Source : CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */
  _(RevaloRG), 
  /**
  * \brief Revalorisation des pensions liquidées pour la Fonction Publique <br>
  * Source initiale: Drees, Taux de revalorisation des pensions dans la Fonction Publique 
  * utilisé par la DREES dans son modèle CALIPER  <br>
  * Dernière mise à jour avec: ServicePublic.fr <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */ _(RevaloFP), 
  /**
  * \brief Taux de cotisation salarié, sous le plafond, au Régime Général <br>
  * Source : CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */     
  _(TauxSalRGSP), 
  /**
  * \brief Taux de cotisation employeur, sous le plafond, au Régime Général <br>
  * Source : CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(TauxEmpRGSP), 
  /**
  * \brief Taux de cotisation salarié portant spécifiquement sur le salaire total, 
  * au Régime Général (=assurance veuvage avant 2004) <br>
  * Source : CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(TauxSalRGSalTot), 
  /**
  * \brief Taux de cotisation employeur portant spécifiquement sur le salaire total, 
  * au Régime Général <br>
  * Source : CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */     
  _(TauxEmpRGSalTot), 
  /**
  * \brief Taux de cotisation Employé dans la Fonction Publique.  <br>
  * Note : D'ici à 2020 la loi de 2010 prévoit l'augmentation du taux de cotisation à 10,55%. <br>
  * Source : IPP <br>
  * Dernière mise à jour avec: IPP <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(TauxFP), 
  /**
  * \brief Taux de cotisation (sous le plafond de la Sécurité Sociale) 
  * des travailleurs indépendants (ex-RSI) <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(TauxRSIssP), 
  /**
  * \brief Taux de cotisation (au dessus du plafond de la Sécurité Sociale) 
  * des travailleurs indépendants (ex-RSI) <br>
  * Source:   <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(TauxRSIsurP), 
  /**
  * \brief Taux de cotisation Employeur au sein de la Fonction Publique de l'Etat
  * Taux qui équilibre le solde élargi en 2011 figé militaires+civils. Pour le passé, 
  * en rétropolation et on reprend avant 2006 chiffres du Jaune Budgétaire. <br>
  * Sources: COR et Jaune Budgétaire. <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(TauxEmplFPE), 
  /**
  * \brief Taux de cotisation Employeur à la CNRACL (pour la fonction publique Territoriale et Hospitalière) <br>
  * Source: COR <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(TauxEmplFPTH), 
  /**
  * \brief Pondération selon parts masses salariales FPE  et FPT/H.  <br>
  * Source: Calculs à partir données DGAFP. <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(TauxEmplFPMoy), 
  /**
  * \brief  Minimum Vieillesse pour une personne seule <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */     
  _(MinVieil1), 
  /**
  * \brief Minimum Vieillesse pour une personne en couple <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(MinVieil2), 
  /**
  * \brief Minimum contributif <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(Mincont1), 
  /**
  * \brief Minimum contributif majoré (pour les personnes totalisant un nombre suffisant
  * de trimestres effectivement cotisés) <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(Mincont2), 
  /**
  * \brief Majoration de durée d'assurance pour âge, au Régime général <br>
  * Pour une personne ayant eu le taux plein par condition sur l'âge, permet de surcoter
  * voir par exemple : http://www.legislation.cnav.fr/Pages/expose.aspx?Nom=mda_mda_age_mda_age_ex <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */   
  _(CoefMdaRG), 
  /**
  * \brief Majoration de durée d'assurance pour âge, dans la Fonction Publique
  * Pour une personne ayant eu le taux plein par condition sur l'âge, permet de surcoter
  * voir par exemple : http://www.legislation.cnav.fr/Pages/expose.aspx?Nom=mda_mda_age_mda_age_ex <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(CoefMdaFP), 
  /**
  * \brief Seuil du Versement Forfaitaire Unique (VFU) au régime général <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(SeuilVFURG), 
  /**
  * \brief Salaire validant un trimestre au régime général <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrBase
  */  
  _(SalValid),  
  /**
  * \brief Taux de cotisation moyen à l'ARRCO sur la tranche 1 <br>
  * Source: COR <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxARRCO_1), 
  /**
  * \brief  Taux de cotisation moyen à l'ARRCO sur la tranche 2 <br>
  * Source: COR <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxARRCO_2), 
  /**
  * \brief Taux de cotisation employé moyen à l'ARRCO sur la tranche 1 <br>
  * Source: Calcul : 40% de _(TauxARRCO_1) <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxARRCO_S1) , 
  /**
  * \brief Taux de cotisation employé moyen à l'ARRCO sur la tranche 2 <br>
  * Source: Calcul : 40% de _(TauxARRCO_2) <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxARRCO_S2), 
  /**
  * \brief Taux d'appel à l'ARRCO <br>
  * Source: COR <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxAppARRCO), 
  /**
  * \brief Salaire de référence ARRCO (i.e. valeur d'achat du point UNIRS/ARRCO) <br>
  * Source: AGIRC/ARRCO <br>
  * Dernière mise à jour avec: IPP <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(SalRefARRCO), 
  /**
  * \brief Valeur de service du point UNIRS/ARRCO <br>
  * Source: AGIRC/ARRCO <br>
  * Dernière mise à jour avec: IPP <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(ValPtARRCO), 
  /**
  * \brief Taux cotisation maladie sur les retraites complémentaires <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxMalComp), 
  /**
  * \brief Taux de cotisation total à l'AGIRC sur la tranche B <br>
  * Source: COR <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxAGIRC_B), 
  /**
  * \brief  Taux de cotisation total à l'AGIRC sur la tranche C <br>
  * Source: COR <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxAGIRC_C) , 
  /**
  * \brief  Taux de cotisation salarié à l'AGIRC sur la tranche B <br>
  * Source:  Calcul : 40% de _(TauxAGIRC_B) <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxAGIRC_SB), 
  /**
  * \brief  Taux de cotisation salarié à l'AGIRC sur la tranche C <br>
  * Source:  Calcul : 40% de _(TauxAGIRC_C) <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxAGIRC_SC), 
  /**
  * \brief Taux d'appel à l'AGIRC <br>
  * Source: COR <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxAppAGIRC), 
  /**
  * \brief Salaire de référence AGIRC (i.e. valeur d'achat du point AGIRC) <br>
  * Source: AGIRC/ARRCO <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(SalRefAGIRC), 
  /**
  * \brief Valeur de service du point AGIRC <br>
  * Source: AGIRC/ARRCO <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(ValPtAGIRC),
  
  /**
  * \brief Garantie Minimale de Point (GMP) à l'AGIRC <br>
  * Source: AGIRC/ARRCO <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(GMP),  
  /**
  * \brief Taux de cotisation total au régime unifié AGIRC/ARRCO, sur la tranche 1 <br>
  * Source: AGIRC/ARRCO <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxAGIRC_ARRCO_1), 
  /**
  * \brief Taux de cotisation total au régime unifié AGIRC/ARRCO, sur la tranche 2 <br>
  * Source: AGIRC/ARRCO <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxAGIRC_ARRCO_2) ,  
  /**
  * \brief Taux d'appel au régime unifié AGIRC/ARRCO <br>
  * Source: AGIRC/ARRCO <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxAppAGIRC_ARRCO), 
  /**
  * \brief Taux de cotisation salarié au régime unifié AGIRC/ARRCO, sur la tranche 1 <br>
  * Source: Calcul : 40% de _(TauxAGIRC_ARRCO_1) <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxAGIRC_ARRCO_S1) , 
  /**
  * \brief Taux de cotisation salarié au régime unifié AGIRC/ARRCO, sur la tranche 1 <br>
  * Source: Calcul : 40% de _(TauxAGIRC_ARRCO_2) <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(TauxAGIRC_ARRCO_S2),  
  /**
  * \brief Salaire de référence AGIRC/ARRCO (i.e. valeur d'achat du point AGIRC/ARRCO) <br>
  * Source: AGIRC/ARRCO <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(SalRefAGIRC_ARRCO), 
  /**
  * \brief Valeur de service du point AGIRC/ARRCO <br>
  * Source: AGIRC/ARRCO <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRetrComp
  */  
  _(ValPtAGIRC_ARRCO), 
  /**
  * \brief Taux de réversion dans le régime unifié AGIRC/ARRCO
  * Source: AGIRC/ARRCO <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRev
  */  
  _(TauxRevAGIRC_ARRCO),
  /**
  * \brief Taux de réversion au Régime général <br>
  * Note: Laissé à 54% (depuis 2010 majoration de 11,1% 
  * pour les faibles pensions) <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRev
  */  
  _(TauxRevRG) , 
  /**
  * \brief Minimum de pension de réversion au régime général, en euros courants <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRev
  */  
  _(MinRevRG), 
  /**
  * \brief Maximum de pension de réversion au régime général, en euros courants <br>
  * Source: Calcul: 0,27 fois le plafond de la sécurité sociale <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRev
  */  
  _(MaxRevRG) ,  
  /**
  * \brief Plafond de ressources pour toucher la réversion, au régime général <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRev
  */  
  _(PlafRevRG) , 
  /**
  * \brief Taux de réversion à l'ARRCO <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRev
  */  
  _(TauxRevARRCO) ,  
  /**
  * \brief Taux de réversion à l'AGIRC <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRev
  */  
  _(TauxRevAGIRC),  
  /**
  * \brief Taux de réversion dans la fonction publique <br>
  * Source:  <br>
  * Dernière mise à jour avec:  <br>
  * Feuille : ParamSociauxXXXX / ParamRev
  */  
  _(TauxRevFP) ,  
  /**
  * \brief Taux de réversion chez les indépendants <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamRev
  */  
  _(TauxRevInd) ,
  /**
  * \brief Plafond correspondent à l'allocation spéciale du fond national pour l'emploi (ASFNE) <br>
  * Note: N'est plus utilisée dans le modèle, voir AgeEligPR <br>
  * Source: Dares <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamPreRet
  */  
  _(TauxPR1,0), 
  /**
  * \brief Plafond correspondent à l'allocation spéciale du fond national pour l'emploi (ASFNE)  <br>
  * Note: N'est plus utilisée dans le modèle, voir AgeEligPR <br>
  * Source: Dares <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamPreRet
  */  
  _(TauxPR2,0),  
  /**
  * \brief Plafond correspondent à l'allocation spéciale du fond national pour l'emploi (ASFNE) <br> 
  * Note: N'est plus utilisée dans le modèle, voir AgeEligPR <br>
  * Source: Dares <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamPreRet
  */  
  _(TauxPR3,0)  , 
  /**
  * \brief Âge correspondent à l'allocation spéciale du fond national pour l'emploi (ASFNE)  <br>
  * N'est plus utilisé dans le modèle. <br>
  * Note (site Pole-Emploi): "L'allocation de préretraite licenciement (ASFNE) est financée par l’Etat avec 
  * la participation de l’employeur, du salarié et de l’assurance chômage.
  * Elle est versée aux personnes de 57 ans et plus dont l’entreprise a conclu avec l'Etat une convention 
  * d’allocations spéciales du Fonds National de l’Emploi. L'article numéro 152 de la loi de finances 
  * numéro 2011-1977 du 28 décembre 2011 procéde à la suppression définitive du dispositif mais les 
  * conventions conclues antérieurement au 1er janvier 2012 continuent à produire leurs effets jusqu'à leur terme." <br>
  * Source: Dares <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamPreRet
  */  
  _(AgeEligPR,0) ,
  /**
  * \brief  Base mensuelle de calcul des allocations familiales (BMAF) <br>
  * Source: CNAF <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(BMAF) , 
  /**
  * \brief Plafond de ressources du complément familial (1 enfant) <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Note 1: une condition pour valider des périodes en AVPF est d'avoir perçu des allocations tel le complément familial,
  * (plus parfois une seconde condition de ressource propre à l'AVPF). Les PlafCF correspondent aux plafonds pour
  * le complément familial, indépendemment de l'AVPF. <br>
  * Note 2: En 2018, le complément familial est  versé, sous certaines conditions, aux personnes 
  * ayant au moins 3 enfants à charge. Historiquement, il a pu être versé dès 1 enfant, d'où probablement la définition
  * de plafonds dès 1 enfant. Il se peut aussi qu'il corresponde dans l'historique aussi à d'autres allocations. <br>
  * Note 3: (présente dans la documentation historique de Destinie)
  * L'allocation pour jeune enfant (APJE) est intégrée à la PAJE (prestation d'accueil pour jeune enfant)
  * depuis 2004. La valeur de 2007 est obtenue par moyenne des deux valeurs Cnaf (avant/après juillet 2007), 
  * puis par rétropolation de la série par application de l'évolution Cnaf. <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(PlafCF1) , 
  /**
  * \brief Plafond de ressources du complément familial (2 enfants) <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Notes: voir PlafCF1 <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(PlafCF2) , 
  /**
  * \brief Plafond de ressources du complément familial (3 enfants) <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Notes:  voir PlafCF1 <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(PlafCF3) , 
  /**
  * \brief Plafond de ressources  du complément familial (4 enfants) <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Notes:  voir PlafCF1 <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(PlafCF4) ,  
  /**
  * \brief Plafond de ressources du complément familial (enfants au delà de 4 enfants) <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Notes:  voir PlafCF1 <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(PlafCF5) , 
  /**
  * \brief Majoration du plafond ressources du complément familial  <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Notes:  voir PlafCF1 <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(MajoPlafCF)  ,  
  /**
  * \brief Plafond de ressources pour recevoir l'Allocation spéciale de rentrée, pour 1 enfant <br>
  * Note 1: Ces plafonds PlafARS sont utilisés car ce sont les conditions de ressources spécifiques pour
  * être affilié à l'AVPF. <br>
  * Note 2: La valeur de 2007 est la valeur Cnaf. Les précédentes sont obtenues par rétropolation 
  * de la série par application de l'évolution Cnaf <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(PlafARS1,0), 
  /**
  * \brief Plafond de ressources pour recevoir l'Allocation spéciale de rentrée, pour 2 enfants
  * Note: La valeur de 2007 est la valeur Cnaf. Les précédentes sont obtenues par rétropolation 
  * de la série par application de l'évolution Cnaf <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(PlafARS2,0) , 
  /**
  * \brief Plafond de ressources pour recevoir l'Allocation spéciale de rentrée, pour 3 enfants
  * Note: La valeur de 2007 est la valeur Cnaf. Les précédentes sont obtenues par rétropolation 
  * de la série par application de l'évolution Cnaf <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(PlafARS3,0)  , 
  /**
  * \brief Plafond de ressources pour recevoir l'Allocation spéciale de rentrée, pour 4 enfants
  * Note: La valeur de 2007 est la valeur Cnaf. Les précédentes sont obtenues par rétropolation 
  * de la série par application de l'évolution Cnaf <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(PlafARS4,0) , 
  /**
  * \brief Plafond de ressources pour recevoir l'Allocation spéciale de rentrée, quand plus de 4 enfants
  * Note: La valeur de 2007 est la valeur Cnaf. Les précédentes sont obtenues par rétropolation 
  * de la série par application de l'évolution Cnaf <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamFam 
  */ 
  _(PlafARS5,0)  ,
  /**
  * \brief Plafond de ressources pour l'allocation de Base (AB) de la PAJE (prestation d'accueil pour jeune enfant),
  * 1 enfant <br>
  * Note 1: une condition pour valider des périodes en AVPF est d'avoir perçu des allocations dont la PAJE-AB <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  
  */  
  _(PlafAB1,0) ,  
  /**
  * \brief Plafond de ressources pour l'allocation de Base (AB) de la PAJE (prestation d'accueil pour jeune enfant),
  * 2 enfants <br>
  * Note 1: une condition pour valider des périodes en AVPF est d'avoir perçu des allocations dont la PAJE-AB <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(PlafAB2,0) , 
  /**
  * \brief Plafond de ressources pour l'allocation de Base (AB) de la PAJE (prestation d'accueil pour jeune enfant),
  * 3 enfants <br>
  * Note 1: une condition pour valider des périodes en AVPF est d'avoir perçu des allocations dont la PAJE-AB <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(PlafAB3,0), 
  /**
  * \brief Plafond de ressources pour l'allocation de Base (AB) de la PAJE (prestation d'accueil pour jeune enfant),
  * 4 enfants <br>
  * Note 1: une condition pour valider des périodes en AVPF est d'avoir perçu des allocations dont la PAJE-AB <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(PlafAB4,0) ,  
  /**
  * \brief Plafond de ressources pour l'allocation de Base (AB) de la PAJE (prestation d'accueil pour jeune enfant),
  * plus de 4 enfants <br>
  * Note 1: une condition pour valider des périodes en AVPF est d'avoir perçu des allocations dont la PAJE-AB <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamFam
  */  
  _(PlafAB5,0) ,
  /**
  * \brief Taux de cotisation maladie salarié 
  * dans le secteur privé, sous le plafond
  * de la sécurité sociale <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxMalSP,0)  , 
  /**
  * \brief Taux de cotisation maladie des
  * indépendants <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxMalRSI,0) ,
  /**
  * \brief Taux de cotisation maladie salarié 
  * dans le secteur privé,  <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxMalTot), 
  /**
  * \brief Taux salarié pour fonctionnaire, sur l'ensemble du salaire 
  * (traitement et toutes les primes) <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxMalPubTot) ,  
  /**
  * \brief Taux de cotisation maladie salarié des fonctionnaires, 
  * seulement sur le traitement et la nouvelle bonification indiciaire (NBI) <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */   
  _(TauxMalPubTr) , 
  /**
  * \brief Taux Fond de Solidarité (FP) <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(Taux_FDS), 
  /**
  * \brief  Taux de cotisation pour Association pour le financement du fonds de financement 
  * de l'Agirc et de l'Arrco (AGFF) tranche 1 <br>
  * Source: IPP <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxAGFF_1), 
  /**
  * \brief Taux de cotisation pour Association pour le financement du fonds de financement 
  * de l'Agirc et de l'Arrco (AGFF) tranche 2	 <br>
  * Source: IPP <br>
  * Dernière mise à jour avec:  <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxAGFF_2) , 
  /**
  * \brief  Taux de cotisation pour Association pour le financement du fonds de financement 
  * de l'Agirc et de l'Arrco (AGFF) tranche 1,	Part salarié <br>
  * Source: IPP <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxAGFF_S1) , 
  /**
  * \brief Taux de cotisation pour Association pour le financement du fonds de financement 
  * de l'Agirc et de l'Arrco (AGFF) tranche 2,	Part salarié <br>
  * Source: IPP <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxAGFF_S2), 
  /**
  * \brief Taux de cotisation Assédic (chômage) <br>
  * Source: IPP <br>
  * Dernière mise à jour avec: IPP <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxAssedic) , 
  /**
  * \brief Taux de cotisation de l'Association pour l’emploi des cadres (APEC) <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(Taux_APEC), 
  /**
  * \brief 	Taux Contribution Sociale Généralisée (CSG), part salarié,
  *  et taux de la contribution à la réduction de la dette sociale (CRDS) <br>
  * Source: Calcul:(taux csg + taux crds)*(1-abattement csg sous 4 PSS) <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxCSGSal) , 
  /**
  * \brief Taux de cotisation maladie des retraités (existait jusqu'en 1997 inclue) <br>
  * Note: Avant  1998, il y avait une cotisation d'assurance maladie, instituée à compter du 01/07/1980. 
  * Elle était prélevée sur la retraite des personnes soumises à l'impôt sur le revenu.  <br>
  * Source: http://www.legislation.cnav.fr/Pages/expose.aspx?Nom=prelevement_retraite_ancienne_disposition_ex <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxMalRet) ,  
  /**
  * \brief Taux de cotisation maladie prélevé sur les retraites complémentaires. <br>
  * Source : https://www.agirc-arrco.fr/particuliers/vivre-retraite/pension-retraite-versement/#c154 <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */ 
  _(TauxMalRetFra) ,  
  /**
  * \brief Taux de cotisation maladie des retraités domiciliés hors de France ou de certains retraités étrangers vivant en France. <br>
  * Note 1 : Le retraité est soumis au prélèvement d'une cotisation d'assurance maladie s'il est domicilié fiscalement 
  * hors de France et relève à titre obligatoire d'un régime français d'assurance maladie. La cotisation est également 
  * prélevée sur la retraite des personnes de nationalité étrangère qui ne relèvent pas à titre obligatoire d'un régime 
  * français d'assurance maladie, mais réunissent au moins 15 ans d'assurance en France. La cotisation prélevée à tort 
  * à l'assuré qui ne réunit pas 15 ans d'assurance est remboursée sans application de la prescription. <br>
  * Note 2 : actuellement pas utilisé dans le modèle Destinie 2. <br>
  * Source: http://www.legislation.cnav.fr/Pages/expose.aspx?Nom=prelevement_retraite_cotisation_assurance_maladie_ex <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxMalRetEtr) ,  
  /**
  * \brief Taux fort de la Contribution Sociale Généralisée (CSG) + 
  *  Contribution pour le remboursement de la dette sociale (CRDS) + 
  *  Contribution additionnelle de solidarité pour l'autonomie (Casa) <br>
  * Note 1 : La contribution sociale généralisée (CSG) est prélevée sur le montant brut des avantages de vieillesse sauf 
  * la majoration pour tierce personne et le versemenrt exceptionnel payé en 2015. Le prélèvement est effectué par 
  * l'organisme qui paie ces avantages. La CSG est prélevée même si la retraite est payée en un versement forfaitaire unique.
  * Il existe 2 taux différents de CSG (un taux fort et un taux minoré). Le taux de la CSG prélevée dépend du 
  * revenu fiscal de référence compte tenu du nombre de parts fiscales et du lieu de résidence. Avant 2015, le taux de 
  * la CSG dépendait de la cotisation d'impôt. La  Contribution additionnelle de solidarité pour l'autonomie (Casa) ne s'applique qu'aux retraités
  * soumis au taux fort de la CSG. <br>
  * Note 2 :	Ajout de la contribution de 0,3% à partir de 2013 (pour financer la dépendance) <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: http://www.legislation.cnav.fr/Pages/bareme.aspx?Nom=prelevement_retraite_taux_prelevement_retraite_bar <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxCSGRetFort), 
  /**
  * \brief Taux minoré de la Contribution Sociale Généralisée (CSG) +CRDS <br>
  * Note:  voir TauxCSGRetFort <br>
  * Source: CNAV <br>
  * Dernière mise à jour avec: http://www.legislation.cnav.fr/Pages/bareme.aspx?Nom=prelevement_retraite_taux_prelevement_retraite_bar <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxCSGRetMin) , 
  /**
  * \brief Seuil d'exonération à la Contribution Sociale Généralisée (CSG) pour la retraite, quand 1 part fiscale <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(SeuilExoCSG)  , 
  /**
  * \brief  Seuil d'exonération à la Contribution Sociale Généralisée (CSG) pour la retraite, quand 2 parts fiscales <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(SeuilExoCSG2), 
  /**
  * \brief Seuil d'aplication du taux réduit de la Contribution Sociale Généralisée (CSG) quand 1 part fiscale <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(SeuilTxReduitCSG) , 
  /**
  * \brief Seuil d'aplication du taux réduit de la Contribution Sociale Généralisée (CSG) quand 2 parts fiscales <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(SeuilTxReduitCSG2) , 
  /**
  * \brief Série des seuils de pauvreté, à 60\% du niveau de vie médian <br>
  * Source: INSEE/COR <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(SeuilPauvrete)     , 
  
  /**
  * \brief Contribution exceptionnelle temporaire (CET), part salariés <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxCET_S)		, 
  /**
  * \brief Contribution exceptionnelle temporaire (CET), totale <br>
  * Source:  <br>
  * Dernière mise à jour avec: <br>
  * Feuille : ParamSociauxXXXX / ParamAutres
  */  
  _(TauxCET),
  
  /**
  * \brief Correcteur démographique <br>
  * Source: Calculé dans la fonction interne de Destinie.cpp : destinieSimOptim. N'est utilisé que
  * dans le cas où l'option options->coeff_demo est mise en oeuvre.
  * Voir \cite dubois_koubi_2017
  */  
  _(correct_demo,0)  ;
  
  
  NumericVector daterevalobase;      		///< Mois de revalorisation des régimes de base 
  NumericVector   daterevalocomp;		   	///< Mois de revalorisation des régimes complémentaires
  //correct_demo,
  NumericVector   RevaloCumFP;         		///< Revalorisations cumulées des pensions liquidées au FP
  NumericVector   RevaloCumRG;         		///< Revalorisations cumulées des pensions liquidées au RG
  //coeff_correc_structure
  ;  
  /// Tables de survie
  vector<NumericMatrix> q_mort;  ///< Tables des quotients de mortalité
  vector<NumericMatrix> survie;  ///< Tables des taux de survie
  vector<NumericMatrix> espvie;  ///< Tables des espérance de vie
  
  
  double poids;                  ///< Pondération de la population
};


/**
* \struct EqSante
* \brief EqSante contient les coefficients pour les équations de santé (adl, iadl, gali).
*  
* Il s'agit des paramètres nécessaires à l'estimation des états de santé (à partir de 50 ans) décrits par
* \cite evrsi : les restrictions dans les activités de la vie quotidienne (adl),
* les restrictions dans les activités instrumentales de la vie quotidienne (iadl), 
* et l'indicateur du gali.  <br>
* Il y a trois types d'équations (dépendant du sexe) : des équations de prévalence (pour attribuer un état de santé à la date
* initiale ou à 50 ans), des équations d'incidence (pour faire varier cet état de santé, dépendant de l'état de santé
* deux ans avant), et des équations de mortalité (dépendant de l'état de santé deux ans avant). <br>
* Source: \cite evrsi sur la partie française de l'enquête SHARE.
*/
struct EqSante {
  double   
  /**
  * \brief Coefficient devant la constante
  */
  _(Intercept),   
  /**
  * \brief Coefficient devant le fait d'être en couple
  */
  _(couple),    
  /**
  * \brief Coefficient devant le fait d'avoir 4 enfants ou plus
  */
  _(enf4plus) ,    
  /**
  * \brief Coefficient devant le fait d'avoir 0 ou 1 enfant
  */
  _(enf01),    
  /**
  * \brief Coefficient devant le fait d'être très diplomé par rapport à sa génération et son sexe
  */
  _(tresdip),   
  /**
  * \brief Coefficient devant le fait d'être peu diplomé par rapport à sa génération et son sexe
  */
  _(peudip),   
  /**
  * \brief Coefficient devant l'âge moins 50.
  * Note: dans les simulations, un âge encore modifié peut être employé pour tenir compte des scénario
  * d'évolution des incapacités, voir \cite evrsi .
  */
  _(agebis) ,   
  /**
  * \brief Coefficient  devant l'âge moins 50, au carré.
  */
  _(agebis2),   
  /**
  * \brief Coefficient  devant l'âge moins 50, au cube.
  */
  _(agebis3),    
  /**
  * \brief Coefficient devant l'état de santé, quand pertinent.
  */
  _(sante);    
};





/**
* \struct Simulation
* \brief La structure Simulation contient l'ensemble des paramètres, options et séries macroéconomique ou de paramètres
* législatifs  de la simulation, ainsi que le vecteur d'individus pop qui contient l'ensemble des individus.
* 
*/

struct Simulation {
  static double heure_debut;      ///< Heure de début de la simulation
  double seed;                    ///< Graine pour le générateur de nombres aléatoires
  Environment envSim;             ///< Environnement R contenant les paramètre de la simulation            
  Context context;                        
  Simulation(Environment env);    ///< Constructeur d'un object Simulation à partir d'un environment R
  ~Simulation();                  ///< Destructeur
  
  /**
  * \brief Tire un nombre aléatoire de loi uniforme dans l'intervalle [0,1] 
  * 
  * Tire un nombre aléatoire de loi uniforme dans l'intervalle [0,1],
  * la graine peut être fixée avec la variable \ref seed.
  */
  /*double alea() {
  static const double alea_a = 16807.0;
  static const double alea_m = 2147483647.0;
  static const double alea_q = 127773.0;
  static const double alea_r = 2836.0;  
  
  double alea_hi   = int(seed/alea_q);
  double alea_lo   = seed - alea_q*alea_hi;
  double alea_test = alea_a*alea_lo - alea_r*alea_hi;
  seed      = (alea_test > 0.0) ? alea_test : (alea_test+alea_m);
  return seed/alea_m;
}*/
  };




void ecriture_aide();
void ecriture_liquidations();     ///< Crée une table R liquidations
void ecriture_retraites(int t);   ///< Exporte dans la table R retraite l'ensemble des pensions versées dans l'année
void ecriture_droitsRetr(Indiv& X, DroitsRetr& r);




/**
* Affiche la durée écoulée depuis le début de la simulation
*/
inline void pointage(string str) {
  Rprintf("%s, temps ecoule : %f s\n", str.c_str(), (clock() - Simulation::heure_debut)/1000.0);
}



extern ptr<Macro>           M;                  ///< pointeur qui contient les paramètres macroéconomiques
extern ptr<Options>         options;            ///< pointeur qui contient les options de législation
extern ptr<Mortalite_diff>  mortalite_diff ;   
extern ptr<Mortadiff_dip_F> mortadiff_dip_F; 
extern ptr<Mortadiff_dip_H> mortadiff_dip_H ; 
extern ptr<FinEtudeMoy>     finEtudeMoy  ;
extern ptr<CiblesDemo>      ciblesDemo;    

extern vector<Indiv> pop;           ///< tableau de l'ensemble des individus (cf. classe \ref Indiv)
extern  int AN_FIN;                 ///< Année de fin de la simulation
extern  int AN_NB;                  ///< Nombre d'années (AN_FIN+1)
extern vector<EqSante> eq_sante ;   ///< Equations régissant les transitions entre états de santé dans l'option santé




// Destinie 2
// Copyright © 2005-2018, Institut national de la statistique et des études économiques
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.