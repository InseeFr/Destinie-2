/**
 * \file CotisationsTRI.h
 * \brief Code pour le calcul du taux de rendement interne 
 * 
 * Ce module sur le taux de rendement interne a été développé à l'occasion du document de travail \cite dubois2015indicateurs .
 */

#pragma once
#include "Simulation.h"
#include "DroitsRetr.h"
#include "OutilsComp.h"
#include "Reversion.h"
#include "Retraite.h"
#include "OutilsRcpp.h"
#include "range.h"
using namespace util::lang;

/**
 * \struct Cotisations
 * \brief La structure Cotisations contient les différents taux de cotisations (maladie et retraite) par année.
 * 
 */
struct Cotisations {  
  NumericVector
  _(Date),
  _(CotEmp_ss_plaf),
  _(CotEmpMaladie_ss_plaf),
  _(CotEmpMaladie_sal_tot),
  _(CotEmpMaladie_alleg_txmin),
  _(CotEmpMaladie_alleg_txmax),
  _(CotEmpMaladie_alleg_salmin),
  _(CotEmpMaladie_alleg_salmax),
  _(CotEmpRetr_ss_plaf),
  _(CotEmpRetr_sal_tot),
  _(CotEmpRetr_alleg_txmin),
  _(CotEmpRetr_alleg_txmax),
  _(CotEmpRetr_alleg_sal_min),
  _(CotEmpRetr_alleg_salmax),
  _(CotEmpAutonomie),
  _(CotEmpFamille_ss_plaf),
  _(CotEmpFamille_sal_tot),
  _(CotEmpFamille_taux1),
  _(CotEmpFamille_taux2),
  _(CotEmpFamille_taux3),
  _(CotEmpFamille_seuil1),
  _(CotEmpFamille_seuil2),
  _(CotEmpFamille_txmin),
  _(CotEmpFamille_txmax),
  _(CotEmpFamille_alleg_sal_min),
  _(CotEmpFamille_alleg_salmax),
  _(CotEmpATMP_sal_tot),
  _(CotEmpAssCho_taux_trA),
  _(CotEmpAssCho_taux_trB),
  _(taux_part_du_revenu),     
  _(taux_part_de_conso),   
  _(csg_FSV_part_minvieil),
  _(taux_csg_sal_FSV),  
  _(taux_csg_retr_FSV),
  _(Transferts_Unedic_Agirc_Arrco),
  _(taux_chomage_A),
  _(taux_chomage_C),
  _(CotPatASF_taux_trA), 
  _(CotSalASF_taux_trA), 
  _(CotPatASF_taux_trB), 
  _(CotSalASF_taux_trB), 
  _(CotPatAGFF_taux_tr1), 
  _(CotSalAGFF_taux_tr1), 
  _(CotPatAGFF_taux_tr2), 
  _(CotSalAGFF_taux_tr2), 
  _(CotPatAGFF_taux_trB), 
  _(CotSalAGFF_taux_trB);
};

/**
 * \struct OptionsTRI
 * \brief La structure OptionsTRI contient les 3 options pour le calcul du taux de rendement interne
 */
struct OptionsTRI {
  bool _(NoCotisationsIndirectes), _(NoAllegementsCot);
  NumericVector _(Calage_cotisationDirectes,0);
};

/**
 * \struct CotisationsTRI
 * \brief La structure CotisationsTRI contient la fonction CotisationsTRI qui calcule l'intégralité
 *  des cotisations d'un individu.
 */
struct CotisationsTRI {
    Indiv& X;
    Macro& s;
    Cotisations& c;
    OptionsTRI& options_tri;
    double cot_patr_Retr_rg     = 0;
    double cot_patr_Retr_fpe    = 0;
    double cot_patr_Retr_fpt    = 0;
    double cot_patr_Retr_ind    = 0;
    double cot_patr_Retr_alleg  = 0;
    double cot_patr_arrco       = 0;
    double cot_patr_agirc       = 0;
    double cot_patr_ASF         = 0;
    double cot_patr_AGFF        = 0;
    double cot_sal_Retr_arrco   = 0;
    double cot_sal_Retr_agirc   = 0;
    double cot_sal_Retr_rg      = 0;
    double cot_sal_Retr_AGFF    = 0;
    double cot_sal_Retr_ASF     = 0;
    double cot_sal_Retr_fp      = 0;
    double cot_sal_Retr_ind     = 0;
    double csg_sal_fsv = 0;
    double contrib_retr_actif   = 0;
    double transferts_Unedic_Agirc_Arrco = 0;
    double cot_rg = 0;
    double cot_tot = 0;
    
    CotisationsTRI(Indiv& X, int age, Macro& s, Cotisations& c, OptionsTRI& opt) :
        X(X), s(s), c(c), options_tri(opt)
    {
      int t = X.date(age);
      vector<double> alleg_pat_Ret = {
                                        c.CotEmpRetr_alleg_sal_min[t], c.CotEmpRetr_alleg_txmin[t], 
                                        c.CotEmpRetr_alleg_salmax[t],  c.CotEmpRetr_alleg_txmax[t]
                                        };
      
      int statut = X.statuts[age];
      double salaire = X.salaires[age];
      bool   ind_fp 	  = in(statut, Statuts_FP);
      bool   ind_fpe 	  = in(statut, Statuts_FPE);
      bool   ind_fpt 	  = in(statut, Statuts_FPT);
      bool   ind_agirc 	= statut == S_CAD;
      bool   ind_ind   	= statut == S_IND;
      bool   ind_arrco 	= (statut == S_NC || statut == S_NONTIT);
      bool   ind_rg 	  = (statut  == S_NC || statut  == S_NONTIT || statut == S_CAD);
      bool   ind_nc     = (statut == S_NC);
      double sal  = salaire  / (ind_fp ? (1 - X.taux_prim) : 1);
      
      
      // Cotisations patronales 
      cot_patr_Retr_rg      = !ind_rg  ? 0 : 
                                      (part(sal,0,s.PlafondSS[t]) * c.CotEmpRetr_ss_plaf[t] + sal * c.CotEmpRetr_sal_tot[t]);
    
      cot_patr_Retr_fpe     = !ind_fpe ? 0 : 
                                      s.TauxEmplFPE[t] * sal;
      
      cot_patr_Retr_fpt     = !ind_fpt ? 0 : 
                                      s.TauxEmplFPTH[t] * sal;
      
      cot_patr_Retr_ind     = !ind_ind ? 0 : 
                                      s.TauxEmpRGSalTot[t]* sal + s.TauxEmpRGSP[t]*part(sal,0,s.PlafondSS[t]); 
      
      cot_patr_Retr_alleg   = !ind_nc  ? 0.0 : 
                                      (c.CotEmpRetr_alleg_salmax[t] == 0 ? 0.0 : sal * affn(sal/s.SMIC[t],alleg_pat_Ret));
      
      cot_patr_arrco        = !ind_arrco?0 : 
                                      (s.TauxARRCO_1[t] - s.TauxARRCO_S1[t]) * s.TauxAppARRCO[t] * part(sal,0,s.PlafondSS[t]);
      
      cot_patr_agirc        = !ind_agirc?0 : (
                                      (s.TauxARRCO_1[t]-s.TauxARRCO_S1[t])*s.TauxAppARRCO[t]*part(sal,               0,  s.PlafondSS[t])
                                    + (s.TauxAGIRC_B[t]-s.TauxAGIRC_SB[t])*s.TauxAppAGIRC[t]*part(sal,  s.PlafondSS[t],4*s.PlafondSS[t])
                                    + (s.TauxAGIRC_C[t]-s.TauxAGIRC_SC[t])*s.TauxAppAGIRC[t]*part(sal ,4*s.PlafondSS[t],8*s.PlafondSS[t])
      );
      cot_patr_ASF          = !(ind_arrco || ind_agirc)?0 : (
                                        c.CotPatASF_taux_trA[t]		*   part(sal,               0,   s.PlafondSS[t])
                                      + c.CotPatASF_taux_trB[t]		*  	part(sal,  s.PlafondSS[t], 4*s.PlafondSS[t])
      );
       
      cot_patr_AGFF         =  
        (ind_nc) ? (
            +   c.CotPatAGFF_taux_tr1[t]    *   part(sal,               0,   s.PlafondSS[t])
            +   c.CotPatAGFF_taux_tr2[t]    *   part(sal,  s.PlafondSS[t], 3*s.PlafondSS[t])
        ) :
        (ind_agirc)?  (
            +   c.CotPatAGFF_taux_tr1[t]    *   part(sal,               0,   s.PlafondSS[t])
            +   c.CotPatAGFF_taux_trB[t]    *   part(sal,  s.PlafondSS[t], 4*s.PlafondSS[t])
        ) : 0;
      
      
      // Cotisations salariales
      cot_sal_Retr_arrco    = !(ind_arrco || ind_agirc)?0 : (
               (s.TauxARRCO_S1[t]		*	s.TauxAppARRCO[t]	*	part(sal,                0,   s.PlafondSS[t])
              + s.TauxARRCO_S2[t]		*	s.TauxAppARRCO[t]	*	part(sal,   s.PlafondSS[t], 3*s.PlafondSS[t])
               )  /** (1+c.Bouclage_macro_cotsal[t])*/
      );
      
      cot_sal_Retr_agirc    = !(ind_agirc)?0 : (
            (s.TauxAGIRC_SB[t]		*	s.TauxAppAGIRC[t]	*	part(sal,   s.PlafondSS[t], 4*s.PlafondSS[t])
           + s.TauxAGIRC_SC[t]		*	s.TauxAppAGIRC[t]	*	part(sal, 4*s.PlafondSS[t], 8*s.PlafondSS[t])
            ) /* * (1+Bouclage_macro_cotsal[t])   */
      );
        
      cot_sal_Retr_rg = !(ind_rg)?0 : (
            (s.TauxSalRGSalTot[t]		*	sal
           + s.TauxSalRGSP[t] 		*  	part(sal,  0, s.PlafondSS[t])
            ) /* * (1+Bouclage_macro_cotsal[t]) */
      );
      
      cot_sal_Retr_AGFF = !(ind_arrco)?0 : (
            +   c.CotSalAGFF_taux_tr1[t]    *   part(sal,  0, s.PlafondSS[t])
            +   c.CotSalAGFF_taux_tr2[t]    *   part(sal,  s.PlafondSS[t], 3*s.PlafondSS[t])
      );
      
      cot_sal_Retr_ASF = !(ind_arrco)?0 : (
            +	c.CotSalASF_taux_trA[t]   *   part(sal,  0, s.PlafondSS[t])
            + c.CotSalASF_taux_trB[t]		*  	part(sal,  s.PlafondSS[t], 4*s.PlafondSS[t])
      );
      
      cot_sal_Retr_AGFF = !(ind_agirc)?0 : (
            +   c.CotSalAGFF_taux_tr1[t]    *   part(sal,  0, s.PlafondSS[t])
            +   c.CotSalAGFF_taux_trB[t]    *   part(sal,  s.PlafondSS[t], 4*s.PlafondSS[t])
      );
      
      cot_sal_Retr_ASF = !(ind_agirc)?0 : (
            +	c.CotSalASF_taux_trA[t]   *   part(sal,  0, s.PlafondSS[t])
            + c.CotSalASF_taux_trB[t]		*  	part(sal,  s.PlafondSS[t], 4*s.PlafondSS[t])
      );
      
      cot_sal_Retr_fp = !(ind_fp)?0: (
            s.TauxFP[t]				*	sal
      );
      
      cot_sal_Retr_ind  = !(ind_ind)?0: (
              s.TauxRSIsurP[t]		*	(sal-part(sal, 0, s.PlafondSS[t]))
            + s.TauxRSIssP[t]			*	part(sal, 0, s.PlafondSS[t])
      );
      
      csg_sal_fsv += c.taux_csg_sal_FSV[t] * X.salaires[age] ;
      
      double cot_patr_AssCho = !(ind_rg)? 0 : c.CotEmpAssCho_taux_trA[t] * part(sal,  0, s.PlafondSS[t]) + c.CotEmpAssCho_taux_trB[t] * part(sal, s.PlafondSS[t], 4*s.PlafondSS[t]);
      double cot_sal_Assedic = !(ind_rg)? 0 : s.TauxAssedic[t] * part(sal, 0, 4*s.PlafondSS[t]);
      int t_min = min(109,t);
      transferts_Unedic_Agirc_Arrco += c.Transferts_Unedic_Agirc_Arrco[t_min] * (cot_sal_Assedic + cot_patr_AssCho) * 
          ((t > 109) ? (c.taux_chomage_C[t] / c.taux_chomage_C[109]) : 1.0);
      
      static int k = 0;
      if(t==110 && k < 10) {
        Rcout << c.taux_chomage_C[t] / c.taux_chomage_C[109] << endl;
        k++;
      }
      
      contrib_retr_actif += X.salaires[age] * c.taux_part_du_revenu[t] + csg_sal_fsv + cot_sal_Retr_AGFF + cot_patr_ASF + cot_patr_ASF + cot_patr_AGFF;
    
      /*static Rdout dfOut("cotis",{
        "salaire", "Id", "age", "statut", "annee",
        "cot_patr_Retr_rg", "cot_patr_Retr_fpe", "cot_patr_Retr_fpt", "cot_patr_Retr_ind", 
        "cot_patr_Retr_alleg", "cot_patr_arrco", "cot_patr_agirc", "cot_patr_ASF", 
        "cot_patr_AGFF", "cot_sal_Retr_arrco", "cot_sal_Retr_agirc", "cot_sal_Retr_rg", 
        "cot_sal_Retr_AGFF", "cot_sal_Retr_ASF", "cot_sal_Retr_fp", "cot_sal_Retr_ind"});
      
      dfOut.push_line(
         X.salaires[age], X.Id, age, X.statuts[age], t+1900,
         cot_patr_Retr_rg ,  cot_patr_Retr_fpe ,  cot_patr_Retr_fpt ,  cot_patr_Retr_ind , 
         cot_patr_Retr_alleg ,  cot_patr_arrco ,  cot_patr_agirc ,  cot_patr_ASF , 
         cot_patr_AGFF ,  cot_sal_Retr_arrco ,  cot_sal_Retr_agirc ,  cot_sal_Retr_rg , 
         cot_sal_Retr_AGFF ,  cot_sal_Retr_ASF ,  cot_sal_Retr_fp ,  cot_sal_Retr_ind     
      );*/
      
      cot_rg = cot_patr_Retr_rg + cot_sal_Retr_rg  + ((options_tri.NoAllegementsCot) ? 0 : cot_patr_Retr_alleg);
      
      cot_tot = + cot_patr_Retr_rg
        //  + cot_patr_Retr_fpe
        //  + cot_patr_Retr_fpt
        //  + cot_patr_Retr_ind
        + ((options_tri.NoAllegementsCot) ? 0 : cot_patr_Retr_alleg)
        + cot_patr_arrco
        + cot_patr_agirc
        //+ cot_patr_ASF
        //+ cot_patr_AGFF
        + cot_sal_Retr_arrco
        + cot_sal_Retr_agirc
        + cot_sal_Retr_rg
        //+ cot_sal_Retr_AGFF
        //+ cot_sal_Retr_ASF
        //  + cot_sal_Retr_fp
        //  + cot_sal_Retr_ind
        // + csg_sal_fsv;
        //+ transf_Unedic_retrcomp;    
        ;
      
    }
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