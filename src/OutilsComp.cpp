  #include "Simulation.h"
  #include "DroitsRetr.h"
  #include "Retraite.h"
  using namespace Rcpp;

  // ajout le 05/04/2013 (approximation pour prendre en compte les exonérés de CSG/CRDS)
  double pension_net_approx(Indiv& X, DroitsRetr& dr, double agetest) {
      int agearr = int_mois(agetest, X.moisnaiss+1);
      int t = X.anaiss%1900 + agearr;
      double tauxcotretr=M->TauxCSGRetFort[t]+M->TauxMalRet[t];
      if (dr.pension<=M->SeuilExoCSG[t])
          tauxcotretr = 0;
      return dr.pension * (1-tauxcotretr);
  }
  
  double salaire_net_approx(Indiv& X, int agearr) {
      int t = X.anaiss%1900 + agearr;   
      
      double sal = X.salaires[agearr];
      double tauxcotsalmoy;
      
      // calcul du taux de cotisation pour le passage brut/net
      if(in(X.statuts[agearr],Statuts_SAL)) {
        double tauxcotsal_SP;
        double tauxcotsal_P;
        
	  if(t<119 || options->anLeg < 2015 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco) {
        tauxcotsal_SP =M->TauxCSGSal[t]+M->TauxAssedic[t]+M->TauxAGFF_1[t]+M->TauxMalSP[t]+M->TauxMalTot[t]+M->TauxARRCO_S1[t]*M->TauxAppARRCO[t]+M->TauxSalRGSP[t]+M->TauxSalRGSalTot[t];
        tauxcotsal_P = 0;
        if (X.statuts[agearr]==S_NC || X.statuts[agearr]==S_NONTIT) 
            tauxcotsal_P =M->TauxCSGSal[t]+M->TauxMalTot[t]+M->TauxAssedic[t]+M->TauxAGFF_2[t]+M->TauxARRCO_S2[t]*M->TauxAppARRCO[t]+M->TauxSalRGSalTot[t];
        else if (X.statuts[agearr]==S_CAD) 
            tauxcotsal_P =M->TauxCSGSal[t]+M->TauxMalTot[t]+M->TauxAssedic[t]+M->TauxAGFF_2[t]+M->TauxAGIRC_SB[t]*M->TauxAppAGIRC[t]+M->TauxSalRGSalTot[t];
	  }
	  else {
		tauxcotsal_SP =M->TauxCSGSal[t]+M->TauxAssedic[t]+M->TauxAGFF_1[t]+M->TauxMalSP[t]+M->TauxMalTot[t]+ M->TauxAGIRC_ARRCO_S1[t]*M->TauxAppAGIRC_ARRCO[t]+M->TauxSalRGSP[t]+M->TauxSalRGSalTot[t];
		tauxcotsal_P =M->TauxCSGSal[t]+M->TauxMalTot[t]+M->TauxAssedic[t]+M->TauxAGFF_2[t]+ (M->TauxAGIRC_ARRCO_S2[t])*M->TauxAppAGIRC_ARRCO[t]+M->TauxSalRGSalTot[t];
        }
        
        tauxcotsalmoy = (min(sal,M->PlafondSS[t])*tauxcotsal_SP+max(0.0,(sal-M->PlafondSS[t]))*tauxcotsal_P)/sal;
      }
      else if(in(X.statuts[agearr],Statuts_FP)) {
        tauxcotsalmoy = M->TauxCSGSal[t]+M->TauxMalSP[t]+M->TauxMalTot[t]+1/(1+X.taux_prim)*M->TauxFP[t];
          
      }
      else{
        // RQ : pour l'instant, on fait rapidement en ne considérant que le taux sous plafond !! => A CORRIGER !
        tauxcotsalmoy =M->TauxCSGSal[t]+M->TauxAssedic[t]+M->TauxAGFF_1[t]+M->TauxMalSP[t]+M->TauxMalTot[t]+M->TauxARRCO_S1[t]*M->TauxAppARRCO[t]+M->TauxSalRGSP[t]+M->TauxSalRGSalTot[t];
      }
      
      return sal*(1-tauxcotsalmoy);
  }
  
  
  bool test_uinst(Indiv & X, DroitsRetr & dr, Leg &l, double agetest) {      
      int agearr = int_mois(agetest, X.moisnaiss+1);
       

      double sal_net = salaire_net_approx(X,agearr);   

      double delta_k = options->delta_k;
      double kinst = max(min(1.0,X.k), (X.k * pow( 1+delta_k, max(0.0,agetest-60))));
      
      dr.Liq();
      double pension_net = pension_net_approx(X, dr, agetest);
      
      static Rdout df15("testliq",{"Id","age" ,"pension" ,"kinst","pension_net","sal_net"});
      if(options->ecrit_dr_test) df15.push_line(X.Id,dr.agetest,dr.pension, kinst ,pension_net  , sal_net);
      
      // et on compare les revenus
      if (dr.pension>0 && kinst*pension_net >= sal_net) {
        
        
        if(!dr.type_liq)  dr.type_liq = liq_cible;
        if(dr.type_liq==liq_cible) {
          if(dr.durdecote_rg || dr.durdecote_fp) {
              dr.type_liq = liq_decote;
          } 
          else if(dr.dursurcote_rg || dr.dursurcote_fp) {
              dr.type_liq = liq_surcote;
          }
          else {
              dr.type_liq = liq_tp;
              // A revoir
              if(arr_mois(dr.agetest) == arr_mois(min(l.AgeMinRG, l.AgeMinFP)))
                  dr.type_liq = liq_tp_aod;
              else if(arr_mois(dr.agetest) == arr_mois(l.AgeAnnDecRG))
                  dr.type_liq = liq_tp_aad;
              else
                  dr.type_liq = liq_tp_duree;
          }
        }
        return true;
      }
      else {
        return false;
      }    
  }
  
  
  bool test_TP(Indiv & X, DroitsRetr & dr, Leg &l, double agetest) {
      int agearr = int_mois(agetest, X.moisnaiss+1);
      // Partie uniqument pour l'option TP
      // Liquidation au taux plein  
      if(dr.duree_fp_maj >= l.DureeCibFP&&(l.AnOuvDroitFP<2006||l.an_leg<2003) && dr.tauxliq_fp >= 1) {
   //avant la reforme 2003  être au taux plein au FP correspondait à avoir la durée cible du regime
          dr.Liq();
          if(!dr.type_liq) {
              if(arr_mois(dr.agetest) == arr_mois(min(l.AgeMinRG, l.AgeMinFP)))
                  dr.type_liq = liq_tp_aod;
              else if(arr_mois(dr.agetest) == arr_mois(l.AgeAnnDecRG))
                  dr.type_liq = liq_tp_aad;
              else
                  dr.type_liq = liq_tp_duree;
          }
          return true;
      }
     
      if(l.AnOuvDroitFP>=2006&&l.an_leg>=2003 && dr.duree_fp>=l.DureeMinFP && dr.tauxliq_fp >= 1 &&(dr.duree_tot_maj >= l.DureeCibFP||arr_mois(agetest)>=arr_mois(l.AgeAnnDecFP))) {
   //apres la reforme il s'agit d'atteindre le taux plein par l'âge ou la duree symbolise 
          dr.Liq();
          if(!dr.type_liq) {
              if(arr_mois(dr.agetest) == arr_mois(min(l.AgeMinRG, l.AgeMinFP)))
                  dr.type_liq = liq_tp_aod;
              else if(arr_mois(dr.agetest) == arr_mois(l.AgeAnnDecRG))
                  dr.type_liq = liq_tp_aad;
              else
                  dr.type_liq = liq_tp_duree;
          }
          return true;
      }
          
      if(dr.duree_fp_maj > 0 && (dr.duree_rg_maj + dr.duree_in_maj) == 0 
        && arr_mois(agetest) >= arr_mois(l.AgeSurcote) && dr.tauxliq_fp >= 1 
        && !in(X.statuts[agearr], Statuts_occ)) {
          if(!dr.type_liq) {
            
              if(arr_mois(dr.agetest) == arr_mois(min(l.AgeMinRG, l.AgeMinFP)))
                  dr.type_liq = liq_tp_aod;
              else if(arr_mois(dr.agetest) == arr_mois(l.AgeAnnDecRG))
                  dr.type_liq = liq_tp_aad;
              else
                  dr.type_liq = liq_tp_duree;
          }
          dr.Liq();
          return true;
        }
          
      if(arr_mois(agetest) >= arr_mois(l.AgeMinRG) && dr.duree_fp == 0 && dr.tauxliq_rg >= 1){
          dr.Liq();
          if(!dr.type_liq) {
            
              if(arr_mois(dr.agetest) == arr_mois(min(l.AgeMinRG, l.AgeMinFP)))
                  dr.type_liq = liq_tp_aod;
              else if(arr_mois(dr.agetest) == arr_mois(l.AgeAnnDecRG))
                  dr.type_liq = liq_tp_aad;
              else
                  dr.type_liq = liq_tp_duree;
          }
          return true;
      }
      if(arr_mois(agetest) >= arr_mois(l.AgeMinRG) && min(dr.tauxliq_rg, dr.tauxliq_fp) >= 1){
          dr.Liq();
          if(!dr.type_liq) {      
              if(arr_mois(dr.agetest) == arr_mois(min(l.AgeMinRG, l.AgeMinFP)))
                  dr.type_liq = liq_tp_aod;
              else if(arr_mois(dr.agetest) == arr_mois(l.AgeAnnDecRG))
                  dr.type_liq = liq_tp_aad;
              else
                  dr.type_liq = liq_tp_duree;
          }
          return true;
      }       
          
      return false;
	    
  }
  /*
  bool test_uact(Indiv& X, DroitsRetr& dr, Leg &l, double agetest) {
      double ref=UAct(X, dr, l, agetest);
      bool test=true;
      double age_report = agetest;
      while(age_report < arr_mois(AgeMax(X,agearr))) {
          age_report += (age_report < 56 || age_report > l.AgeAnnDecRG) ? pas2 : pas1;
          if(UAct(X, age_report) > ref * (1 + X.seuil))
              return false;
      }
      return true;    
  }
  */
  
  bool test_uninst_old(Indiv & X, DroitsRetr & dr, Leg &l, double agetest) {
    int agearr = int_mois(agetest, X.moisnaiss+1);
    int t = X.anaiss%1900 + agearr;          
    double sal = X.salaires[agearr];
    
    // Imputation prestations chômage ou préretraite (très brutales)
    if(in(X.statuts[agearr], Statuts_cho)) {
      if(in(X.statuts[agearr-1], Statuts_occ)) {
        sal = X.salaires[agearr-1] * 0.7;
      }
      else {
        for(int a = agearr; a > 14; a--) {
          if(in(X.statuts[a], Statuts_occ)) {
            sal = X.salaires[agearr-1] * pow(0.7,agearr-a);
            sal = max(sal, 0.7*M->SMIC[t]);
            break;
          }
        }
      }
    }
    else if(in(X.statuts[agearr], Statuts_PRPRRG)) {
      if(in(X.statuts[agearr-1], Statuts_occ)) {
        sal = X.salaires[agearr-1] * 0.7;
      }
      else {
        for(int a = agearr-1; a > 14 ;a--) {
          if(in(X.statuts[a], Statuts_occ)) {
            sal = X.salaires[agearr-1] * 0.7;
            sal = max(sal, 0.7*M->SMIC[t]);
            break;
          }
        }
      }
    }    
        
    dr.Liq();
  
    double delta_k = options->delta_k;
    
    double kinst = max(min(1.0,X.k), (X.k * pow( 1+delta_k, max(0.0,agetest-60))));
    
    double pension_net = pension_net_approx(X, dr, agetest);
    double sal_net = salaire_net_approx(X, agearr);
    
    // et on compare les revenus
    if (dr.pension>0 && kinst*pension_net >= sal_net) {
      if(!dr.type_liq) dr.type_liq = liq_cible;
      return true;
    }
    else {
      return false;
    } 
  }
  
  
  /**
   * \fn TestLiq
   * Teste si départ en retraite de l'individu X, pour un âge donné  selon l'option de
   * comportement choisie. La mise à la reraite est systématique à 70 ans,
   * quelle que soit l'option de comportement choisie
   */
  
  bool TestLiq(Indiv & X, DroitsRetr & dr, Leg &l, double agetest)
  {
    static Rdout TestLiq("TestLiq",{"Id","type"});
    int agearr = int_mois(agetest, X.moisnaiss+1);
    int comport_liq = options->comp;
      
    bool agemin = dr.AgeMin(); 
    if(dr.duree_tot==0 || ((!agemin) && comport_liq != comp_exo)) {
        if(options->ecrit_dr_test) TestLiq.push_line(X.Id,1);
        return false;
    }
      
    
    // Liquidation d'office si age max
    if(arr_mois(agetest) >= arr_mois(dr.AgeMax())) {
        dr.Liq();
        if(!dr.type_liq) dr.type_liq = liq_agemax;
        if(options->ecrit_dr_test) TestLiq.push_line(X.Id,2);
        return true;
    }
    
    if(options->comp == comp_ageMax) {
        return false;
    }
    
    if(options->comp==comp_uinst_old)
        return test_uninst_old(X,dr,l,agetest);
    
    // Liquidation d'office si chomeur ou retraite ayant atteint le taux plein
    static vector<int> statuts_prcho = {S_CHO,S_PR}; 
    if(in(X.statuts[agearr], statuts_prcho) && agetest >= l.AgeMinRG) {
        if( (dr.duree_fp==0 && dr.tauxliq_rg>=1)
            || (min(dr.tauxliq_rg, dr.tauxliq_fp) >= 1)) {
              
            dr.Liq();
            if(!dr.type_liq) dr.type_liq = liq_tp;
            if(options->ecrit_dr_test) TestLiq.push_line(X.Id,3);
            return true;
        }
    }
    

   
    // Liquidation si invalidité
    static vector<int> statuts_invalid = {S_INVAL,S_INVALRG,S_INVALIND};
    if(in(X.statuts[agearr], statuts_invalid) && agetest >= l.AgeMinRG)  {
        // TODO vérifier effets de bord sur l.AgeMinRG X.AgeMinRG ?
        dr.Liq();
        if(!dr.type_liq) dr.type_liq = liq_invald;
        if(options->ecrit_dr_test) TestLiq.push_line(X.Id,4);
        return true;
    }
	
	  if(options->DepInact && X.statuts[agearr]==S_INA && agetest>=l.AgeMinRG && X.retr->primoliq) {
		  dr.Liq();
		  if(!dr.type_liq) dr.type_liq = liq_inact;
      if(options->ecrit_dr_test) TestLiq.push_line(X.Id,5);
      return true;
    }
    
    if(comport_liq == comp_umixt)
      comport_liq = in(X.statuts[agearr], Statuts_occ) ? comp_uinst : comp_tp;
    
    if(comport_liq == comp_tp) {
      if(options->ecrit_dr_test) TestLiq.push_line(X.Id,6);
      return test_TP(X,dr,l,agetest);
		  if (X.Id==13526) {Rcout << " X.Id test tp " << X.Id <<" agetest " << agetest <<" test "<<test_TP(X,dr,l,agetest) <<endl;}
    }
    else if(comport_liq == comp_exo) {
      if(options->ecrit_dr_test) TestLiq.push_line(X.Id,7);
      if (arr_mois(agetest) >= arr_mois(X.age_exo)) {
        dr.Liq();
        return true;
      }
      return false;
    }
    else if(comport_liq == comp_uinst) {
      if(options->ecrit_dr_test) TestLiq.push_line(X.Id,8);
      return test_uinst(X,dr,l,agetest);
    }
    
    return false;
  }  
   
  bool TestSecondLiq(Indiv & X, DroitsRetr & dr, Leg& l, double agetest)
  {
    static Rdout TestLiq2("TestLiq2",{"Id","type"});
	  int agearr = int_mois(agetest, X.moisnaiss+1);
    dr.AgeMin();
    dr.DecoteSurcote();
    TestLiq2.push_line(X.Id,0);
      
	  if(options->DepInact && X.statuts[agearr]==S_INA && agetest>=l.AgeMinRG && X.retr->primoliq) {
		  dr.SecondLiq();
      if(options->ecrit_dr_test) TestLiq2.push_line(X.Id,1);
		  return true;
    }
	
	  if(!options->SecondLiq && options->anLeg>=2014 && int_mois(X.anaiss+agetest, X.moisnaiss+1)>=2015 && (X.anaiss+X.retr->primoliq->ageprimoliq >=2015) && agetest>=l.AgeMinRG) {
		  dr.SecondLiq();
      if(options->ecrit_dr_test) TestLiq2.push_line(X.Id,2);
		  return true;
	 }
	
      if(dr.tauxliq_rg >=1 && agetest>=l.AgeMinRG) {
          dr.SecondLiq();
          if(options->ecrit_dr_test) TestLiq2.push_line(X.Id,3);
          return true;
      }
  
      return false;
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
  