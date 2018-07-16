#pragma once

/**
 * \file Migration.h
 * \brief Migrations détermine les individus qui émigrent et créent de nouveaux individus immigrants en respectant les cibles de 
 * solde migratoire par tranche d'âge.
 */

/** 
 *  \fn multinomiale(double alea,const vector<double>& vect)
 *  \brief Renvoie la réalisation d'une multinomiale définie par sa fonction de répartition.
 *  \param alea à choisir entre 0 et 1
 *  \param vect fonction de répartition de la loi multinomiale
 */
double multinomiale(double alea,const vector<double>& vect);
 
 
/** 
 *  \fn age_arrive(int sexe, bool estEnfant, int t)
 *  \brief Renvoie l'âge d'arrivée d'un migrant en fonction de son sexe et du fait qu'il soit enfant ou non à la date t donnée.
 *  
 *   L'âge d'arrivée des migrants est simulé suivant la distribution de l’âge d’arrivée des migrants. Cette distribution donne, pour les femmes
 *   (enfants ou adultes) et les hommes (enfants ou adultes), la répartition de l’âge par tranches quinquennales. L’âge est
 *   ensuite réparti uniformément au sein de chaque tranche.
 *   \param sexe du migrant
 *   \param estEnfant 'vrai' s'il s'agit d'un enfant
 *   \param t année de la simulation
 */
int age_arrive(int sexe, bool estEnfant, int t);




/** 
*  \fn findet_migrant (Indiv& X, int generation);
*  \brief Simule l'âge de fin d'études d'un migrant X suivant sa génération. 
*  
*   Pour les adultes l'âge de fin d'étude des migrants est l'âge moyen de la génération. <br>
*   Pour les enfants, si leur mère  a été identifiée, leur âge de fin d'étude est tiré dans une loi normale.
*   \param X un individu
*   \param generation année de naissance
*/
int findet_migrant (Indiv& X);

/** 
 *  \fn migrant (int t)
 *  \brief Simule les migrations (entrées et sorties) à la date t. 
 *  
 *   L'émigration concerne la tranche des 1-60 ans, et le nombre d'émigrants est calé
 *   par tranches quinquennales, soit 10 tranches entre 1 et 60 ans. Pour chaque sexe, des enfants et des adultes sont ajoutés
 *   à l’échantillon, conformément aux cibles. Une fois ajoutés, leur âge est simulé via un appel à la fonction \ref age_arrive.
 *   S'agissant des liens conjuguaux, les migrants sont tous célibataires : aucune vie de couple ne leur est imputée. Le seul 
 *   lien familial simulé concerne le lien mère/enfant. Une migrante a au plus trois enfants selon la procédure suivante. 
 *   Chaque enfant se voit attribuer comme mère une femme ayant terminé ses études au plus tard un an après sa naissance, et minimisant
 *   l’écart entre son année de naissance et l’année de fin d’études. Elle devient sa mère si elle a moins de 45 ans et a terminé
 *   ses études au plus tôt dix ans avant sa naissance. A l’issue de cette boucle, les migrantes ayant au moins un enfant sont
 *   ainsi sélectionnées. Ensuite, à chaque enfant n’ayant pas de mère est assigné la première migrante ayant eu un enfant à 
 *   l’étape précédente. Si cette femme a moins de 50 ans et que son premier enfant est plus jeune, elle devient la mère de
 *   l’enfant. De cette manière, les migrantes avec au moins deux enfants sont choisies. Enfin, pour chaque enfant n'ayant toujours
 *   pas de mère, on choisit une migrante avec deux enfants de l’étape précédente ayant le plus petit âge de fin d’études. Elle
 *   devient la mère si la différence d’âge est d’au plus 50 ans et si ses deux premiers enfants sont plus jeunes. Après la
 *   constitution des liens mère/enfant, l’âge de fin d’études de chaque enfant migrant est simulé via un appel à findet_migrant. 
 *   Si un enfant n’a pas encore atteint son âge de fin d’études en t, il est réputé être en scolarité, comme pour les non-migrants.
 *   Dans Destinie, l’émigration concerne deux catégories : les nouveaux-nés et les 18-26 ans. Parmi les nouveaux-nés, les
 *   sorties sont simulées dans la fonction \ref naissance. Pour les 18-26 ans, chaque individu se voit attribuer une probabilité
 *   d’émigrer, fonction de l’âge (entre 18 et 25 ans pour les femmes, 18 et 26 ans pour les hommes) et du sexe. Les émigrants 
 *   sont alors sélectionnés par sexe (et non par âge fin) suivant l’algorithme d’alignement par tri avec pour cible le nombre
 *   de jeunes émigrants fourni par les hypothèses démographiques.
 */
void migrant (int t);

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
