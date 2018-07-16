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
    Eff_Cotisants, Sal_Plaf, Sal_Tot, M_Cotisations, Eff_DD, M_DD, Eff_Flux, Eff_Flux_Primo, NB_vfu, M_vfu, 
    Eff_Min_Flux, M_Min_Flux, Eff_Min, M_Min, Eff_Derive, M_Derive, Eff_Derive_seul, 
    M_Derive_seul, Eff_Flux_Derive, M_Majo_Derive, Eff_DD_MV, M_DD_MV, Eff_DRseul_MV, 
    M_DRseul_MV, M_Pensions, Eff_Cotisants_finAnnee, Eff_DD_finAnnee, Eff_DD_MV_ma, Eff_Flux_DD_MV, Eff_Flux_DRseul_MV,
	Eff_DD_Flux_coeffSoli, M_DD_Flux_coeffSoli, M_Flux, Eff_MV, Eff_Flux_MV, M_MV, M_Flux_MV, Eff_DD_Deces, Eff_Derive_Deces, M_Agff, M_DD_Flux, 
	M_Flux_Derive, M_DD_Deces, M_Derive_Deces, Eff_Cotisants_ma, Eff_DD_ma, Eff_Min_ma, Eff_DD_Flux_coeffSoli_ma,Eff_Min_Flux_ma,M_DD_Flux_ma,
	M_DD_ma, M_DD_Flux_coeffSoli_ma,M_Min_Flux_ma, M_Min_ma,Eff_Flux_ma, M_Pensions_ma, Eff_Retr_Derive, M_MICO_flux_ma, M_MICO_stock_ma, 
	Sal_Plaf_31dec, Sal_Tot_31dec;

  Moyenne
    Age_Ret_Flux_Primo, Coeff_Prorat_Flux, Sal_fin_carr,
    AgeMoyRetr_finannee, PMoy_Flux, Age_Ret_Flux, Dur_Ass_Flux, PMoy_Flux_Derive, Sal_relatif,
	PMoy_DD;
    
  Ratio 
    TauxRemplace;
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