#pragma once
/**
 * \file Naissance.h
 * \brief Naissances contient la fonction qui simule les naissances.
 */


/** 
 *  \fn naissance(int t, bool option_mort_tirageCale)
 *  \brief La fonction naissance comprend deux grandes parties : le choix des mères et l'initialisation des enfants.
 *  
 *  La première partie sert à calculer les probabilités des femmes en couples d'avoir un enfant l'année t considérée selon :
 *  <ul>
 *    <li> le nombre d’enfants qu'ont déjà chacun des parents
 *    <li> la durée des études de la mère (soit courte soit longue)
 *    <li> que l'enfant précédent de la mère a ou non le même père.
 *  </ul>  
 * Les probabilités ont été estimées  par \cite duee2005modelisation . <br>
 * Dans la deuxième partie, on sélectionne les mères potentielles avec la fonction tirage soit calé soit non-calé. Et puis, on impute
 * les âges de fin d'étude des nouveaux bébés et complète toutes les informations nécessaires pour ajouter ces nouveaux individus dans la liste
 * de la population. 
 * Finalement sont simulés les décès des enfants avant l'âge d'un an. Ceci n'est pas fait dans la fonction \ref mortalite puisque cette 
 * fonction est appelée avant la fonction naissance dans la boucle temporelle. De même, le solde migratoire des enfants de moins d'un an étant 
 * négatif, on simule l'émigration des enfants de moins d'un an.
 * 
 *  \param t année
 *  \param option_mort_tirageCale choix de caler ou non le nombre de décès d'enfants de moins d'un an
 */
void naissance (int t, bool option_mort_tirageCale);


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