/**
 * \file Indicateurs_gen.h 
 * 
 * \brief Permet d'exporter la structure éponyme avec notamment les motifs de liquidation de la pension.
 */
#pragma once

#include "Simulation.h"
#include "Retraite.h"
#include "DroitsRetr.h"
#include "Reversion.h"
#include "Statistiques.h"


/**
 * \struct Indic_gen 
 * \brief contient des indicateurs par génération : les différents motifs de liquidation,
 * les moyennes d'âge à la liquidation, de durée validée, taux de remplacement...
 */
struct Indic_gen {
  Moyenne AGEPRIMOLIQ;
  Moyenne AGETOTLIQ;
  Moyenne DUREEVAL;
  Taux    DEPARTAVANT;
  Taux    DEPARTNORM;
  Taux    DEPARTENTRE;
  Taux    DEPARTANNDEC;
  Taux    DEPARTAPRES;
  Taux    DEPARTNODECFP;
  Taux    DEPARTTPDUR;
  Taux    DEPARTTPAGE;
  Taux    DEPARTTPINAPT;
  Taux    DEPARTDECOTE;
  Taux    DEPARTSURCOTE;
  Moyenne PMOYLIQ;
  Moyenne COEFFPRORAT;
  Somme   NBAFF;
  Somme   NBRETTOT;
  Somme   NBRETPRIMO;
  Somme   NBVFU;
  Taux    PARTAFF;
  Taux    PARTRETTOT;
  Taux    PARTRETPRIMO;
  Taux    PARTVFU;
  Moyenne TAUXREMPLBRUT;
  Moyenne SALFINCARR;
  Moyenne AGEFINEMPL;
  Moyenne PENSION68;
};

/**
 * \fn ecriture_indicateurs_gen
 * \brief Calcule et exporte dans R les éléments de \ref Indic_gen
 * 
 */
void ecriture_indicateurs_gen() {
  static const vector<int> Regimes = {REG_TOT, REG_RG, REG_IN, REG_FP, REG_AR, REG_AG, REG_FPE, REG_FPT};
  auto indicateurs = make_vector3(3,AN_NB,Regimes.size(),Indic_gen()); 

  for(auto& X: pop){
    if(X.retr->liq) {
      DroitsRetr& dr = *(X.retr->liq);
      for(int reg : Regimes) {
        if(dr.pensionliq(reg) > 0)
        for(int sexe : {0,X.sexe}) {
            auto& indic = indicateurs[sexe][X.anaiss%1900][reg];
            indic.AGEPRIMOLIQ.push(true, dr.agefin_primoliq, M->poids);
            indic.AGETOTLIQ.push(dr.agefin_totliq > 0, dr.agefin_totliq, M->poids);
            indic.DUREEVAL.push(true, dr.duree_maj(reg), M->poids);
            indic.DUREEVAL.push(true, dr.duree_maj(reg), M->poids);
            indic.PMOYLIQ.push(true, dr.pensionliq(reg), M->poids);
            indic.COEFFPRORAT.push(true, dr.taux_prorat(reg), M->poids);
            //indic.NBAFF.push(true, dr.taux_prorat(reg), M->poids);
            indic.NBRETTOT.push(true, 1, M->poids);
            indic.NBRETPRIMO.push(true, dr.primoliq && !dr.liq, M->poids);
            indic.SALFINCARR.push(true, dr.derSalNet(), M->poids);
            indic.AGEFINEMPL.push(true, dr.ageFinEmp(), M->poids);
            indic.TAUXREMPLBRUT.push(true, dr.pensionliq(reg) / dr.derSalNet(), M->poids);
            if(X.est_present(X.date(68))) {
              Retraite& retr = *(X.retr);
              retr.revalo(X.date(68));
              indic.PENSION68.push(true, retr.pension(reg), 0);
            }
        }
        if(dr.VFU(reg) > 0) {
          for(int sexe : {0,X.sexe}) {
              auto& indic = indicateurs[sexe][X.anaiss%1900][reg];
              indic.NBVFU.push(true, 1, M->poids);
          }
        }
      }
    }
  }
  
  static auto df = Rdout("Indicateurs_gen",{
      "sexe","gen","regime",
      "AGEPRIMOLIQ", "AGETOTLIQ", "DUREEVAL", "DEPARTAVANT", "DEPARTNORM", "DEPARTENTRE", "DEPARTANNDEC", "DEPARTAPRES", 
      "DEPARTNODECFP", "DEPARTTPDUR", "DEPARTTPAGE", "DEPARTTPINAPT", "DEPARTDECOTE", "DEPARTSURCOTE", "PMOYLIQ", 
      "COEFFPRORAT", "NBAFF", "NBRETTOT", "NBRETPRIMO", "NBVFU", "PARTAFF", "PARTRETTOT", "PARTRETPRIMO", "PARTVFU", 
      "TAUXREMPLBRUT", "SALFINCARR", "AGEFINEMPL", "PENSION68"
    });
  
  for(int sexe : range(3))
    for(int gen : range(AN_NB))
      for(int reg : range(Regimes.size())) {
        auto& i = indicateurs[sexe][gen][reg];
        df.push_line(
          i.AGEPRIMOLIQ, i.AGETOTLIQ, i.DUREEVAL, i.DEPARTAVANT, i.DEPARTNORM, i.DEPARTENTRE, i.DEPARTANNDEC, i.DEPARTAPRES, 
          i.DEPARTNODECFP, i.DEPARTTPDUR, i.DEPARTTPAGE, i.DEPARTTPINAPT, i.DEPARTDECOTE, i.DEPARTSURCOTE, i.PMOYLIQ, 
          i.COEFFPRORAT, i.NBAFF, i.NBRETTOT, i.NBRETPRIMO, i.NBVFU, i.PARTAFF, i.PARTRETTOT, i.PARTRETPRIMO, i.PARTVFU, 
          i.TAUXREMPLBRUT, i.SALFINCARR, i.AGEFINEMPL, i.PENSION68
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