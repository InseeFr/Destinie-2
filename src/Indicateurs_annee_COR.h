/**
 * \file Indicateurs_annee_COR.h
 * \brief Ce fichier permet de calculer des agrégats annuels par régime et par sexe, ainsi que la chronique annuelle des cotisations et du salaire net par individus. 
 *  Les résultats, stockés dans les tables Indicateurs_an, cotisations et salairenet, sont exportés dans R.
 */

 #pragma once

#include "Simulation.h"
#include "DroitsRetr.h"
#include "Reversion.h"
#include "Statistiques.h"
#include "Retraite.h"
#include "Cotisations.h"

/**
 * \struct Indic_annee
 * \brief La structure \ref Indic_annee contient des agrégats sur un régime de retraite au cours d'une année, tels que les effectifs de
 *  cotisants, de retraités, de liquidants, les montants de cotisations, de dépenses, la pension moyenne, l'âge moyen de liquidation...
 */
struct Indic_annee {

  Somme
    Eff_Cotisants,        ///< Effectif total des cotisants (en emploi : ne compte pas les personnes au chômage ou à l'AVPF)
    Sal_Plaf,             ///< Montant total des salaires plafonnés (par le PSS)
    Sal_Tot,              ///< Montant total des salaires
    M_Cotisations,        ///< Montant total des cotisations
    Eff_DD,               ///< Effectit total des retraités de droits directs
    M_DD,                 ///< Montant total des droits directs
    Eff_Flux,             ///< Nombre de nouveaux retraités de droit direct
    Eff_Flux_Primo,       ///< Nombre de nouveaux primo-liquidants
    NB_vfu,               ///< (à vérifier) Nombre de bénéficiaires d'un versement forfaitaire unique
    M_vfu,                ///< (à vérifier) Masse des versements forfaitaires uniques versés
    
    Eff_Min_Flux,         ///< Nombre de personnes entrant dans le dispositif minimum vieillesse
    M_Min_Flux,           ///< Montant total du minimum vieillesse, en se restreignant au nouveaux bénéficiaires
    Eff_Min,              ///< Nombre de personnes bénéficiant du minimum vieillesse
    M_Min,                ///< Montant total du minimum vieillesse
    
    Eff_Derive,           ///< Effectif total des bénéficiaires de droits dérivés
    M_Derive,             ///< Montant total des droits dérivés
    Eff_Derive_seul,      ///< (à vérifier) Effectif total des bénéficiaires de droits dérivés, où c'est le seul droit
    M_Derive_seul,        ///< (à vérifier) Montant total des droits dérivés, où c'est le seul droit
    Eff_Flux_Derive,      ///< Nombre de nouveaux bénéficiaires de droits dérivés
    M_Majo_Derive,        ///< 
    
    Eff_DD_MV,            ///< (à vérifier) Effectif total de retraités de droit direct au minimum vieillesse
    M_DD_MV,              ///< (à préciser) Montant des pensions de droit direct placés au minimum vieillesse
    Eff_DRseul_MV,        ///< (à vérifier) Effectif total de retraités avec seulement du droit direct et au minimum vieillesse
    M_DRseul_MV,          ///< (à préciser) Montant des pensions avec seulement du droit direct et au minimum vieillesse
    
    M_Pensions,           ///< Montant total des pensions
    Eff_Cotisants_finAnnee,       ///<  (à vérifier) Effectif total des cotisants, au 31 décembre
    Eff_DD_finAnnee,      ///< (à vérifier) Effectit total des retraités de droits directs, au 31 décembre
    
    Eff_DD_MV_ma,         ///< (à vérifier) obsolète ?
    Eff_Flux_DD_MV,       ///< (à vérifier) obsolète ? la partie où il est défini semble être commentée ???
    Eff_Flux_DRseul_MV,   ///< 
    
  	Eff_DD_Flux_coeffSoli,        ///< 
  	M_DD_Flux_coeffSoli,  ///< 
  	M_Flux,               ///< Montant total des pensions des nouveaux retraités
  	
  	Eff_MV,               ///< 
  	Eff_Flux_MV,          ///< 
  	M_MV,                 ///< 
  	M_Flux_MV,            ///< 
  	
  	Eff_DD_Deces,         ///< 
  	Eff_Derive_Deces,     ///< 
  	M_Agff,               ///< 
  	M_DD_Flux,            ///< (à vérifier) Montant total des nouveaux droits dérivés
  	M_Flux_Derive,        ///< (à vérifier) différence avec M_DD_Flux ???
  	M_DD_Deces,           ///< 
  	M_Derive_Deces,       ///< 
  	
  	Eff_Cotisants_ma,     ///< 
  	Eff_DD_ma,            ///< 
  	
  	Eff_Min_ma,           ///< 
  	
  	Eff_DD_Flux_coeffSoli_ma,        ///< 
  	Eff_Min_Flux_ma,M_DD_Flux_ma,    ///< 
  	M_DD_ma,              ///< 
  	M_DD_Flux_coeffSoli_ma,         ///< 
  	M_Min_Flux_ma,        ///< 
  	M_Min_ma,             ///< 
  	Eff_Flux_ma,          ///< 
  	M_Pensions_ma,        ///< 
  	Eff_Retr_Derive,      ///< 
  	M_MICO_flux_ma,       ///< 
  	M_MICO_stock_ma,      ///< 
  	
  	Sal_Plaf_31dec,       ///< (à vérifier) Montant total des salaires plafonnés (par le PSS), au 31 décembre
  	Sal_Tot_31dec         ///< (à vérifier) Montant total des salaires, au 31 décembre
  ;

  Moyenne
    Age_Ret_Flux_Primo,       ///< Âge moyen de départ en retraite pour les nouveaux primo-liquidants
    Coeff_Prorat_Flux,        ///< Coefficient de proratisation moyen, pour les nouveaux liquidants
    Sal_fin_carr,             ///< (à préciser) Salaire moyen de fin de carrière
    AgeMoyRetr_finannee,      ///< Âge moyen du départ en retraire, au 31 décembre
    PMoy_Flux,                ///< Pension de droit direct moyenne parmi les nouveaux retraités de droit direct
    Age_Ret_Flux,             ///< Âge moyen de départ en retraite pour les nouveaux liquidants
    Dur_Ass_Flux,             ///< Durée moyenne d'assurances pour les nouveaux liquidants
    PMoy_Flux_Derive,         ///< Pension de droits dérivés moyenne parmi les nouveaux retraités de droit direct
    Sal_relatif,              ///< 
	  PMoy_DD                    ///< Pension de droit direct moyenne
    ;
    
  Ratio 
    TauxRemplace      ///< (à préciser) Taux de remplacement (moyen? du régime?)
    ;
};


/**
 * \fn cotisation_retraite()
 * \brief La fonction cotisation_retraite prend en argument un indiv X, un age a et un régime, et renvoie le montant des cotisations
 *  retraite (parts patronale et salariale) destinées au régime, prélevées sur les revenus d'activité perçus par X à l'âge a
 */
double cotisation_retraite(Indiv& X, int age, int regime);

/**
 * \fn regimes_cot()
 * \brief La fonction regimes_cot prend en argument un indiv X et un âge a, et renvoie, sous forme de vecteur d'entiers,
 * la liste des régimes d'affiliation de X à l'âge a
 */
deque<int> regimes_cot(Indiv& X, int age);

/**
 * \fn ecriture_indicateurs_an()
 * \brief La fonction ecriture_indicateurs_an prend en argument une date t et calcule les agrégats de \ref{Indic_annee} à la date t
 *  pour chaque régime. Les résultats sont exportés.
 */
void ecriture_indicateurs_an(int t);

/**
 * \fn ecriture_cotisations()
 * \brief La fonction ecriture_cotisations exporte les cotisations prélevées (part patronale et salariale) sur chaque individu à chaque age,
 * dans le public, le privé et les non-salariés.
 */
void ecriture_cotisations();

/**
 * \fn ecriture_salairenet()
 * \brief La fonction ecriture_salairenet exporte le salaire net perçu par chaque individu à chaque age
 */
void ecriture_salairenet();

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