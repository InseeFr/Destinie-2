#pragma once
/**
 * \file Sante.h
 * \brief Imputation d'un état de sante pour les plus de 50 ans.
 * 
 * Ce module correspond à l'étude sur l'espérance de vie sans incapacité sévère réalisée par \cite evrsi .
 */
	

/** 
*  \fn evolsante(int t)
*  \brief Opère les transitions pour les personnes ayant déjà un état de santé.
*  \param t année
*/
void evolsante(int t);

 /** 
 *  \fn imput_sante(int t)
 *  \brief Impute l'état de sante des individus qui atteignent l'âge de 50 ans et fait évoluer la santé des seniors tous les deux ans.
 *  \param t année
 */


void imputsante(int t);

/** 
 *  \fn premiere_sante(int t, vector<int>& champ ,int sexe)
 *  \param t année
 *  \param champ vecteur des identifiants
 *  \param sexe précise le sexe des individus auxquels on attribue un état de santé.
 *  \brief Impute le premier état de santé pour le vecteur des identifiants passés dans champ.
 */
void premiere_sante(int t, vector<int>& champ ,int sexe);



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