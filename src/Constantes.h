

/**
 * \file Constantes.h
 * \brief Fichier contenant les constantes générales
 */
#pragma once
#include "OutilsBase.h" 
#include "Simulation.h"

const int AN_BASE = 109;      ///< Année base de l'échantillon
const int AGE_MAX = 128;      ///< Âge en vie maximal
const int NB_ENF_MAX = 6;     ///< Nombre maximal d'enfants


// Modalités des variables

enum code_mode_regime {
  ACTUEL=1,
  ACTUEL_MODIF=2,
  DELEVOYE=3
} ;

/// Codes sexe :
enum code_sexe {
  HOMME=1,    ///< Sexe : modalité HOMME
  FEMME=2     ///< Sexe : modalité FEMME
} ;

/// Enumération des statuts matrimoniaux :
enum code_matri {
  CELIB=1,      ///< Statut matrimonial : modalité célibataire
  MARIE=2,      ///< Statut matrimonial : modalité marié(e)
  VEUF=3,       ///< Statut matrimonial : modalité veuf/veuve
  SEPAR=4,      ///< Statut matrimonial : modalité séparé(e)
  CONCUBINAGE=5 ///< Statut matrimonial : modalité en concubinage ou pacsé(e)
};

/// Enumération des codes diplôme :
enum code_dipl {
  SSDIPL=1,     ///< sans diplôme
  BREVET=2,     ///< brevet, CEP
  CAP=3,        ///< CAP, BEP
  BAC=4,        ///< baccalauréat
  UNIV=5        ///< diplômé du supérieur
};

/// Codes de la variable individuelle typeFP 
enum code_typeFP {
  FPE,        ///< Type de fonction publique : modalité fonction publique d'Etat 
  FPTH,       ///< Type de fonction publique : modalité fonction publique territoriale ou hospitaliaire
  NONFP       ///< Type de fonction publique : modalité n'appartient pas à la fonction publique
};

/// Codes de comportement de liquidation des droits directs
enum comportement {
  comp_tp,    ///< Départ au taux plein
  comp_uinst, ///< Départ avec Utilité instantanée cible
  comp_umixt, ///< Départ comportement UMIXT 
  comp_exo,   ///< Départ à un âge exogène
  comp_ageMax, ///< Départ à l'âge de la mise à la retraite d'office
  comp_uinst_old ///< Obsolète
};

//TODO: A améliorer : voir OutilsComp, prendre en compte double liquidation
//Utilisé ? Documentation à partir de type_liq_labels
///Type de liquidation (obsolète ?)
enum type_liq {
  liq_non = 0,      ///< Type de liquidation: modalité Non
  liq_tp = 1,       ///< Type de liquidation: modalité taux plein
  liq_invald = 2,   ///< Type de liquidation: modalité invalide
  liq_inapte = 3,   ///< Type de liquidation: modalité inapte
  liq_decote = 4,   ///< Type de liquidation: modalité decote
  liq_surcote = 5,  ///< Type de liquidation: modalité surcote
  liq_agemax = 6,   ///< Type de liquidation: modalité agemax
  liq_cible = 7,    ///< Type de liquidation: modalité cible umixt
  liq_tp_aod = 8,   ///< Type de liquidation: modalité AOD
  liq_tp_duree = 9, ///< Type de liquidation: modalité Durée
  liq_tp_aad = 10,  ///< Type de liquidation: modalité AAD
  liq_inact = 11    ///< Type de liquidation: modalité inactif
};

///étiquettes associées à type_liq
const vector<string> type_liq_labels = {"Non","taux plein","invalide","inapte","decote","surcote","agemax","cible umixt","AOD","Duree","AAD"};

/// Enumération des codes de régime de retraite
enum regime_indicateurs {
  REG_TOT,        ///< ensemble des régimes
  REG_RG,         ///< régime général
  REG_IN,         ///< régime des indépendants
  REG_FP,         ///< régime de la fonction publique
  REG_AR,         ///< régime Arrco
  REG_AG,         ///< régime Agirc
  REG_AGIRC_ARRCO,///< régime unifié Agirc-Arrco
  REG_FPE,        ///< régime de la fonction publique d'état
  REG_FPT,        ///< régime des fonctions publiques territoriale et hospitalière
  REG_AVPF        ///< affiliés au titre de l'AVPF
  };

/// Enumération des modalités de statut 
enum Statuts {
  S_NC                = 1   ,   ///< Non cadre du privé
  S_NONTIT            = 11  ,   ///< Contractuel
  S_SCOEMP            = 12  ,   ///< Scolaire en emploi
  S_SNAT              = 13  ,   ///< Service militaire
  S_CAD               = 2   ,   ///< Cadre du privé
  S_FPAE              = 311 ,   ///< Titulaire de la FP d'Etat catégorie active
  S_FPSE              = 321 ,   ///< Titulaire de la FP d'Etat emploi sédentaire 
  S_FPAAE             = 331 ,   ///< Régimes spéciaux : FP d'état
  S_FPATH             = 312 ,   ///< Titulaire de la FP hospitalière ou territoriale catégorie active
  S_FPSTH             = 322 ,   ///< Titulaire de la FP hospitalière ou territoriale emploi sédentaire
  S_FPAATH            = 332 ,   ///< Régimes spéciaux : FP territoriale
  S_IND               = 4   ,   ///< Indépendant
  S_CHO               = 5   ,   ///< Chômeur
  S_CHOBIT            = 51  ,   ///< Chômeur
  S_CHONONBIT         = 52  ,   ///< Dispensé de recherche d'emploi
  S_CHOEMPL           = 53  ,   ///< Obsolète : cumul emploi-chômage
  S_INA               = 6   ,   ///< Inactif pur
  S_INVAL             = 62 ,    ///< Invalide
  S_INVALRG           = 621 ,   ///< Invalide/ Malade du secteur privé
  S_INVALFP           = 623 ,   ///< Invalide/ Malade du secteur public
  S_INVALIND          = 624 ,   ///< Invalide/ Malade indépendant
  S_SCO               = 63  ,   ///< En scolarité
  S_PR                = 7   ,   ///< En préretraite
  S_RET               = 8   ,   ///< Retraité
  S_AVPF              = 9       ///< AVPF
};
/// Enumération des prestations familiales

enum code_prestafam{
  AB                  = 1 ,     ///< allocation de base pour enfant de moins de 3 ans avec condition de ressources
  CF3enfs             = 2 ,     ///< complément familial pour 3 enfants dont le plus jeune a plus de trois ans
  CLCA                = 3 ,     ///< prestation pour enfant de moins de trois ans sans condition ressource
  AB_CLCA             = 4       ///< cumul de l'AB et du CLCA
};



// Regroupements de modalité de statut

/** Statuts de la fonction publique, catégorie active */
const  vector<int> Statuts_FPA = { S_FPAE, S_FPAAE, S_FPATH, S_FPAATH};
/** Statuts de la fonction publique, catégorie sédentaire */
const  vector<int> Statuts_FPS= {S_FPSE, S_FPSTH};
/** Statuts de chômage */
const  vector<int> Statuts_cho= {S_CHOBIT, S_CHONONBIT};
/** Statuts d'emploi */
const  vector<int> Statuts_occ= {S_NC, S_CAD, S_NONTIT, S_SCOEMP, S_FPAE, 
                                  S_FPSE, S_FPAAE, S_FPATH, S_FPSTH, 
                                  S_FPAATH, S_IND, S_CHOEMPL};
/** Statuts d'activité */
const  vector<int> Statuts_act= {S_NC, S_CAD, S_NONTIT, S_SCOEMP, S_FPAE, 
                                  S_FPSE, S_FPAAE, S_FPATH, S_FPSTH, 
                                  S_FPAATH, S_IND, S_CHOEMPL,
                                  S_CHOBIT, S_CHONONBIT};
/** Statuts FP */
const  vector<int> Statuts_FP = {S_FPAE,S_FPSE,S_FPAAE,S_FPATH,S_FPSTH,S_FPAATH,S_INVALFP};
/** Statuts d'affiliation au régime général */
const  vector<int> Statuts_RG = {S_NC,S_NONTIT,S_SCOEMP,S_SNAT,S_CAD,S_CHO,S_CHOBIT,S_CHONONBIT,S_CHOEMPL,S_INVAL,S_INVALRG,S_PR,S_AVPF};
/** Statuts d'affiliation à un régime des indépendants */
const  vector<int> Statuts_IN= {S_IND,S_INVALIND};
/** Statuts d'emploi salarié au régime général*/
const  vector<int> Statuts_SAL= {S_NC, S_CAD, S_NONTIT};
/** Statuts fonctionnaire en catégorie active ou sédentaire */
const  vector<int> Statuts_FPAS= {S_FPAE, S_FPSE, S_FPATH, S_FPSTH};
/** Statuts d'emploi non fonctionnaire */
const  vector<int> Statuts_RGIN= {S_NC, S_CAD, S_NONTIT, S_IND};
/** Statuts de préretraité */
const  vector<int> Statuts_PRPRRG= {S_PR};
/** Statuts en emploi, fonction publique d'Etat */
const  vector<int> Statuts_FPE= {S_FPAE, S_FPSE, S_FPAAE};
/** Statuts en emploi, fonction publique territoriale ou hospitalière */
const  vector<int> Statuts_FPT= {S_FPATH, S_FPSTH, S_FPAATH};
/** Statuts privé */
const  vector<int> Statuts_Priv = {S_NC,S_NONTIT,S_SCOEMP,S_SNAT,S_CAD,S_IND,S_CHO,S_CHOBIT,S_CHONONBIT,S_CHOEMPL,S_INVAL,S_INVALRG,S_INVALIND,S_PR,S_AVPF};

/// Statuts des salariés du secteur privé
enum statutPrive {NC, ///< Non Cadre
                  CAD ///< Cadre
  };

/// Définition des classes relatives aux équations de salaire 
enum {FPE_F,FPE_H,FPHT_F,FPHT_H,IND_F,IND_H,PRI_F_deb,PRI_F_fin,PRI_H_deb,PRI_H_fin};
/** Définition des classes relatives aux équations de salaire */
const vector<string> STATUTS_EQS {"FPE_F","FPE_H","FPHT_F","FPHT_H","IND_F","IND_H","PRI_F","PRI_H"};

/// Types d'équation pour la simulation d'un état de santé
enum nom_eqsante {INCID_0_F,  ///< incidence chez les femmes sans incapacité
                  INCID_0_H,  ///< incidence chez les hommes sans incapacité 
                  INCID_1_F,  ///< incidence chez les femmes avec incapacité 
                  INCID_1_H,  ///< incidence chez les hommes avec incapacité 
                  MORT_F,     ///< mortalité chez les femmes, tenant compte de l'état de santé
                  MORT_H,     ///< mortalité chez les hommes, tenant compte de l'état de santé
                  PREVAL_F,   ///< prévalence chez les femmes 
                  PREVAL_H    ///< prévalence chez les hommes
};
/** Types d'équation pour la simulation d'un état de santé */
const vector<string> EQS_SANTE {"INCID_0_F","INCID_0_H","INCID_1_F","INCID_1_H","MORT_F","MORT_H","PREVAL_F","PREVAL_H"};

/// Définition des classes relatives à l'imputation d'un état de santé
enum classeagesante {C50_54=0,C55_59=1,C60_64=2,C65_69=3,C70_74=4,C75_79=5,C80_84=6,C85_89=7,C90=8};


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
