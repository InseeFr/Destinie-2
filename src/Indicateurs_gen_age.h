/**
 * \file Indicateurs_gen_age.h
 * \brief Contient la structure éponyme qui contient des indicateurs agrégés par génération et par âge.
 */
#pragma once
#include <Rcpp.h>
#include "Simulation.h"
#include "DroitsRetr.h"
#include "Reversion.h"
#include "Statistiques.h"

/**
 * \struct Indic_gen_age
 * \brief Cette structure contient les principaux indicateurs calculés par génération et par âge. 
 */
struct Indic_gen_age {
  Somme
    PopTOT,       ///< Nombre de personnes
    PopEmploi,    ///< Nombre de personnes en emploi
    PopActive,    ///< Nombre de personnes actives
    PopChomage,   ///< Nombre de personnes au chômage
    PopRetraitee, ///< Nombre de retraités
    SalTot,       ///< Montant total des salaires
    immigrTot     ///<
  ;

  Moyenne
    SMPTEch     ///< Salaire moyen par tête dans l'échantillon
    ;
    
  Taux 
    TxChomage,  ///< Taux de chômage
    TxActif,    ///< Taux d'activité
    TxCadre     ///< Pourcentage de cadres parmi les personnes en emploi
    ;
};

/**
 * \fn ecriture_indicateurs_gen_age(int t)
 * \brief Cette fonction calcule les indicateurs et exporte la table R Indicateurs_gen_age.
 * \param t année
 */
void ecriture_indicateurs_gen_age(int t) {
  auto indics = make_vector2(3,AGE_MAX,Indic_gen_age());
  
  for(Indiv& X: pop) {
    if(X.est_present(t)) {

      int age = X.age(t);

      double partavtliq = X.retr->partavtliq(t);     
      auto& indic = indics[X.sexe][age];
	  auto& indicEnsemble = indics[0][age];

      indic.PopTOT        .push(true, 1, M->poids/1000);
      indic.PopEmploi     .push(in(X.statuts[age], Statuts_occ), 1, M->poids/1000*partavtliq);
      indic.PopActive     .push(in(X.statuts[age], Statuts_act), 1, M->poids/1000*partavtliq);
      indic.PopChomage    .push(in(X.statuts[age], Statuts_cho), 1, M->poids/1000*partavtliq);
      indic.PopRetraitee  .push(1,1- partavtliq, M->poids/1000);
      indic.SalTot        .push(in(X.statuts[age], Statuts_occ), X.salaires[age], M->poids/1000*partavtliq);
      indic.SMPTEch       .push(in(X.statuts[age], Statuts_occ), X.salaires[age], M->poids/1000*partavtliq);
      indic.TxChomage     .push(in(X.statuts[age], Statuts_act), in(X.statuts[age], Statuts_occ)*partavtliq, M->poids/1000);
      indic.TxActif       .push(true, in(X.statuts[age], Statuts_act)*partavtliq, M->poids/1000);  
      indic.TxCadre       .push(X.statuts[age]==S_CAD, in(X.statuts[age], Statuts_occ)*partavtliq, M->poids/1000);
	  
	  indicEnsemble.PopTOT        .push(true, 1, M->poids/1000);
      indicEnsemble.PopEmploi     .push(in(X.statuts[age], Statuts_occ), 1, M->poids/1000*partavtliq);
      indicEnsemble.PopActive     .push(in(X.statuts[age], Statuts_act), 1, M->poids/1000*partavtliq);
      indicEnsemble.PopChomage    .push(in(X.statuts[age], Statuts_cho), 1, M->poids/1000*partavtliq);
      indicEnsemble.PopRetraitee  .push(1,1- partavtliq, M->poids/1000);
      indicEnsemble.SalTot        .push(in(X.statuts[age], Statuts_occ), X.salaires[age], M->poids/1000*partavtliq);
      indicEnsemble.SMPTEch       .push(in(X.statuts[age], Statuts_occ), X.salaires[age], M->poids/1000*partavtliq);
      indicEnsemble.TxChomage     .push(in(X.statuts[age], Statuts_act), in(X.statuts[age], Statuts_occ)*partavtliq, M->poids/1000);
      indicEnsemble.TxActif       .push(true, in(X.statuts[age], Statuts_act)*partavtliq, M->poids/1000);  
      indicEnsemble.TxCadre       .push(X.statuts[age]==S_CAD, in(X.statuts[age], Statuts_occ)*partavtliq, M->poids/1000);
      if(age>0)
        indic.immigrTot     .push(X.statuts[age-1] == 0, 1, M->poids/1000);
		indicEnsemble.immigrTot     .push(X.statuts[age-1] == 0, 1, M->poids/1000);
    }
  }


  static Rdout indicateurs_gen_age("Indicateurs_gen_age",
      {"annee", "gen", "age", "sexe",
      "PopTOT", "PopEmploi", "PopActive", "PopChomage", 
      "PopRetraitee","SalTot", "SMPTEch", 
      "TxChomage", "TxActif", "TxCadre",
      "immigrTot"});
  

  const vector<string> label_sexe = {"ens","hom","fem"};
  for(int s = 0; s < 3; s++)
  for(int a = 0; a < AGE_MAX; a++) {
      auto& i = indics[s][a];
      indicateurs_gen_age.push_line(
          t,t-a,a,s,
          i.PopTOT, i.PopEmploi, i.PopActive, i.PopChomage,
          i.PopRetraitee, i.SalTot, i.SMPTEch,
          i.TxChomage, i.TxActif, i.TxCadre,
          i.immigrTot
        );
  }
}


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