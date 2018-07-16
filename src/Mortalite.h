/**
 * \file Mortalite.h
 * \brief Ce fichier contient la fonction \fn mortalite qui impute les décès en projection.
*/

#pragma once

/** 
 *  \fn mortalite(int t, bool option_mort_diff, bool option_mort_tirageCale, bool option_sante, bool option_mort_diff_dip)
 *  \brief Simule la mortalité pour l'année t en fonction des options choisies. 
 *  
 *  La simulation des décès consiste, pour chaque âge et chaque sexe, à attribuer
 *   une probabilité de décès aux individus présents en t \f$-\f$ 1 et à effectuer, lorsque option_mort_tirageCale==TRUE, un tirage
 *   d’alignement par tri en prenant pour cible K = (probabilité de décès) × (nombre de présents en t \f$-\f$ 1). Une fois la liste
 *   des morts constituée, leurs éventuels conjoints deviennent veufs et pourront se remettre en couple en t. Quant aux
 *   survivants, leurs statuts professionnels et matrimoniaux en t \f$-\f$ 1 sont prolongés - ils seront éventuellement mis
 *   à jour ultérieurement via les mises en couples, séparations et transitions sur le marché du travail.
 *   Lorsque option_mort_diff=FALSE, option_mort_diff_dip=FALSE et option_mort_tirageCale=TRUE, les probabilités de décès
 *   correspondent aux quotients de mortalité des projections démographiques, déclinés par âge, par année et par sexe.
 *   Lorsque option_mort_diff_dip==TRUE, les quotients de mortalité dépendent, à partir de 31 ans, du sexe et du niveau de diplôme
 *   (sans diplôme, brevet, CAP-BEP, baccalauréat, diplôme du supérieur). Le niveau de diplôme dépend de la variable findet.
 *   En 2010, les quotients de mortalité par diplôme correspondent à ceux observés sur la
 *   période 2009-2013. En projection, ils évoluent comme les quotients de mortalité de la population générale.
 *   Lorsque option_mort_diff==TRUE, les quotients de mortalité dépendent, à partir de 31 ans, du sexe et de la tranche de findet 
 *   (cette dernière dépendant de la génération). Cette option reproduit la mortalité différentielle par CS via une table de
 *   passage entre CS et findet, obtenue à partir de l'enquête Patrimoine.
 *   \param t année
 *   \param option_mort_diff
 *   \param option_mort_tirageCale
 *   \param option_sante
 *   \param option_mort_diff_dip
 */
void mortalite(int t, bool option_mort_diff, bool option_mort_tirageCale, bool option_sante, bool option_mort_diff_dip);


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