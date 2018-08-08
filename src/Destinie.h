#pragma once
#define UNDEF 9999

/**
 * \file Destinie.h 
 * \brief contient les fonctions \ref calage_salaires, \ref destinieSim et \ref destinieSimIndiv pouvant être
 * appelées depuis R. 
 *
 * 
 * Ces fonctions prennent en paramètre un environnement de simulation.
 * Celui-ci doit contenir 3 tables de données individuelles (généralement, données
 * générées par le genebios) :
 * - `ech` : contient une ligne par individu
 * - `emp` : contient une ligne par individu * âge
 * - `fam` : contient une ligne par individu * année à partir de 2009
 * 
 * Il contient de plus une table `macro` contenant l'ensemble des séries macroéconomiques et des paramètres
 * législatifs.
 * 
 * Enfin il contient une liste de paramètres et d'options.
 */

/**
 * Fonction calage_salaires :
 * 
 * Cale les salaires sur le SMPT cible
 * 
 */
void calage_salaires(Environment envSim);

/**
  Fonction destinieSim : Lance une simulation en utilisant la fonction SimDir. La fonction boucle sur chaque année
  de simulation. Pour chaque année elle boucle sur chaque individu pour le calcul des droits directs, puis boucle
  à nouveau sur chaque individu pour le calcul des droits indirects et enfin boucle sur chaque individu pour le calcul
  du minimum vieillesse.
  
  L'avantage de la fonction destinieSim sur la fonction destinieSimIndiv est qu'elle calcule les droits indirects et le minimum vieillesse.
  
*/
void destinieSim(Environment envSim) ;

/**
  Fonction destinieSimIndiv : Lance une simulation en utilisant la fonction SimDir. Elle calcule les droits retraites de droit direct
 (pas le minimum vieillesse ou les réversions). Elle boucle sur chaque individu, puis sur chaque âge.
  
*/
void destinieSimIndiv(Environment envSim);



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
