#include "Indicateurs_annee_COR.h"


double cotisation_retraite(Indiv& X, int age, int regime) {
  static const vector<int> cot_RG = {S_NC, S_NONTIT, S_CAD};
  double cotisation(0);
  double salaire(X.salaires[age]);
  int statut(X.statuts[age]);
  int t(X.date(age));

  if( (statut==S_AVPF) && (regime==REG_AVPF) && !options->NoAVPF && !options->NoSpcAVPF){
	 salaire = min(M->SMIC[t-1], M->PlafondSS[t]); 
     cotisation += (M->TauxEmpRGSalTot[t] + M->TauxSalRGSalTot[t]) * salaire
                 + (M->TauxEmpRGSP[t]     + M->TauxSalRGSP[t]    ) * part(salaire, 0, M->PlafondSS[t]);
  }
  
  if( in(statut,cot_RG) && (regime==REG_RG || regime==REG_TOT) )
     cotisation += (M->TauxEmpRGSalTot[t] + M->TauxSalRGSalTot[t]) * salaire
                 + (M->TauxEmpRGSP[t]     + M->TauxSalRGSP[t]    ) * part(salaire, 0, M->PlafondSS[t]);
  
  //if( in(statut, Statuts_FP) && (regime==REG_FP || regime==REG_TOT)  )
  //   cotisation += M->TauxFP[t] * salaire / (1+X.taux_prim);
  
  if( in(statut, Statuts_FPE)  && (regime==REG_FP || regime==REG_TOT) ) {
     cotisation +=  (M->TauxFP[t] + M->TauxEmplFPE[t]) * salaire / (1+X.taux_prim);
  }
  
  if( in(statut, Statuts_FPT) && (regime==REG_FP || regime==REG_TOT)) {
     cotisation += (M->TauxFP[t] + M->TauxEmplFPTH[t]) * salaire / (1+X.taux_prim);
  }
  
  if(t < 119 || options->anLeg < 2015 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco) {
	  if( in(statut,cot_RG) && (regime==REG_AR || regime==REG_TOT)) {
		 cotisation += M->TauxARRCO_1[t] * M->TauxAppARRCO[t] * part(salaire, 0,  M->PlafondSS[t])
					  + M->TauxAGFF_1[t] * part(X.salaires[age],             0 ,   M->PlafondSS[t]);
		 if( statut != S_CAD ) 
			  cotisation += M->TauxARRCO_2[t] * M->TauxAppARRCO[t]* part(salaire,  M->PlafondSS[t], 3*M->PlafondSS[t])
							+ M->TauxAGFF_2[t] * part(X.salaires[age],           M->PlafondSS[t] ,   3*M->PlafondSS[t]);
	  }
	  
	  if( statut == S_CAD && (regime==REG_AG || regime==REG_TOT)){
		 cotisation +=  M->TauxAGIRC_B[t]*M->TauxAppAGIRC[t] * part(salaire,  M->PlafondSS[t], 4*M->PlafondSS[t])
						+ M->TauxAGIRC_C[t]*M->TauxAppAGIRC[t] * part(salaire, 4*M->PlafondSS[t],8*M->PlafondSS[t])
						+  M->TauxAGFF_2[t] * part(X.salaires[age],           M->PlafondSS[t] ,   8*M->PlafondSS[t])
						+ M->TauxCET[t] * part(X.salaires[age],0 , 8*M->PlafondSS[t]); // la CET est effectivement vers?e ? l'Agirc
	  }
 }  
  if( t >=119 && options->anLeg >=2015 && (!options->NoAccordAgircArrco) && (!options->NoRegUniqAgircArrco) &&in(statut,cot_RG) && (regime==REG_AGIRC_ARRCO || regime==REG_TOT)) {
    cotisation += M->TauxAGIRC_ARRCO_1[t] * M->TauxAppAGIRC_ARRCO[t] * part(salaire, 0,  M->PlafondSS[t]) 
				+ M->TauxAGIRC_ARRCO_2[t] * M->TauxAppAGIRC_ARRCO[t] * part(salaire, M->PlafondSS[t],8*M->PlafondSS[t])
				+  M->TauxAGFF_1[t] * part(X.salaires[age],           0 ,   M->PlafondSS[t])
				+  M->TauxAGFF_2[t] * part(X.salaires[age],           M->PlafondSS[t] ,   8*M->PlafondSS[t])
				+ M->TauxCET[t] * part(X.salaires[age],0 , 8*M->PlafondSS[t]);
	
  }
  if( statut == S_IND && (regime==REG_IN || regime==REG_TOT)){
     cotisation +=    (M->TauxRSIsurP[t] ) * (salaire-part(salaire, 0, M->PlafondSS[t]) )
                 + (M->TauxRSIssP[t])         * part(salaire, 0, M->PlafondSS[t]) ;
  }
  return cotisation;
}


deque<int> regimes_cot(Indiv& X, int age) {
  static const vector<int> cot_RG = {S_NC, S_NONTIT, S_CAD};
  deque<int> regimes;
  int annee = X.anaiss +age;
  
  int statut(X.statuts[age]);
  
  if( in(statut,cot_RG) )  regimes.push_back(REG_RG);
  if( in(statut, Statuts_FP) )  regimes.push_back(REG_FP);
  if( in(statut, Statuts_FPE) )  regimes.push_back(REG_FPE);
  if( in(statut, Statuts_FPT) )  regimes.push_back(REG_FPT);
  if( statut == S_IND )  regimes.push_back(REG_IN);
  if (annee < 2019 || options->anLeg < 2015 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco) {
	if( in(statut,cot_RG) )  regimes.push_back(REG_AR);
	if( statut == S_CAD )  regimes.push_back(REG_AG);
}

  if (annee >=2019 && options->anLeg >=2015 && !options->NoAccordAgircArrco && !options->NoRegUniqAgircArrco) {
    if( in(statut,cot_RG) ) { regimes.push_back(REG_AGIRC_ARRCO);}
}
  if( regimes.size() )  regimes.push_back(REG_TOT);
  
  if ( statut == S_AVPF) regimes.push_back(REG_AVPF);
  
  return regimes;
}


void ecriture_indicateurs_an(int t) {
  static const Levels regimes_labels({"tot","rg","in","fp","ar","ag","ar_ag","fpe","fpt","avpf"});
  static const Levels sexe_labels({"ens","hom","fem"});
  array<array<Indic_annee, 10>, 3> indic_annee = {}; 
  static const vector<int> cot_RG = {S_NC, S_NONTIT, S_CAD};
  static const vector<int> Regimes = {REG_TOT, REG_RG, REG_IN, REG_FP, REG_AR, REG_AG,REG_AGIRC_ARRCO, REG_FPE, REG_FPT, REG_AVPF};

  for(auto& X : pop)
  if(X.est_present(t))
  for(auto sexe : {0,X.sexe})
  {
    int age = X.age(t);
    
      
    // Statistiques sur les cotisations par régime
    if(!X.retr->liq || X.retr->liq->ageliq >= age || X.retr->liq->ageliq==0) {
        double salaire   = X.salaires[age];
		if(X.statuts[age] == S_AVPF && !options->NoAVPF && !options->NoSpcAVPF) {
                 salaire = min(M->SMIC[t-1], M->PlafondSS[t]);
              }
        //double indprix   = 1 / M->Prix[t];
        //double partavtprimo = X.retr->liq ? X.retr->liq->partavtprimo(age) : 1;
        double sal_tot      = salaire;
        double sal_plaf     = part(salaire, 0, M->PlafondSS[t]);
        //double sal_relatif  = salaire / M->SMPT[t];
        auto reg_cot = regimes_cot(X, age);
		//if(X.Id==255 && sexe==0 && t>=100) {Rcout << " X.Id " << X.Id << " t " << t <<endl;}
        //double partavtprimo_liq =1.0;
	    //if(X.retr->primoliq && in(X.statuts[X.retr->primoliq->ageprimoliq],Statuts_FP)) {partavtprimo_liq=(trimInf(X.retr->primoliq->agefin_primoliq, X.moisnaiss + 1)%4)/4.0;}
	
        for(auto& regime : reg_cot) {
           auto& indic = indic_annee[sexe][regime];
           int reg = regime;
           if(reg==REG_FPE || reg==REG_FPT)
              reg = REG_FP;
		   bool cotisant_31dec = !X.retr->liq || X.retr->liq->ageprimoliq > age || ((X.retr->liq->ageliq > age||X.retr->liq->ageliq==0) && reg!=REG_FP)  ;
		   double partavt = X.retr->liq ? (reg==REG_FP ? X.retr->liq->partavtprimo(age) : X.retr->liq->partavtliq(age)) : 1;		   
           double cotis = cotisation_retraite(X,age,reg);
		   double cotis_ma = X.retr->liq ? (reg==REG_TOT ? 
							X.retr->liq->partavtprimo(age) *cotisation_retraite(X,age,REG_FP)+
							X.retr->liq->partavtliq(age) *(cotisation_retraite(X,age,REG_TOT)-cotisation_retraite(X,age,REG_FP)) : cotis*partavt) : cotis;
		   //double cotisAGFF =cotAGFFTot(X,age);
		   //if(X.Id==255 && sexe==0 && t>=100) {Rcout << " X.Id " << X.Id << " t " << t << " regime " << reg <<endl;}
		   // valeur au 31/12
		   indic.Eff_Cotisants  .push( cotisant_31dec, 1 , M->poids/1000);
		   indic.Sal_Plaf_31dec       .push( true, sal_plaf     , M->poids/1000000);
           indic.Sal_Tot_31dec        .push( true, sal_tot      , M->poids/1000000);
		   
		   // indicateurs calculés en moyenne annuelle
           indic.Eff_Cotisants_ma  .push( true, partavt , M->poids/1000);
           indic.Sal_Plaf       .push( true, sal_plaf*partavt     , M->poids/1000000);
           indic.Sal_Tot        .push( true, sal_tot*partavt      , M->poids/1000000);
           indic.M_Cotisations  .push( true, cotis_ma        , M->poids/1000000); // modif 31/07/2017 : sp?cifique aux engagements implicites. Pour les cotis, on consid?re la valeur au 31/12.
           //indic.Sal_relatif    .push( true, sal_relatif  , M->poids*partavtprimo);
		   /*if(regime==REG_AG||regime==REG_AGIRC_ARRCO||regime==REG_TOT) {
		   indic.M_Agff  .push( true, cotisAGFF        , M->poids/1000000);
		   } */
        }
    }

	
	// Statistiques sur les droits dérivés
    for(auto& regime : Regimes) {
      auto& indic = indic_annee[sexe][regime];
	  
    if(X.retr->liqrev) {
	int typeFPConj = pop[X.retr->liqrev->idConj].typeFP;
      if(regime==REG_FPE && typeFPConj != FPE)
          continue;
      if(regime==REG_FPT && typeFPConj != FPTH)
          continue;
            
      int reg = (regime==REG_FPE || regime==REG_FPT) ? REG_FP : regime;
     
	double rev = X.retr->reversion(reg);
	double pensRev = ((regime==REG_FPE && typeFPConj != FPE) || (regime==REG_FPT && typeFPConj != FPTH)) ? 0 : rev;
	bool ind_rev = pensRev > 0;
	
	// minimum vieillesse
      double min_vieil = X.retr->min_vieil;
      bool ind_mv = min_vieil > 0;
	            
      indic.Eff_Derive        .push(ind_rev    , 1, M->poids/1000.0);
      indic.M_Derive          .push(ind_rev    , pensRev, M->poids/1000000.0); 
      indic.Eff_Derive_seul   .push(ind_rev && X.retr->pension_tot == 0, 1, M->poids/1000.0);
      indic.M_Derive_seul     .push(ind_rev && X.retr->pension_tot == 0, rev, M->poids/1000000.0);
      indic.Eff_Flux_Derive   .push(ind_rev && X.retr->liqrev->ageliq_rev == age, 1, M->poids/1000.0); 
      //M_Majo_Derive     (ind_rev && X.retr->liqrev->ageliq == age, 1, M->poids/1000.0);
	  //indic.Eff_Retr_Derive   .push(retr && ind_rev    , 1, M->poids/1000.0);
	  
	
	  
      indic.PMoy_Flux_Derive  .push(ind_rev && X.retr->liqrev->ageliq_rev == age, rev, M->poids/1000.0);
	  
	  indic.M_Flux_Derive          .push(ind_rev && X.retr->liqrev->ageliq_rev == age   , pensRev, M->poids/1000000.0); 
	  indic.M_Pensions_ma        .push(ind_rev , pensRev, M->poids/1000000.0); // valeur au 31/12
	  indic.M_Pensions        .push(ind_rev , pensRev, M->poids/1000000.0);
	  indic.M_MV           .push(ind_mv, pensRev, M->poids/1000000.0);
	  indic.M_Flux_MV.push((X.dateMinFlux==t) && ind_mv  , pensRev, M->poids/1000000.0);
	}  
	   
    }
	
	
    // Statistiques sur les retraites par régime
    for(auto& regime : Regimes) {
      auto& indic = indic_annee[sexe][regime];
      
      if(regime==REG_FPE && X.typeFP != FPE)
          continue;
      if(regime==REG_FPT && X.typeFP != FPTH)
          continue;
            
      int reg = (regime==REG_FPE || regime==REG_FPT) ? REG_FP : regime;
      
      // pension DD dans le r?gime
      double pens_dd = X.retr->pension(reg);
      
      bool ind_liq_dd = X.retr->liq && ((reg!=REG_TOT && X.retr->liq->age_liq(reg)==age) || (reg==REG_TOT && (X.retr->liq->ageliq==age||X.retr->liq->ageprimoliq==age)));
      // //bool retr = X.retr->liq && X.retr->liq->age_liq(reg)<=age;
	  
      // Statistiques des droits directs
      if(pens_dd > 0) {
          bool min_reg = X.retr->liq->min_reg(reg);
		  double partavt = (reg==REG_FP||reg==REG_TOT) ? X.retr->partavtliq(t) : X.retr->partavttotliq(t);
		  double majo_minimum_ma = reg==REG_TOT ? 
						(1-X.retr->partavtliq(t))*X.retr->liq->majo_min(REG_FP) +
						(1-X.retr->partavttotliq(t))*X.retr->liq->majo_min(REG_RG) +
						(1-X.retr->partavttotliq(t))*X.retr->liq->majo_min(REG_IN) : (1-partavt)*X.retr->liq->majo_min(reg);
										
	    double pension_min_ma=0.0;
	    if (reg==REG_TOT) {
		pension_min_ma= (X.retr->liq->indic_mg>0)*X.retr->pension_prorat(age,REG_FP)+ 
					   (X.retr->liq->indic_mc>0)*X.retr->pension_prorat(age,REG_RG) + (X.retr->liq->indic_mc_in>0)*X.retr->pension_prorat(age,REG_IN);}
		else {pension_min_ma=X.retr->pension_prorat(age,reg);}
		
		 double pension_min=0.0;
	    if (reg==REG_TOT) {
		pension_min= (X.retr->liq->indic_mg>0)*X.retr->pension(REG_FP)+ 
					   (X.retr->liq->indic_mc>0)*X.retr->pension(REG_RG) + (X.retr->liq->indic_mc_in>0)*X.retr->pension(REG_IN);}
		else {pension_min=X.retr->pension(reg);}
						
		  // valeurs au 31/12
          indic.Eff_DD              .push(true, 1, M->poids/1000.0);
          indic.M_DD                .push(true, pens_dd, M->poids/1000000.0);
          indic.AgeMoyRetr_finannee .push(true, age, 1); // non demand?
          indic.Eff_Min             .push(min_reg   , 1, M->poids/1000) ;
          indic.M_Min               .push(min_reg   , pension_min , M->poids/1000000.0); // non demand?
		  
		  indic.Eff_DD_ma              .push(true, 1-partavt, M->poids/1000.0);
		  indic.Eff_Min_ma             .push(min_reg   , 1-partavt, M->poids/1000) ;
		  indic.M_DD_ma                .push(true, X.retr->pension_prorat(age,reg), M->poids/1000000.0);
		  indic.M_Min_ma               .push(min_reg   , pension_min_ma , M->poids/1000000.0);
		  indic.M_MICO_stock_ma			.push(min_reg   , majo_minimum_ma , M->poids/1000000.0);
		  indic.PMoy_DD   .push(true, pens_dd / M->Prix[t], M->poids) ;
      }
      
      // Statistiques sur les liquidations
      if(ind_liq_dd && (X.retr->liq->pensionliq(reg) + X.retr->liq->VFU(reg)) > 0) {
        auto& dr = *(X.retr->liq);
      //if(X.Id==255 && sexe==0) {Rcout << " X.Id " << X.Id << " t " << t << " regime " << reg <<endl;}
        bool primoliq = (reg==REG_FP && dr.agefin_primoliq != dr.agefin_totliq);
        //bool vfu = dr.VFU(reg);
		//bool vfu = reg==REG_TOT ? dr.VFU(REG_AR)*dr.VFU(REG_AG)*dr.VFU(REG_RG) : dr.VFU(reg);
		bool vfu =false;
		if (reg==REG_TOT) {
		vfu= (age==X.retr->liq->ageprimoliq && (age<X.retr->liq->ageliq||X.retr->liq->ageliq==0)) ? false :
					   (age==X.retr->liq->ageliq && age>X.retr->liq->ageprimoliq) ? dr.VFU(REG_AR)*dr.VFU(REG_AG)*dr.VFU(REG_RG) : 
					   (age==X.retr->liq->ageliq && age==X.retr->liq->ageprimoliq && X.retr->liq->pensionliq(REG_FP)>0) ? false :
					   (age==X.retr->liq->ageliq && age==X.retr->liq->ageprimoliq) ? dr.VFU(REG_AR)*dr.VFU(REG_AG)*dr.VFU(REG_RG) :false;}
		else {vfu=dr.VFU(reg);}
		
		double dernier_sal = X.SalMoyBrut(dr.agefin_primoliq, 1);
        bool min_reg = !vfu && dr.min_reg(reg);
		bool coeffSoli = (dr.coeffTemp < 1) && (regime==REG_AR||regime==REG_AG||regime==REG_AGIRC_ARRCO || regime==REG_TOT) && age==X.retr->liq->ageliq;
						 //(regime!=REG_TOT || X.retr->liq->agefin_primoliq==X.retr->liq->agefin_totliq || age!=X.retr->liq->ageliq)
		//double partavt = (reg==REG_FP||reg==REG_TOT) ? X.retr->partavtliq(t) : X.retr->partavttotliq(t);
		double partavt =0.0;
		if (reg==REG_TOT) {
		partavt= (age==X.retr->liq->ageprimoliq && (age<X.retr->liq->ageliq||X.retr->liq->ageliq==0)) ? X.retr->partavtliq(t) :
					   (age==X.retr->liq->ageliq && age>X.retr->liq->ageprimoliq) ? X.retr->partavttotliq(t) : 
					   (age==X.retr->liq->ageliq && age==X.retr->liq->ageprimoliq) ? X.retr->partavtliq(t) :0;}
		else if(reg==REG_FP) {partavt=X.retr->partavtliq(t);}
		else {partavt=X.retr->partavttotliq(t);}
		//double pensionliq_ma = reg==REG_TOT ? 
			//			(1-X.retr->partavtliq(t))*dr.pensionliq(REG_FP) +
				//		(1-X.retr->partavttotliq(t))*(dr.pensionliq(REG_TOT) - dr.pensionliq(REG_FP)) : dr.pensionliq(reg)*(1-partavt);
						
		double pensionliq_ma=0.0;
		if (reg==REG_TOT) {
		pensionliq_ma= (age==X.retr->liq->ageprimoliq && (age<X.retr->liq->ageliq||X.retr->liq->ageliq==0)) ? dr.pension_prorat(age,REG_FP) :
					   (age==X.retr->liq->ageliq && age>X.retr->liq->ageprimoliq) ? (dr.VFU(REG_RG)==0)*dr.pension_prorat(age,REG_RG)+
					   (dr.VFU(REG_AR)==0)*dr.pension_prorat(age,REG_AR)+(dr.VFU(REG_AG)==0)*dr.pension_prorat(age,REG_AG)+dr.pension_prorat(age,REG_IN) : 
					   (age==X.retr->liq->ageliq && age==X.retr->liq->ageprimoliq) ? dr.pension_prorat(age,REG_FP)+
						(dr.VFU(REG_RG)==0)*dr.pension_prorat(age,REG_RG)+(dr.VFU(REG_AR)==0)*dr.pension_prorat(age,REG_AR)+(dr.VFU(REG_AG)==0)*dr.pension_prorat(age,REG_AG)+dr.pension_prorat(age,REG_IN)
						:0;}
		else {pensionliq_ma=dr.pension_prorat(age,reg);}
		
		double pensionliq =0.0;
		if (reg==REG_TOT) {
		pensionliq= (age==X.retr->liq->ageprimoliq && (age<X.retr->liq->ageliq||X.retr->liq->ageliq==0)) ? dr.pensionliq(REG_FP) :
					   (age==X.retr->liq->ageliq && age>X.retr->liq->ageprimoliq) ? (dr.VFU(REG_RG)==0)*dr.pensionliq(REG_RG)+
					   (dr.VFU(REG_AR)==0)*dr.pensionliq(REG_AR)+(dr.VFU(REG_AG)==0)*dr.pensionliq(REG_AG)+dr.pensionliq(REG_IN) : 
					   (age==X.retr->liq->ageliq && age==X.retr->liq->ageprimoliq) ? dr.pensionliq(REG_FP)+
						(dr.VFU(REG_RG)==0)*dr.pensionliq(REG_RG)+(dr.VFU(REG_AR)==0)*dr.pensionliq(REG_AR)+(dr.VFU(REG_AG)==0)*dr.pensionliq(REG_AG)+dr.pensionliq(REG_IN)
						:0;}
		else {pensionliq=dr.pensionliq(reg);}
		
		double pensionliq_min=0.0;
	    if (reg==REG_TOT) {
		pensionliq_min= (age==X.retr->liq->ageprimoliq && (age<X.retr->liq->ageliq||X.retr->liq->ageliq==0)) ? (dr.indic_mg>0)*dr.pensionliq(REG_FP) :
					   (age==X.retr->liq->ageliq && age>X.retr->liq->ageprimoliq) ? (dr.indic_mc>0)*(dr.VFU(REG_RG)==0)*dr.pensionliq(REG_RG) + (dr.indic_mc_in>0)*dr.pensionliq(REG_IN) : 
					   (age==X.retr->liq->ageliq && age==X.retr->liq->ageprimoliq) ? (dr.indic_mg>0)*dr.pensionliq(REG_FP)+ 
					   (dr.indic_mc>0)*(dr.VFU(REG_RG)==0)*dr.pensionliq(REG_RG) + (dr.indic_mc_in>0)*dr.pensionliq(REG_IN):0;}
		else {pensionliq_min=dr.pensionliq(reg);}
		
		double pensionliq_coeffSoli=0.0;
		pensionliq_coeffSoli = reg==REG_TOT ? (dr.VFU(REG_AR)==0)*dr.pensionliq(REG_AR)+(dr.VFU(REG_AG)==0)*dr.pensionliq(REG_AG) : dr.pensionliq(reg);
		
		int ageliq_reg = 0;
		if (reg==REG_TOT) {
		ageliq_reg= (age==X.retr->liq->ageprimoliq && (age<X.retr->liq->ageliq||X.retr->liq->ageliq==0)) ? dr.ageprimoliq :
					   (age==X.retr->liq->ageliq && age>X.retr->liq->ageprimoliq) ? dr.ageliq : 
					   (age==X.retr->liq->ageliq && age==X.retr->liq->ageprimoliq) ? dr.ageliq :0;}
		else {ageliq_reg=dr.age_liq(reg);}
		
        // valeurs au 31/12
        indic.Eff_Flux            .push(!vfu      , 1, M->poids/1000.0);
        indic.PMoy_Flux           .push(!vfu      , dr.pensionliq(reg), M->poids) ; //TODO
        indic.Age_Ret_Flux        .push(!vfu      , ageliq_reg, M->poids);
        indic.Dur_Ass_Flux        .push(!vfu      , dr.duree_tot_maj, M->poids);
        indic.Eff_Flux_Primo      .push(primoliq  , 1, M->poids/1000.0);
        indic.Age_Ret_Flux_Primo  .push(primoliq  , dr.agefin_primoliq, M->poids);
        indic.Coeff_Prorat_Flux   .push(!vfu      , dr.taux_prorat(reg), M->poids);
        
        indic.Sal_fin_carr        .push(!vfu && dernier_sal > 0, dernier_sal, M->poids);
        
        indic.TauxRemplace .push(!vfu && dernier_sal > 0 , dr.pensionliq(REG_TOT)  ,  dernier_sal);
        
        indic.NB_vfu              .push(vfu      , 1, M->poids/1000.0);
        indic.M_vfu               .push(vfu      , dr.VFU(reg), M->poids/1000000.0);
        
        indic.Eff_Min_Flux        .push(min_reg   , 1, M->poids/1000.0) ;
        indic.M_Min_Flux          .push(min_reg   , pensionliq_min , M->poids/1000000.0);
		
		indic.Eff_DD_Flux_coeffSoli .push(!vfu && coeffSoli      , 1, M->poids/1000.0);
		indic.M_DD_Flux_coeffSoli  .push(!vfu && coeffSoli  , pensionliq_coeffSoli, M->poids/1000000.0) ;
		//indic.M_Flux  .push(!vfu      , dr.pensionliq(reg), M->poids/1000000.0) ;
		
		indic.M_DD_Flux  .push(!vfu     , pensionliq, M->poids/1000000.0) ;
		
		indic.Eff_Flux_ma            .push(!vfu      , 1-partavt, M->poids/1000.0);
		indic.Eff_DD_Flux_coeffSoli_ma            .push(!vfu && coeffSoli      , 1-partavt, M->poids/1000.0);
		indic.Eff_Min_Flux_ma        .push(min_reg   , 1-partavt, M->poids/1000.0) ;
		indic.M_DD_Flux_ma  .push(!vfu     , pensionliq_ma, M->poids/1000000.0) ;
		
		double pensionliq_coeffSoli_ma=0.0;
		pensionliq_coeffSoli_ma = reg==REG_TOT ? (dr.VFU(REG_AR)==0)*dr.pension_prorat(age,REG_AR)+(dr.VFU(REG_AG)==0)*dr.pension_prorat(age,REG_AG) : dr.pension_prorat(age,reg);
		
		double pensionliq_min_ma=0.0;
	    if (reg==REG_TOT) {
		pensionliq_min_ma= (age==X.retr->liq->ageprimoliq && (age<X.retr->liq->ageliq||X.retr->liq->ageliq==0)) ? (dr.indic_mg>0)*dr.pension_prorat(age,REG_FP) :
					   (age==X.retr->liq->ageliq && age>X.retr->liq->ageprimoliq) ? (dr.indic_mc>0)*(dr.VFU(REG_RG)==0)*dr.pension_prorat(age,REG_RG) + (dr.indic_mc_in>0)*dr.pension_prorat(age,REG_IN) : 
					   (age==X.retr->liq->ageliq && age==X.retr->liq->ageprimoliq) ? (dr.indic_mg>0)*dr.pension_prorat(age,REG_FP)+ 
					   (dr.indic_mc>0)*(dr.VFU(REG_RG)==0)*dr.pension_prorat(age,REG_RG) + (dr.indic_mc_in>0)*dr.pension_prorat(age,REG_IN):0;}
		else {pensionliq_min_ma=dr.pension_prorat(age,reg);}
		
		double majo_min_ma=0.0;
	    if (reg==REG_TOT) {
		majo_min_ma= (age==X.retr->liq->ageprimoliq && (age<X.retr->liq->ageliq||X.retr->liq->ageliq==0)) ? (1-X.retr->partavtliq(t))*dr.majo_min(REG_FP) :
					   (age==X.retr->liq->ageliq && age>X.retr->liq->ageprimoliq) ? (1-X.retr->partavttotliq(t))*((dr.VFU(REG_RG)==0)*dr.majo_min(REG_RG) + dr.majo_min(REG_IN)) : 
					   (age==X.retr->liq->ageliq && age==X.retr->liq->ageprimoliq) ? (1-X.retr->partavtliq(t))*dr.majo_min(REG_FP) + (1-X.retr->partavttotliq(t))*((dr.VFU(REG_RG)==0)*dr.majo_min(REG_RG) + dr.majo_min(REG_IN)) :0;}
		else {majo_min_ma=dr.majo_min(reg)*(1-partavt);}
		
		indic.M_DD_Flux_coeffSoli_ma  .push(!vfu && coeffSoli  , pensionliq_coeffSoli_ma, M->poids/1000000.0) ;
		indic.M_Min_Flux_ma          .push(min_reg   , pensionliq_min_ma , M->poids/1000000.0);
		indic.M_MICO_flux_ma		.push(min_reg   , majo_min_ma , M->poids/1000000.0);
		
      }
 
 
      indic.M_Pensions_ma        .push(X.retr->pension(reg), X.retr->pension_prorat(age,reg), M->poids/1000000.0);
	  indic.M_Pensions        .push(X.retr->pension(reg), X.retr->pension(reg), M->poids/1000000.0);
	  
      
      
      // Statistiques avec le minimum vieillesse
	
     /* indic.Eff_DD_MV         .push(ind_dd && ind_mv, 1, M->poids/1000.0);
      indic.M_DD_MV           .push(ind_dd && ind_mv, pens_dd, M->poids/1000000.0);
      //indic.Eff_DRseul_MV     .push(!ind_dd && ind_rev && ind_mv, 1, M->poids/1000.0);
      //indic.M_DRseul_MV       .push(!ind_dd && ind_rev && ind_mv, rev, M->poids/1000000.0);
	  indic.Eff_Flux_DD_MV         .push(X.retr->liq && ind_dd && ind_mv && (!X.retr->liq->VFU(reg)) && X.retr->liq->age_liq(reg)==age, 1, M->poids/1000.0);
	  //indic.Eff_Flux_DRseul_MV     .push( ind_rev && ind_mv && X.retr->liqrev->ageliq_rev == age, 1, M->poids/1000.0); //X.retr->liq->age_liq(reg)!=age&& X.retr->liqrev->ageliq_rev == age
	  indic.Eff_MV .push(ind_mv, 1, M->poids/1000.0);
	  
	  indic.Eff_Flux_MV.push(X.dateMinFlux==t && ind_mv, 1, M->poids/1000.0);
	  indic.M_Flux_MV.push((X.dateMinFlux==t) && ind_mv  , pens_dd, M->poids/1000000.0);
	  indic.M_MV           .push(ind_mv, pens_dd, M->poids/1000000.0);   */
	   
    }
	
	// statistiques sur le minimum vieillesse
	for(auto& regime : Regimes) {
      auto& indic = indic_annee[sexe][regime];
	  double min_vieil = X.retr->min_vieil;
      bool ind_mv = min_vieil > 0;
	  int typeFPConj=NONFP;
	  if(X.retr->liqrev) {typeFPConj = pop[X.retr->liqrev->idConj].typeFP;}
	  
	  int reg = (regime==REG_FPE || regime==REG_FPT) ? REG_FP : regime;
	  double pens_dd = (regime==REG_FPE && X.typeFP!=FPE)||(regime==REG_FPT && X.typeFP!=FPTH) ? 0 : X.retr->pension(reg);
      double pensRev = ((regime==REG_FPE && typeFPConj != FPE) || (regime==REG_FPT && typeFPConj != FPTH)) ? 0 : X.retr->reversion(reg);
	  int type_fp_reg= regime==REG_FPE ? FPE :
				   regime==REG_FPT ? FPTH : 9999;
	  
	  indic.Eff_MV .push(ind_mv && ((!in(regime,{REG_FPE,REG_FPT})&&pens_dd+pensRev>0)||(X.typeFP==type_fp_reg&&X.retr->liqrev&&typeFPConj==type_fp_reg&&pens_dd+pensRev>0)||
						(X.typeFP==type_fp_reg&&pens_dd>0)||(X.retr->liqrev&&typeFPConj==type_fp_reg&&pensRev>0)), 1, M->poids/1000.0);
	  indic.Eff_Flux_MV .push(X.dateMinFlux==t && ind_mv && ((!in(regime,{REG_FPE,REG_FPT})&&pens_dd+pensRev>0)||(X.typeFP==type_fp_reg&&X.retr->liqrev&&typeFPConj==type_fp_reg&&pens_dd+pensRev>0)||
						(X.typeFP==type_fp_reg&&pens_dd>0)||(X.retr->liqrev&&typeFPConj==type_fp_reg&&pensRev>0)), 1, M->poids/1000.0);
	  indic.M_MV           .push(ind_mv, pens_dd, M->poids/1000000.0);
	  indic.M_Flux_MV.push((X.dateMinFlux==t) && ind_mv  , pens_dd, M->poids/1000000.0);
	 }
  }
  
  
  // Statistiques sur les d?c?d?s
  for(auto& X : pop)
  if(X.age(t)==X.ageMax)
  for(auto sexe : {0,X.sexe})
  {
	for(auto& regime : Regimes) {
      auto& indic = indic_annee[sexe][regime];
      
	  bool indic_DD = (regime!=REG_FPE || X.typeFP == FPE) && (regime!=REG_FPT || X.typeFP == FPTH);
      int reg = (regime==REG_FPE || regime==REG_FPT) ? REG_FP : regime;
	  
	if(X.retr->liqrev) {
		int typeFPConj = pop[X.retr->liqrev->idConj].typeFP;
		bool indic_Derive = (regime!=REG_FPE || typeFPConj == FPE) && (regime!=REG_FPT || typeFPConj == FPTH);
		
		double rev = X.retr->reversion(reg);
        bool ind_rev = rev > 0;
		
		indic.Eff_Derive_Deces        .push(ind_rev && indic_Derive    , 1, M->poids/1000.0);
	    indic.M_Derive_Deces                .push(ind_rev && indic_Derive, rev, M->poids/1000000.0);
		  
    }
	  
	  double pens_dd = X.retr->pension(reg);
	  
	  
	  if(pens_dd > 0) {
		  indic.Eff_DD_Deces              .push(indic_DD, 1, M->poids/1000.0);
		  indic.M_DD_Deces                .push(indic_DD, pens_dd, M->poids/1000000.0);
      }
	  
	  
	  }
	}
	  
  static Rdout indicateurs_an("Indicateurs_an", {
        "annee","regime","sexe",
        "Eff_Coti", "Sal_Plaf", "Sal_Tot", "M_Cotisations", "Eff_DD", "M_DD", "Eff_Flux", "Eff_Flux_Primo", "NB_VFU", "M_VFU", 
        "Eff_Min_Flux", "M_Min_Flux", "Eff_Min", "M_Min", "Eff_Derive", "M_Derive", "Eff_Derive_seul", 
        "M_Pensions__Derive_seul", "Eff_Flux_Derive", "M_Majo_Derive", "Eff_DD_avec_MV", "M_Pensions_DD_avec_MV", "Eff_DeriveSeul_avec_MV", 
        "M_Pensions_DeriveSeul_avec_MV", "M_Pensions",
        "Age_Ret_Flux_primo", "Coeff_Prorat_Flux", "Sal_fin_carr",
        "AgeMoyRetr_finannee", "Pmoy_Flux", "Age_Ret_Flux", "Dur_Ass_Flux", "Pmoy_Flux_Derive", "Sal_relatif",
        "Eff_Flux_coeffSoli", "M_Flux_coeffSoli", "M_Flux", "Eff_MV", "Eff_Flux_MV", "M_Flux_MV", "M_MV",
		"Eff_DD_Deces", "Eff_Derive_Deces", "M_Agff", "M_DD_Flux", "M_Flux_Derive", "M_DD_Deces", "M_Derive_Deces", "PMoy_DD",
		"Eff_Flux_DD_MV", "Eff_Flux_DRseul_MV", "Eff_Cotisants_ma","Eff_DD_ma","Eff_Min_ma","Eff_Flux_ma","Eff_DD_Flux_coeffSoli_ma",
		"Eff_Min_Flux_ma","M_DD_Flux_ma","M_DD_ma","M_DD_Flux_coeffSoli_ma","M_Min_Flux_ma","M_Min_ma", "M_Pensions_ma",
		"M_MICO_flux_ma", "M_MICO_stock_ma", "Sal_Plaf_31dec", "Sal_Tot_31dec", "TauxRemplace"}, "regime", regimes_labels, "sexe", sexe_labels);




  // ajoute les résultats de l'année dans le data.frame
  for(int s =0; s < 3; s++) 
  for(int r =0; r < 10; r++)
  {
      auto& i = indic_annee[s][r];
      indicateurs_an.push_line(
          t+1900, r, s,
          i.Eff_Cotisants, i.Sal_Plaf, i.Sal_Tot, i.M_Cotisations, i.Eff_DD, i.M_DD, i.Eff_Flux, i.Eff_Flux_Primo, i.NB_vfu, i.M_vfu, 
          i.Eff_Min_Flux, i.M_Min_Flux, i.Eff_Min, i.M_Min, i.Eff_Derive, i.M_Derive, i.Eff_Derive_seul, 
          i.M_Derive_seul, i.Eff_Flux_Derive, i.M_Majo_Derive, i.Eff_DD_MV, i.M_DD_MV, i.Eff_DRseul_MV, 
          i.M_DRseul_MV, i.M_Pensions,
          i.Age_Ret_Flux_Primo, i.Coeff_Prorat_Flux, i.Sal_fin_carr,
          i.AgeMoyRetr_finannee, i.PMoy_Flux, i.Age_Ret_Flux, i.Dur_Ass_Flux, i.PMoy_Flux_Derive, i.Sal_relatif,
          i.Eff_DD_Flux_coeffSoli, i.M_DD_Flux_coeffSoli, i.M_Flux, i.Eff_MV, i.Eff_Flux_MV, i.M_Flux_MV, i.M_MV, i.Eff_DD_Deces,
		  i.Eff_Derive_Deces, i.M_Agff, i.M_DD_Flux, i.M_Flux_Derive, i.M_DD_Deces, i.M_Derive_Deces, i.PMoy_DD, i.Eff_Flux_DD_MV, i.Eff_Flux_DRseul_MV,
		  i.Eff_Cotisants_ma, i.Eff_DD_ma, i.Eff_Min_ma, i.Eff_Flux_ma, i.Eff_DD_Flux_coeffSoli_ma, i.Eff_Min_Flux_ma, i.M_DD_Flux_ma, i.M_DD_ma, i.M_DD_Flux_coeffSoli_ma,
		  i.M_Min_Flux_ma,i.M_Min_ma, i.M_Pensions_ma, i.M_MICO_flux_ma, i.M_MICO_stock_ma, i.Sal_Plaf_31dec, i.Sal_Tot_31dec, i.TauxRemplace
        );
  }
}

void ecriture_cotisations() {
for(auto& X : pop) {
	for(auto age : range(0,X.ageMax+1)) {
	if(!X.retr->liq || X.retr->liq->ageliq >= age ) {
        X.cotis_rg[age] = cotisation_retraite(X,age,REG_RG)*X.retr->partavttotliq(X.date(age));
		X.cotis_fp[age] = cotisation_retraite(X,age,REG_FP)*X.retr->partavtliq(X.date(age));
		X.cotis_in[age] = cotisation_retraite(X,age,REG_IN)*X.retr->partavttotliq(X.date(age));
	}
	}
	}

// export de la table cotisations
    for(auto& X : pop) for(auto age : range(0,X.ageMax)) {
          if (X.Id == 0) continue;
          static Rdout df("cotisations", {"Id","age","cotis_rg","cotis_fp","cotis_in"});
          df.push_line(X.Id, age, X.cotis_rg[age], X.cotis_fp[age], X.cotis_in[age]);
    }
}

void ecriture_salairenet() {

// export de la table salairenet
    for(auto& X : pop) for(auto age : range(0,X.ageMax)) {
          if (X.Id == 0) continue;
          static Rdout df("salairenet", {"Id","age","salaires_net"});
          df.push_line(X.Id, age, X.SalNet(X.date(age)));
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