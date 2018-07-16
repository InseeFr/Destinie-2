#include "Simulation.h"
#include "OutilsBase.h"
#include "DroitsRetr.h"
#include "Retraite.h"

const vector<int> NC_NonTit = {S_NC, S_NONTIT};

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
        return M->TauxCSGRetFort[t]*X.retr->pension_tot;
    else
        return 0;
}

double CSGSal(Indiv& X, int age)
{
    int t = X.anaiss%1900 + age;
    return M->TauxCSGSal[t]*X.salaires[age];
}

double CotAut(Indiv & X, int age)
{
    int t = X.anaiss%1900 + age;
	
	
    if(in(X.statuts[age], Statuts_FP))
        return  M->TauxMalPubTot[t] * X.salaires[age]
              + M->TauxMalPubTr[t]  * X.salaires[age] / (1+(X.taux_prim))
			        + M->Taux_FDS[t]* part(X.salaires[age]-M->TauxFP[t]*X.salaires[age]/(1+X.taux_prim), 0 , 4*M->PlafondSS[t])
			        *(X.salaires[age]-M->TauxFP[t]*X.salaires[age]/(1+X.taux_prim)>309*M->PointFP[t])?1:0;
    else if(X.statuts[age] == S_CAD) {
        return 
             M->TauxMalTot[t] * X.salaires[age]
            +M->TauxMalSP[t]  * part(X.salaires[age], 0 ,   M->PlafondSS[t])
            +M->TauxAssedic[t]* part(X.salaires[age], 0 , 4*M->PlafondSS[t])
			      +M->Taux_APEC[t]*part(X.salaires[age], 0 , 4*M->PlafondSS[t]); //approximation avant 2011 en dessous de 1 PSS part forfaitaire
			}
			else if (X.statuts[age] == S_NC){
			return 
			      M->TauxMalTot[t] * X.salaires[age]
            +M->TauxMalSP[t]  * part(X.salaires[age], 0 ,   M->PlafondSS[t])
            +M->TauxAssedic[t]* part(X.salaires[age],  0 , 4*M->PlafondSS[t]);
			}
			else return
			(M->TauxMalRSI[t]-0.035*(1-part(X.salaires[age], 0, 0.7*M->PlafondSS[t])/(0.7*M->PlafondSS[t])))*part(X.salaires[age], 0, 0.7*M->PlafondSS[t])
            +M->TauxMalRSI[t]*(X.salaires[age]-part(X.salaires[age], 0, 0.7*M->PlafondSS[t])); //formule vraie à partir de 2017 (avant donne une approximation)
}


double CotRet(Indiv & X, int age)
{
   double cot=0;
   int a = age;
   int t = X.anaiss%1900 + age;
   
   double effetLegislationARRCO_bis = (t < 119 || options->anLeg <2015 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco) ? M->TauxARRCO_S1[t]*M->TauxAppARRCO[t]*part(X.salaires[a],               0,  M->PlafondSS[t]) : 0;
   double effetLegislationARRCO_ter = (t >= 119 && options->anLeg >=2015 && !options->NoAccordAgircArrco && !options->NoRegUniqAgircArrco) ? M->TauxAGIRC_ARRCO_S1[t]*M->TauxAppAGIRC_ARRCO[t]*part(X.salaires[a],               0,  M->PlafondSS[t]) : 0;
   double effetLegislationARRCO = ((t < 119 || options->anLeg <2015 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco) && in(X.statuts[age], NC_NonTit)) ? M->TauxARRCO_S2[t]*M->TauxAppARRCO[t]*part(X.salaires[a],  M->PlafondSS[t],3*M->PlafondSS[t]) : 0;
   double effetLegislationAGIRC = ((t < 119 || options->anLeg <2015 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco) && X.statuts[age]==S_CAD) ? M->TauxAGIRC_SB[t]*M->TauxAppAGIRC[t]*part(X.salaires[a],  M->PlafondSS[t],4*M->PlafondSS[t]) + M->TauxAGIRC_SC[t]*M->TauxAppAGIRC[t]*part(X.salaires[a],4*M->PlafondSS[t],8*M->PlafondSS[t]) : 0;
   double effetLegislationAGIRC_ARRCO = (t >= 119 && options->anLeg >=2015 && !options->NoAccordAgircArrco && !options->NoRegUniqAgircArrco) ? M->TauxAGIRC_ARRCO_S2[t] * M->TauxAppAGIRC_ARRCO[t] * part(X.salaires[a], M->PlafondSS[t],8*M->PlafondSS[t]) : 0;
   double effetLegislation_CAD = (t >= 116 && options->anLeg >=2015 && !options->NoAccordAgircArrco) ? M->TauxAGFF_S2[t] * part(X.salaires[age],4*M->PlafondSS[t] , 8*M->PlafondSS[t]) : 0;
   double effetLegislation_NON_CAD = (t >= 119 && options->anLeg >=2015 && !options->NoAccordAgircArrco) ? M->TauxAGFF_S2[t] * part(X.salaires[age],3*M->PlafondSS[t] , 4*M->PlafondSS[t]) : 0;
   //double effetLegislationCET = (t >= 119 && options->anLeg >=2015 && !options->NoAccordAgircArrco) ? M->TauxCET_S[t] * part(X.salaires[age],0 , 8*M->PlafondSS[t]) : 0;
   
   
   
   if (X.statuts[a]==S_NC || X.statuts[a]==S_NONTIT || X.statuts[a]==S_CAD){ 
       cot= M->TauxSalRGSalTot[t]*X.salaires[a]
            + M->TauxSalRGSP[t] *part(X.salaires[a], 0,  M->PlafondSS[t])
			      + effetLegislationARRCO
			      + effetLegislationARRCO_bis
			      + effetLegislationARRCO_ter
			      + effetLegislationAGIRC
			      + effetLegislationAGIRC_ARRCO;
		if(X.statuts[age] == S_CAD) {	cot+=            +M->TauxAGFF_S1[t] * part(X.salaires[age],  0 ,   M->PlafondSS[t])
            + M->TauxAGFF_S2[t] * part(X.salaires[age],M->PlafondSS[t] , 4*M->PlafondSS[t])
			      + effetLegislation_CAD // extension du taux AGFF de la tranche B à C à compter du 01/01/16
		        + M->TauxCET_S[t] * part(X.salaires[age],0 , 8*M->PlafondSS[t]);}
		else {cot+= (in(X.statuts[age], NC_NonTit)) * (M->TauxAGFF_S1[t] * part(X.salaires[age],  0 ,   M->PlafondSS[t])
              + M->TauxAGFF_S2[t] * part(X.salaires[age],M->PlafondSS[t] , 3*M->PlafondSS[t])
			        + effetLegislation_NON_CAD);}
      
	}
	
   if (in(X.statuts[a],Statuts_FP))
      cot=M->TauxFP[t]*X.salaires[a]/(1+X.taux_prim);
   if (X.statuts[a] == S_IND)
      cot= M->TauxRSIsurP[t]*(X.salaires[a]-part(X.salaires[a],0,M->PlafondSS[t]))
            +M->TauxRSIssP[t]*part(X.salaires[a],0,M->PlafondSS[t]);
   
   return cot;  
}



   
double CotRetrPatr(Indiv & X, int age) {
    int t = X.anaiss%1900 + age;
	int a =age;
    double cot=0;
	
   double effetLegislationARRCO_bis = (t < 119 || options->anLeg <2015 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco) ? (M->TauxARRCO_1[t] - M->TauxARRCO_S1[t])*M->TauxAppARRCO[t]*part(X.salaires[a],               0,  M->PlafondSS[t]) : 0;
   double effetLegislationARRCO_ter = (t >= 119 && options->anLeg >=2015 && !options->NoAccordAgircArrco && !options->NoRegUniqAgircArrco) ? (M->TauxAGIRC_ARRCO_1[t] -M->TauxAGIRC_ARRCO_S1[t])*M->TauxAppAGIRC_ARRCO[t]*part(X.salaires[a],               0,  M->PlafondSS[t]) : 0;
   double effetLegislationARRCO = ((t < 119 || options->anLeg <2015 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco) && in(X.statuts[age], NC_NonTit)) ? (M->TauxARRCO_2[t] -M->TauxARRCO_S2[t])*M->TauxAppARRCO[t]*part(X.salaires[a],  M->PlafondSS[t],3*M->PlafondSS[t]) : 0;
   double effetLegislationAGIRC = ((t < 119 || options->anLeg <2015 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco) && X.statuts[age]==S_CAD) ? (M->TauxAGIRC_B[t] -M->TauxAGIRC_SB[t])*M->TauxAppAGIRC[t]*part(X.salaires[a],  M->PlafondSS[t],4*M->PlafondSS[t]) + (M->TauxAGIRC_C[t] -M->TauxAGIRC_SC[t])*M->TauxAppAGIRC[t]*part(X.salaires[a],4*M->PlafondSS[t],8*M->PlafondSS[t]) : 0;
   double effetLegislationAGIRC_ARRCO = (t >= 119 && options->anLeg >=2015 && !options->NoAccordAgircArrco && !options->NoRegUniqAgircArrco) ? (M->TauxAGIRC_ARRCO_2[t] -M->TauxAGIRC_ARRCO_S2[t]) * M->TauxAppAGIRC_ARRCO[t] * part(X.salaires[a], M->PlafondSS[t],8*M->PlafondSS[t]) : 0;
   //double effetLegislationCET = ((t < 119 || options->anLeg <2015 || options->NoAccordAgircArrco) && X.statuts[age]==S_CAD) ? (M->TauxCET[t] - M->TauxCET_S[t]) * part(X.salaires[age],0 , 8*M->PlafondSS[t]) : 0;
   //double effetLegislationCET_bis = (t >= 119 && options->anLeg >=2015 && !options->NoAccordAgircArrco) ? (M->TauxCET[t] - M->TauxCET_S[t]) * part(X.salaires[age],0 , 8*M->PlafondSS[t]) : 0;
   double cotisCET= X.statuts[age]==S_CAD ? (M->TauxCET[t] - M->TauxCET_S[t]) * part(X.salaires[age],0 , 8*M->PlafondSS[t]) : 0;
   double effetLegislationAGFF_2 = ((t < 119 || options->anLeg <2015 || options->NoAccordAgircArrco) && X.statuts[age]==S_CAD) ? (M->TauxAGFF_2[t]-M->TauxAGFF_S2[t]) * part(X.salaires[age],M->PlafondSS[t] , 4*M->PlafondSS[t]) : 0;
   double effetLegislationAGFF_2_bis = ((t < 119 || options->anLeg <2015 || options->NoAccordAgircArrco) && in(X.statuts[age], NC_NonTit)) ? (M->TauxAGFF_2[t]-M->TauxAGFF_S2[t]) * part(X.salaires[age],M->PlafondSS[t] , 3*M->PlafondSS[t]) : 0;
   double effetLegislationAGFF_2_ter = (t >= 119 && options->anLeg >=2015 && !options->NoAccordAgircArrco && X.statuts[age]==S_CAD) ? (M->TauxAGFF_2[t]-M->TauxAGFF_S2[t]) * part(X.salaires[age],M->PlafondSS[t] , 8*M->PlafondSS[t]) : 0;
   double effetLegislationAGFF_2_quater = (t >= 119 && options->anLeg >=2015 && !options->NoAccordAgircArrco && in(X.statuts[age], NC_NonTit)) ? (M->TauxAGFF_2[t]-M->TauxAGFF_S2[t]) * part(X.salaires[age],M->PlafondSS[t] , 4*M->PlafondSS[t]) : 0;
   
    if (in(X.statuts[age], NC_NonTit) || X.statuts[age] == S_CAD)  // A corriger pour prendre en compte taux de cotisation Ircantec différent de celui de l'ARRCO
    {
       cot= M->TauxEmpRGSalTot[t] * X.salaires[age]
            +M->TauxEmpRGSP[t] *                                           part(X.salaires[age],               0,    M->PlafondSS[t])
            + effetLegislationARRCO_bis
			+ effetLegislationARRCO_ter
			+ effetLegislationARRCO
			+ effetLegislationAGIRC
			+ effetLegislationAGIRC_ARRCO
			+(M->TauxAGFF_1[t] -M->TauxAGFF_S1[t]) * part(X.salaires[age],             0 ,   M->PlafondSS[t])
            + effetLegislationAGFF_2
			+ effetLegislationAGFF_2_bis
			+ effetLegislationAGFF_2_ter
			+ effetLegislationAGFF_2_quater
			+ cotisCET;
    }
	
			
    if (in(X.statuts[age],Statuts_FPE))
    {
       cot =M->TauxEmplFPE[t] * X.salaires[age] / (1 + X.taux_prim);
    }
    if (in(X.statuts[age],Statuts_FPT))
    {
       cot =M->TauxEmplFPTH[t] * X.salaires[age] / (1 + X.taux_prim);     // FPT ou FPH -> on applique le taux moyen CNRACL
    }
    if (X.statuts[age] == S_IND)
    {
       cot= M->TauxEmpRGSalTot[t] * X.salaires[age]
            +M->TauxEmpRGSP[t] * part(X.salaires[age], 0, M->PlafondSS[t]);
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
            if(--nb_an == 0)
                return num/denom;
            
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
	double effetLegislation_CAD = (t >= 116 && options->anLeg >=2015 && !options->NoAccordAgircArrco) ? M->TauxAGFF_2[t] * part(X.salaires[age],4*M->PlafondSS[t] , 8*M->PlafondSS[t]) : 0;
	double effetLegislation_NON_CAD = (t >= 119 && options->anLeg >=2015 && !options->NoAccordAgircArrco) ? M->TauxAGFF_2[t] * part(X.salaires[age],3*M->PlafondSS[t] , 8*M->PlafondSS[t]) : 0;
	
   if(X.statuts[age] == S_CAD) {
        return 
             M->TauxAGFF_1[t] * part(X.salaires[age],             0 ,   M->PlafondSS[t])
            +M->TauxAGFF_2[t] * part(X.salaires[age],M->PlafondSS[t] , 4*M->PlafondSS[t])
			+ effetLegislation_CAD; // extension du taux AGFF de la tranche B à C à compter du 01/01/16
			}
			else if(in(X.statuts[age], NC_NonTit)) {
			return 
             M->TauxAGFF_1[t] * part(X.salaires[age],             0 ,   M->PlafondSS[t])
            +M->TauxAGFF_2[t] * part(X.salaires[age],M->PlafondSS[t] , 3*M->PlafondSS[t])
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