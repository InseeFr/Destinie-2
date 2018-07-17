#pragma once
/**
 * \file Separations.h
 * \brief Contient les fonctions simulant les mises en couple et les séparations.
 */


/** 
 *  \fn duree_etat(Indiv& X, int age)
 *  \brief  Renvoie la durée passée dans le statut matrimonial actuel. 
 *  \param X l'individu
 *  \param age son âge
 */
int duree_etat(Indiv& X, int age) ;

/** 
 *  \fn  NEnfunion()
 *  \brief La fonction NEnfunion dénombre le nombre d'enfants nés dans le couple actuel.
 *  \param X l'individu
 *  \param age
 */
int NEnfunion(Indiv& X, int age);
  

/** 
*  \fn mise_en_couple(int t)
*  \brief La fonction mise_en_couple choisit les personnes seules qui vont se mettrent en couple et les apparient.
*  
*  Il y a deux grandes parties dans la fonction mise_en_couple. D'abord le calcul de la probabilité 
*  de mettre en couple ou remettre en couple pour chaque hommes et chaque femme de l'échantillon. Puis
*  l'appariement entre femmes et hommes. Pour réaliser l'appariement, on calcule un score basé sur la différence d'âge et la différence
*  d'âge de fin d'étude pour respecter l'homogamie des couples. Puis successivement à chaque femme ayant été choisie comme se mettant 
*  en couple, on attribue le partenaire le plus proche au sens du score précédemment calculé. Une fois le couple
*  formé, les caractaristiques des deux membres du couple sont mises à jour. <br>
*  Les estimations des paramètres de cette fonction ont été faites par \cite duee2005modelisation
*  
*  \param t année
*/ 
void mise_en_couple (int t);

/** 
 *  \fn  separation(int t)
 *  \brief La fonction separation sert à déterminer les couples qui se séparent.
 *  
 *   On calcule la probabilité de se séparer pour chaque femme en fonction de la durée du couple et du nombre d'enfants.
 *   En suite, on réalise un tirage simple des femmes qui vont se séparer. Puis on met à jour ses informations et celles de 
 *   son ex-conjoint. <br>
 *   
 *   Les estimations des paramètres de cette fonction ont été faites par \cite duee2005modelisation
 *  
 *  \param t année
 */   
void separation (int t);


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