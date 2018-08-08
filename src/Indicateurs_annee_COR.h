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
    Eff_Cotisants,        ///< Effectif d'affiliés au 31/12
    Sal_Plaf,             ///< Montant total des salaires plafonnés (au PSS) et proratisés par la durée en emploi
    Sal_Tot,              ///< Montant total des salaires proratisés par la durée en emploi
    M_Cotisations,        ///< Montant total des cotisations proratisées par la durée en emploi
    Eff_DD,               ///< Effectif de retraités de droits directs au 31/12
    M_DD,                 ///< Somme des pensions annualisées de droit direct
    Eff_Flux,             ///< Nombre de nouveaux retraités de droit direct
    Eff_Flux_Primo,       ///< Nombre de nouveaux primo-liquidants
    NB_vfu,               ///< Nombre de bénéficiaires d'un versement forfaitaire unique
    M_vfu,                ///< Masse des versements forfaitaires uniques versés
    
    Eff_Min_Flux,         ///< Flux d'individus bénéficiant d'un minimum de pension
    M_Min_Flux,           ///< Masse des pensions annualisées de droit direct des nouveaux bénéficiaires d'un minimum de pension
    Eff_Min,              ///< Nombre de bénéficiaires d'un minimum de pension
    M_Min,                ///< Masse des pensions annualisées de droit direct des bénéficiaires d'un minimum de pension
    
    Eff_Derive,           ///< Nombre de réversataires
    M_Derive,             ///< Montant total des droits dérivés
    Eff_Derive_seul,      ///< Nombre d'individus bénéficiant uniquement d'une pension de réversion
    M_Derive_seul,        ///< Masse des pensions de réversion des individus ne bénéficiant pas de droit direct
    Eff_Flux_Derive,      ///< Flux de réversataires
    
    M_Pensions,           ///< Montant total des pensions annualisées
    
  	Eff_DD_Flux_coeffSoli,        ///< Flux de liquidants soumis à un coefficient de solidarité (cf. accord Arrco-Agirc d'octobre 2015)
  	M_DD_Flux_coeffSoli,  ///< Masse des pensions soumises à un coefficient de solidarité (cf. accord Arrco-Agirc d'octobre 2015)
  	M_Flux,               ///< Montant total des pensions des nouveaux retraités
  	
  	Eff_MV,               ///< Nombre de bénéficiaires du minimum vieillesse
  	Eff_Flux_MV,          ///< Nombre de nouveaux bénéficiaires du minimum vieillesse
  	M_MV,                 ///< Masse des pensions annualisées des bénéficiaires du minimum vieillesse
  	M_Flux_MV,            ///< Masse des pensions annualisées des nouveaux bénéficiaires du minimum vieillesse
  	
  	Eff_DD_Deces,         ///< Nombre de décès parmi les retraités de droit direct
  	Eff_Derive_Deces,     ///< Nombre de décès parmi les réversataires
  	M_DD_Flux,            ///< Montant total des nouveaux droits directs
  	M_Flux_Derive,        ///< Montant des flux de droits dérivés
  	M_DD_Deces,           ///< Montant des droits directs (au 31/12) des décédés
  	M_Derive_Deces,       ///< Montant des droits dérivés des décédés
  	
  	Eff_Cotisants_ma,     ///< Effectif d'affiliés au prorata de la durée passée en emploi
  	Eff_DD_ma,            ///< Effectif de retraités de droits directs au prorata de la durée passée en retraite
  	
  	Eff_Min_ma,           ///< Nombre de bénéficiaires d'un minimum de pension, au prorata de la durée passée en retraite
  	
  	Eff_DD_Flux_coeffSoli_ma,        ///< Flux de liquidants soumis à un coefficient de solidarité (cf. accord Arrco-Agirc d'octobre 2015), au prorata de la durée passée en emploi
  	Eff_Min_Flux_ma,  ///< Flux d'individus bénéficiant d'un minimum de pension, au prorata de la durée passée en retraite
  	M_DD_Flux_ma,    ///< Flux des pensions proratisées de droit direct
  	M_DD_ma,              ///< Somme des pensions proratisées de droit direct
  	M_DD_Flux_coeffSoli_ma,         ///< Masse des pensions proratisées soumises à un coefficient de solidarité (cf. accord Arrco-Agirc d'octobre 2015)
  	M_Min_Flux_ma,        ///< Masse des pensions proratisées de droit direct des nouveaux bénéficiaires d'un minimum de pension
  	M_Min_ma,             ///< Masse des pensions proratisées de droit direct des bénéficiaires d'un minimum de pension
  	Eff_Flux_ma,          ///< Nombre de nouveaux retraités de droit direct, au prorata de la durée passée en retraite
  	M_Pensions_ma,        ///< Montant total des pensions proratisées
  	M_MICO_flux_ma,       ///< Montant des nouvelles dépenses, proratisées, effectuées au titre des minima de pensions
  	M_MICO_stock_ma,      ///< Montant des dépenses, proratisées, effectuées au titre des minima de pensions
  	
  	Sal_Plaf_31dec,       ///< Montant total des salaires plafonnés (au PSS), au 31 décembre
  	Sal_Tot_31dec         ///< Montant total des salaires, au 31 décembre
  ;

  Moyenne
    Age_Ret_Flux_Primo,       ///< Âge moyen de liquidation des primo-liquidants
    Coeff_Prorat_Flux,        ///< Coefficient de proratisation moyen des liquidants
    Sal_fin_carr,             ///< Salaire moyen de fin de carrière
    AgeMoyRetr_finannee,      ///< Âge moyen (au 31/12) des retraités de droit direct
    PMoy_Flux,                ///< Pension moyenne de droit direct des liquidants
    Age_Ret_Flux,             ///< Âge moyen de liquidation totale
    Dur_Ass_Flux,             ///< Durée moyenne d'assurance, y compris majorations, des liquidants
    PMoy_Flux_Derive,         ///< Montant moyen du droit de réversion des nouveaux réversataires
	  PMoy_DD                    ///< Pension moyenne de droit direct, en euros 2012
    ;
    
  Ratio 
    TauxRemplace      ///< Taux de remplacement de la somme des salaires du régime par la somme des pensions tous régimes
    ;
};


/**
 * \fn cotisation_retraite()
 * \brief La fonction cotisation_retraite prend en argument un indiv X, un âge a et un régime, et renvoie le montant des cotisations
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
 * \brief La fonction ecriture_cotisations exporte les cotisations prélevées (part patronale et salariale) sur chaque individu à chaque âge,
 * dans le public, le privé et les non-salariés.
 */
void ecriture_cotisations();

/**
 * \fn ecriture_salairenet()
 * \brief La fonction ecriture_salairenet exporte le salaire net perçu par chaque individu à chaque âge
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