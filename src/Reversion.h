/**
 * \file Reversion.h
 * 
 * Calcul de la liquidation d'une reversion 
 */



#pragma once


#include  "Simulation.h"
#include  "DroitsRetr.h"




  /**
   * \class Reversion
   * Calcul de la liquidation d'une reversion 
   */
class Reversion
{
public:
  double rev;       ///< Montant totale de la reversion à la liquidation
  double rev_rg;    ///< Montant de la pension de reversion au RG à la liquidation
  double rev_fp;    ///< Montant de la pension de reversion au FP à la liquidation
  double rev_in;    ///< Montant de la pension de reversion au RSI à la liquidation
  double rev_ag;    ///< Montant de la pension de reversion à l'AGIRC à la liquidation
  double rev_ar;    ///< Montant de la pension de reversion à l'ARRCO à la liquidation
	double rev_ag_ar; ///< Montant de la pension de reversion dans les complémentaires unifiées à la liquidation
  int ageliq_rev;   ///< Âge à la liquidation
	int nbEnfCharge;  ///< Nombre d'enfants à charge lors du décès du conjoint
	int idConj;       ///< Identifiant du défunt
    
  /**
  * \fn Reversion(Indiv & X, Indiv & Y, int t, int legRetroMax)
  * Calcul de la liquidation d'une reversion pour le reversataire X,
  * au moment du décès de son conjoint Y à la date t.
  */
  Reversion(Indiv & X, Indiv & Y, int t, int legRetroMax);
};

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
