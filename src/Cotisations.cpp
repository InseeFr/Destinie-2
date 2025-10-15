#include "Cotisations.h"
#include "Simulation.h"
#include "OutilsBase.h"
#include "DroitsRetr.h"
#include "Retraite.h"

const vector<int> NC_NonTit = {S_NC, S_NONTIT};

Cotisations::Cotisations(Indiv& X, int t) 
{
  Id = X.Id;
  annee = t;
  age = X.age(t);
  partavtprimo = X.retr->primoliq ? min_max(arr_mois(X.retr->primoliq->agefin_primoliq - age, X.moisnaiss+1),  0, 1) : 1;
  statut = (X.retr->primoliq && partavtprimo < 1) ? 8 : X.statuts[age];
  salaire         = X.salaires[age] * partavtprimo;
  salaire_net     = X.salaires[age] - CotRet(X,age) - CotAut(X,age) - CSGSal(X,age) * partavtprimo;
  cotis_retr      = CotRet(X,age) * partavtprimo;
  cotis_retr_patr = CotRetrPatr(X,age) * partavtprimo;
}


double CotMalRetrComp(Indiv& X, int age) {
    int t = X.anaiss%1900 + age;  
    if(X.retr->pension_ar>0 || X.retr->pension_ag>0||X.retr->pension_ag_ar>0) {
        return M->TauxMalComp[t] * (X.retr->pension_ag + X.retr->pension_ar+X.retr->pension_ag_ar);        
    }
    return 0.0;
}

double CSGRet(Indiv& X, int age)
{
    int t = X.anaiss%1900 + age;
    if (X.retr->pension_tot > M->SeuilExoCSG[t])
        return M->TauxCSGRetFort[t] * X.retr->pension_tot;
    else
        return 0;
}

double CSGSal(Indiv& X, int age)
{
    int t = X.anaiss%1900 + age;
    return M->TauxCSGSal[t] * X.salaires[age];
}

double CotAut(Indiv & X, int age)
{
    int t = X.anaiss%1900 + age;
	double bool_fds=(X.salaires[age]-M->TauxFP[t]*X.salaires[age]/(1+X.taux_prim)>309*M->PointFP[t])?1:0;
	
    if(in(X.statuts[age], Statuts_FP))
        return  M->TauxMalPubTot[t] * X.salaires[age]
              + M->TauxMalPubTr[t]  * X.salaires[age] / (1+(X.taux_prim))
			        + M->Taux_FDS[t]* part(X.salaires[age]-M->TauxFP[t]*X.salaires[age]/(1+X.taux_prim), 0 , 4*M->PlafondSS[t])*bool_fds;
    else if(X.statuts[age] == S_CAD) {
        return 
             M->TauxMalTot[t] * X.salaires[age]
            +M->TauxMalSP[t]  * part(X.salaires[age], 0 , M->PlafondSS[t])
            +M->TauxAssedic[t]* part(X.salaires[age], 0 , 4 * M->PlafondSS[t])
			      +M->Taux_APEC[t]*part(X.salaires[age], 0 , 4 * M->PlafondSS[t]); //approximation avant 2011 en dessous de 1 PSS part forfaitaire
			}
			else if (X.statuts[age] == S_NC){
			return 
			      M->TauxMalTot[t] * X.salaires[age]
            +M->TauxMalSP[t]  * part(X.salaires[age], 0 , M->PlafondSS[t])
            +M->TauxAssedic[t]* part(X.salaires[age],  0 , 4 * M->PlafondSS[t]);
			}
			else return
			(M->TauxMalRSI[t]-0.035*(1-part(X.salaires[age], 0, 0.7*M->PlafondSS[t])/(0.7*M->PlafondSS[t])))*part(X.salaires[age], 0, 0.7*M->PlafondSS[t])
            +M->TauxMalRSI[t]*(X.salaires[age]-part(X.salaires[age], 0, 0.7*M->PlafondSS[t])); //formule vraie à partir de 2017 (avant donne une approximation)
}


double CotRet(Indiv& X, int age)
{
  int t = X.anaiss%1900 + age;
  double salaire = X.salaires[age];
  double PSS = M->PlafondSS[t];

  double cotisations_AGIRC_ARRCO = 0;
  if (t < 119 || options->anLeg < 2015 || options->NoRegUniqAgircArrco) {
    // Agirc et Arrco séparés
    double cotisations_ARRCO = 0;
    if (in(X.statuts[age], NC_NonTit)) {
      // Les assiettes de l'ASF (ancien AGFF) sont différentes de celles de l'AGFF
      double plafond_AGFF_NC_NonTit = (t < 101 || options->anLeg < 2001) ? 4 * PSS : 3 * PSS;
      
      cotisations_ARRCO =
        M->TauxARRCO_S1[t] * M->TauxAppARRCO[t]	*	part(salaire, 0, PSS)
      + M->TauxARRCO_S2[t] * M->TauxAppARRCO[t]	*	part(salaire, PSS, 3 * PSS)
      + M->TauxAGFF_S1[t] * part(salaire, 0, PSS)
      + M->TauxAGFF_S2[t] * part(salaire, PSS, plafond_AGFF_NC_NonTit);
    } else if (X.statuts[age] == S_CAD) {
      // Les cotisations de la tranche A (= 1) des cadres sont versées à l'Arrco
      cotisations_ARRCO = M->TauxARRCO_S1[t] * M->TauxAppAGIRC[t]	*	part(salaire, 0, PSS);
    }
    
    double cotisations_AGIRC = 0;
    if (X.statuts[age] == S_CAD) {
      cotisations_AGIRC =
        // La tranche A est déjà versée à Arrco
        M->TauxAGIRC_SB[t] * M->TauxAppAGIRC[t] * part(salaire, PSS, 4 * PSS)
      + M->TauxAGIRC_SC[t] * M->TauxAppAGIRC[t] * part(salaire, 4 * PSS, 8 * PSS)
      + M->TauxAGFF_S1[t] * part(salaire, 0, PSS)
      + M->TauxAGFF_S2[t] * part(salaire, PSS, 4 * PSS)
      // Extension du taux AGFF de la tranche B à C à compter du 01/01/16
      + (((t >= 116) & (options->anLeg >= 2015)) ? M->TauxAGFF_S2[t] * part(salaire, 4 * PSS, 8 * PSS) : 0)
      + M->TauxCET_S[t] * part(salaire, 0, 8 * PSS);
    }
    
    cotisations_AGIRC_ARRCO = cotisations_AGIRC + cotisations_ARRCO;
  } else {
    // Agirc et Arrco fusionnés après 2019
    cotisations_AGIRC_ARRCO = 
      M->TauxAGIRC_ARRCO_S1[t] * M->TauxAppAGIRC_ARRCO[t] * part(salaire, 0, PSS) 
    + M->TauxAGIRC_ARRCO_S2[t] * M->TauxAppAGIRC_ARRCO[t] * part(salaire, PSS, 8 * PSS)
    + (salaire > PSS ? M->TauxCET_S[t] * part(salaire, 0, 8 * PSS) : 0)
    + M->TauxCEG_S1[t] * part(salaire, 0, PSS)
    + M->TauxCEG_S2[t] * part(salaire, PSS, 8 * PSS);
  }
    
  double cotisations_RG = 
      M->TauxSalRGSP[t] * part(salaire, 0, PSS)
    + M->TauxSalRGSalTot[t] * salaire;
  
  double cot = 0;
  if (X.statuts[age] == S_NC || X.statuts[age] == S_NONTIT || X.statuts[age] == S_CAD){ 
    cot = cotisations_RG + cotisations_AGIRC_ARRCO;
  } else if (in(X.statuts[age], Statuts_FP)) {
    cot = M->TauxFP[t] * salaire / (1 + X.taux_prim);
  } else if (X.statuts[age] == S_IND) {
    cot = 
      M->TauxRSIsurP[t] * (salaire - part(salaire, 0, PSS))
    + M->TauxRSIssP[t] * part(salaire, 0, PSS);
  }
  
  return cot;  
}



double CotRetrPatr(Indiv& X, int age) {
  int t = X.anaiss%1900 + age;
  double cot = 0;
  double salaire = X.salaires[age];
  double PSS = M->PlafondSS[t];
  
  double cotisations_AGIRC_ARRCO = 0;
  if (t < 119 || options->anLeg < 2015 || options->NoRegUniqAgircArrco) {
    // Agirc et Arrco séparés
    double cotisations_ARRCO = 0;
    if (in(X.statuts[age], NC_NonTit)) {
      // Les assiettes de l'ASF (ancien AGFF) sont différentes de celles de l'AGFF
      double plafond_AGFF_NC_NonTit = (t < 101 || options->anLeg < 2001) ? 4 * PSS : 3 * PSS;
      
      cotisations_ARRCO =
        (M->TauxARRCO_1[t] - M->TauxARRCO_S1[t]) * M->TauxAppARRCO[t] * part(salaire, 0, PSS)
        + (M->TauxARRCO_2[t] - M->TauxARRCO_S2[t]) * M->TauxAppARRCO[t] * part(salaire, PSS, 3 * PSS)
        + (M->TauxAGFF_1[t] - M->TauxAGFF_S1[t]) * part(salaire, 0, PSS)
        + (M->TauxAGFF_2[t] - M->TauxAGFF_S2[t]) * part(salaire, PSS, plafond_AGFF_NC_NonTit);
    } else if (X.statuts[age] == S_CAD) {
      // Les cotisations de la tranche A (= 1) des cadres sont versées à l'Arrco
      cotisations_ARRCO = (M->TauxARRCO_1[t] - M->TauxARRCO_S1[t]) * M->TauxAppARRCO[t]	*	part(salaire, 0, PSS);
    }
    
    double cotisations_AGIRC = 0;
    if (X.statuts[age] == S_CAD) {
      cotisations_AGIRC =
        // La tranche A est déjà versée à Arrco
        (M->TauxAGIRC_B[t] - M->TauxAGIRC_SB[t]) * M->TauxAppAGIRC[t] * part(salaire, PSS, 4 * PSS)
      + (M->TauxAGIRC_C[t] - M->TauxAGIRC_SC[t]) * M->TauxAppAGIRC[t] * part(salaire, 4 * PSS, 8 * PSS)
      + (M->TauxAGFF_1[t] - M->TauxAGFF_S1[t]) * part(salaire, 0, PSS)
      + (M->TauxAGFF_2[t] - M->TauxAGFF_S2[t])  * part(salaire, PSS, 4 * PSS)
      // Extension du taux AGFF de la tranche B à C à compter du 01/01/16
      + (((t >= 116) & (options->anLeg >= 2015)) ? (M->TauxAGFF_2[t] - M->TauxAGFF_S2[t]) * part(salaire, 4 * PSS, 8 * PSS) : 0)
      + (M->TauxCET[t] - M->TauxCET_S[t]) * part(salaire, 0, 8 * PSS);
    }
    
    cotisations_AGIRC_ARRCO = cotisations_AGIRC + cotisations_ARRCO;
  }
  else {
    // Agirc et Arrco fusionnés après 2019
    cotisations_AGIRC_ARRCO = 
      (M->TauxAGIRC_ARRCO_1[t] - M->TauxAGIRC_ARRCO_S1[t]) * M->TauxAppAGIRC_ARRCO[t] * part(salaire, 0, PSS) 
    + (M->TauxAGIRC_ARRCO_2[t] - M->TauxAGIRC_ARRCO_S2[t]) * M->TauxAppAGIRC_ARRCO[t] * part(salaire, PSS, 8 * PSS)
    + (salaire > PSS ? (M->TauxCET[t] - M->TauxCET_S[t]) * part(salaire, 0, 8 * PSS) : 0)
    + (M->TauxCEG_1[t] - M->TauxCEG_S1[t]) * part(salaire, 0, PSS)
    + (M->TauxCEG_2[t] - M->TauxCEG_S2[t]) * part(salaire, PSS, 8 * PSS);
  }
  
  
  double cotisations_RG = M->TauxEmpRGSalTot[t] * salaire + M->TauxEmpRGSP[t] * part(salaire, 0, PSS);
  
  if (in(X.statuts[age], NC_NonTit) || X.statuts[age] == S_CAD)  // A corriger pour prendre en compte taux de cotisation Ircantec différent de celui de l'ARRCO
  {
    cot = cotisations_RG + cotisations_AGIRC_ARRCO;
  } else if (in(X.statuts[age], Statuts_FPE)) {
    cot = M->TauxEmplFPE[t] * salaire / (1 + X.taux_prim);
  } else if (in(X.statuts[age], Statuts_FPT)) {
    cot = M->TauxEmplFPTH[t] * salaire / (1 + X.taux_prim);     // FPT ou FPH -> on applique le taux moyen CNRACL
  } else if (X.statuts[age] == S_IND) {
    cot = 
      M->TauxEmpRGSalTot[t] * salaire
    + M->TauxEmpRGSP[t] * part(salaire, 0, PSS);
  }
  
  return cot;
}

double SalNet(Indiv& X, int age) {
  return X.salaires[age] - CotRet(X, age) - CotAut(X, age) - CSGSal(X, age);
}

double SalMoy(Indiv& X, int age, int nb_an) {
  double denom(0), num(0);
  int t = X.date(age);
  
  for(int a = age; a >= 14; a--) {
    if(in(X.statuts[a], Statuts_occ)) {
      denom++;
      num += SalNet(X,a) * M->SMPT[t] / M->SMPT[X.date(a)];
      if(--nb_an == 0) {
        return num / denom;
      }
    }
  }
  
  return 0;
}

double PNet(Indiv& X, int age) {
  if(X.retr->pension_tot)
    return X.retr->pension_tot - CSGRet(X,age) - CotMalRetrComp(X,age);
  else
    return 0;
}

double TauxRemp(Indiv& X, int age) {
    double denom = SalMoy(X,age,1);
    double num = PNet(X,age);
    if(denom > 0)
        return num / denom;
    return 0;
}

double cotAGFFTot(Indiv& X, int age) {
int t = X.anaiss%1900 + age;
	double effetLegislation_CAD = (t >= 116 && options->anLeg >= 2015) ? M->TauxAGFF_2[t] * part(X.salaires[age],4*M->PlafondSS[t] , 8*M->PlafondSS[t]) : 0;
	double effetLegislation_NON_CAD = (t >= 119 && options->anLeg >= 2015) ? M->TauxAGFF_2[t] * part(X.salaires[age],3*M->PlafondSS[t] , 8*M->PlafondSS[t]) : 0;
	
	if(X.statuts[age] == S_CAD) {
	  return 
	  M->TauxAGFF_1[t] * part(X.salaires[age], 0 , M->PlafondSS[t])
	  + M->TauxAGFF_2[t] * part(X.salaires[age], M->PlafondSS[t], 4 * M->PlafondSS[t])
	  + effetLegislation_CAD; // extension du taux AGFF de la tranche B à C à compter du 01/01/16
	} else if(in(X.statuts[age], NC_NonTit)) {
	  return 
	  M->TauxAGFF_1[t] * part(X.salaires[age], 0, M->PlafondSS[t])
	  + M->TauxAGFF_2[t] * part(X.salaires[age], M->PlafondSS[t], 3 * M->PlafondSS[t])
	  + effetLegislation_NON_CAD;
	}
	else {return 0;}
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