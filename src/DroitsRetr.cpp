#include "DroitsRetr.h"
#include "Retraite.h"

  
DroitsRetr::DroitsRetr(const Indiv& X, Leg& leg, double agetest) :
    X(X), l(leg), agetest(agetest)
{
    // RMQ sur les âges :
    datetest = arr_mois(X.anaiss + agetest, X.moisnaiss + 1 );
    age     = int_mois(agetest, X.moisnaiss + 1 ); // âge atteint l'année de l'agetest
    t = X.date(age);
}
  
// Fonction duree_trim

double DroitsRetr::duree_trim(const vector<int>& statuts_cpt, int anneeLimite) {
  double duree=0;
  double duree_emprg=0;
  int ageLimite=X.age(anneeLimite%1900);
  // âge en fin d'année civile précédent la date testée
  // (ie précédent la date du 1er du mois suivant l'anniversaire)
  int am1 = max(age - 1, 14);
  // pour le chomage, seuls les 6 premiers trimestres de chomage non indemnisés sont comptés
  if (equal(statuts_cpt.begin(), statuts_cpt.end(), statuts_cho.begin())) {
    for(int a = 14; a <= min(am1, ageLimite); a++)
      {
	if (in(X.statuts[a], statuts_emprg )) duree_emprg=+1;
	if( in(X.statuts[a], statuts_cpt) && duree_emprg ) duree += 1;
	if ( in(X.statuts[a], statuts_cpt) && duree_emprg==0 ) {duree+=1; duree=min(duree,1.5);}
      }
  }
  else{
    for(int a = 14; a <= min(am1, ageLimite); a++)
      if( in(X.statuts[a], statuts_cpt) )
	duree += 1;
  }
    
  if( in(X.statuts[am1 + 1], statuts_cpt) && (am1+1 <= ageLimite) )
    duree += (trimInf(agetest, X.moisnaiss + 1)%4)/4.0;
  //lorsqu'on n'a pas validé/ouvert les droits attachés à la catégorie active avant l'age limite de l'emploi, il y a reclassement en sédentaire
  // on ne doit pas compter les durées suivantes en catégorie active
  double durfpa=l.durfpa+0.0;
  if (equal(statuts_cpt.begin(), statuts_cpt.end(), statuts_fpa.begin())&& l.durfpa<l.DureeMinFPA) duree=min(duree,durfpa);
  return duree ;
}


const  vector<int> statuts_cho         = {S_CHO, S_CHOBIT, S_CHONONBIT} ;
const  vector<int> statuts_pr          = {S_PR};    
const  vector<int> statuts_inv         = {S_INVAL, S_INVALRG} ;     
const  vector<int> statuts_SNat        = {S_SNAT} ; 
const  vector<int> statuts_Avpf        = {S_AVPF} ; 
const  vector<int> statuts_emprg       = {S_NC, S_NONTIT, S_CAD}  ; 
const  vector<int> statuts_fpa         = {S_FPAE, S_FPAAE, S_FPATH, S_FPAATH}; 
const  vector<int> statuts_fps         = {S_FPSE, S_FPSTH,  S_INVALFP} ; 
const  vector<int> statuts_fp          = {S_FPSE, S_FPSTH,  S_INVALFP,S_FPAE, S_FPAAE, S_FPATH, S_FPAATH} ; 
const  vector<int> statuts_cad         = {S_CAD}  ;
const  vector<int> statuts_ind         = {S_IND} ;
const  vector<int> statuts_ind_assimil = {S_IND, S_INVALIND};

void DroitsRetr::durees_base() {  
    
	double resteTrimPrimo=0.0;
	if(X.retr->primoliq && in(X.statuts[X.retr->primoliq->ageprimoliq],Statuts_FP)) {
	resteTrimPrimo = ArrTrimInf(min(1.0,arr_mois(X.anaiss+agetest,X.moisnaiss+1)-int_mois(X.anaiss+X.retr->primoliq->agefin_primoliq,X.moisnaiss+1))-X.retr->primoliq->partavtprimo(X.retr->primoliq->ageprimoliq));
		}
    duree_cho    =  duree_trim(statuts_cho);
    duree_PR     =  duree_trim(statuts_pr);
    duree_inv    =  duree_trim(statuts_inv);
    duree_snat   =  duree_trim(statuts_SNat);
    duree_avpf   =  duree_trim(statuts_Avpf);
    duree_emprg  =  duree_trim(statuts_emprg) + resteTrimPrimo; // lors d'une primo-liquidation, basculement vers NONTIT dès la date de liquidation si l'individu est dans la FP.
    duree_fpa    =  X.retr->primoliq ? X.retr->primoliq->duree_fpa : duree_trim(statuts_fpa );
    duree_fps    =  X.retr->primoliq ? X.retr->primoliq->duree_fps : duree_trim(statuts_fp )-duree_fpa;
    duree_ag     =  (l.an_leg < 2015 || t < 119 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco)? duree_trim(statuts_cad) : duree_trim(statuts_cad,2018);
    
    duree_emp    = duree_trim(Statuts_occ) ;
    duree_in     = options->NoAssimil ?
                      duree_trim(statuts_ind) :
                      duree_trim(statuts_ind_assimil );

// à faire : prendre en compte TOUTES les bonnes durées après une primo-liquidation. Ex : si t>=2015, ageliq>ageprimoliq, X.statuts[ageprimoliq]=FPE, duree_emp comptabilisera toute l'année de primo-liquidation dans 
// la FPE au lieu de la durée proratisée. Piste : récupérer toutes les durées calculées à l'issue de la primo-liquidation.
    //TODO: Scorie
    int tmp = age;
    age = 54;
    static const vector<int> test = {S_INVALIND};
    duree_in -= duree_trim(test);
    age = tmp;


    duree_rg = duree_emprg;

    if(!options->NoAVPF) 
        duree_rg += duree_avpf;

    if(!options->NoAssimil) {
        if(duree_emprg)
            duree_rg += duree_cho + duree_inv + duree_PR + duree_snat;
        else if(duree_fps + duree_fpa)
            duree_fps   += !X.retr->primoliq ? duree_PR + duree_snat : 0;
        else if(duree_in)
            duree_in += duree_snat;
    }

    if (t>48 && t<72)       
        duree_rg = max( duree_rg, 0.5*duree_rg+15);       // ?? à comprendre

    // Durées agrégées
    duree_fp  =    duree_fps +   duree_fpa;
    duree_tot =    duree_rg  +   duree_fp +   duree_in;

    //TODO: à modifier si le modèle intégre un jour des périodes de service national et de maladie
    dureecotmin_tot   = duree_emp + duree_snat;

    
    if (l.an_leg < 2012 || datetest <  arr_mois(2012, 10)) {
      dureecotdra_tot = duree_emp + min(duree_snat,1.0);  
    } 
    else if (l.an_leg < 2013 || t < 114) {              
      dureecotdra_tot = duree_emp + min(0.5, duree_cho) + min(duree_snat,1.0);
    }
    else  {
      dureecotdra_tot = duree_emp + min(1.0, duree_cho) + min(0.5, duree_inv)  + min(duree_snat,1.0);
    }                   
    
	
                        
    if( 0 < duree_fp && duree_fp < l.DureeMinFP ) {
        duree_rg    += duree_fp;
        duree_emprg += duree_fp;
        duree_fp    = duree_fpa = duree_fps = 0;
    }
	
	if(l.an_leg < 2015 || t < 119 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco) { // A revoir: faut-il prendre en compte les points acquis en cas de chômage indemnisé ?
		duree_ar = duree_trim(statuts_emprg)+resteTrimPrimo;
		//duree_ag_ar = 0;
	}
	else {
		duree_ar = duree_trim(statuts_emprg,2018);
		//duree_ag_ar = duree_rg - duree_ar;
	}
	
	duree_ag_ar =max(duree_trim(statuts_emprg) - duree_trim(statuts_emprg,2018), 0.0); // à revoir
}

/**
 * Enumeration des régimes de base
 */
enum regime {
    regime_aucun,
    regime_rg,
    regime_fp,
    regime_ind
};


/**
 * fonction durees_majo

Repart des durées calculées par DurBase, et calcule les durées majorées prenant
en compte la Mda selon les options.
*/
void DroitsRetr::durees_majo() {
    duree_rg_maj  = duree_rg;
    duree_fp_maj  = duree_fp;
    duree_in_maj  = duree_in;
    duree_tot_maj = duree_tot;
//if(X.Id==13954) {Rcout << " t "<<t<<" debut duree_tot "<<duree_tot<<endl;}
    // Ajout MDA
    if(X.sexe == FEMME && !(options->NoMDA) && X.nb_enf(age) > 0)  {
      
      int regime_ref  = regime_aucun;
  
      int e = 0;
      for(int a=14; a <= age; a++) {
          int u  = X.anaiss + a; // Changement Marion 30/01/2013 u à la place de t
  
          if( in(X.statuts[a], Statuts_FP) && duree_fp  )
              regime_ref = regime_fp;
          else if ( in(X.statuts[a], Statuts_IN) && duree_in) //TODO
              regime_ref = regime_ind;
          else if ( in(X.statuts[a], Statuts_RG) && duree_rg )
              regime_ref = regime_rg;
          
          if( u == X.anaissEnf[e]) {
              if( regime_ref == regime_rg) {
                  duree_rg_maj  += M->CoefMdaRG[t];
                  duree_tot_maj += M->CoefMdaRG[t];
              }
              else if(regime_ref == regime_ind) {
                  duree_in_maj  += M->CoefMdaRG[t];
                  duree_tot_maj += M->CoefMdaRG[t];
              }
              else if(regime_ref == regime_fp) {
                  if(u < 2004 || l.an_leg < 2004) {
                      duree_fp_maj  += M->CoefMdaFP[t];  //bonification comptant pour le calcul de la durée d'assurance et non pour la liquidation
                      duree_tot_maj += M->CoefMdaFP[t];
                  } 
                  else
                      duree_tot_maj += 0.5 * M->CoefMdaFP[t]; //majoration uniquement de durée d'assurance
              }
              e++;
          }
    
          if (e >= X.nb_enf(age)) 
              break;
      }
    }
  
   // Ajout de la durée d'assurance pour âge au régime général (26/02/2015 )
   // (pour plus d'info sur cette MDA : 
   //     http://www.legislation.cnav.fr/Pages/expose.aspx?Nom=mda_mda_age_mda_age_ex/ 
   //     et circulaire : http://www.legislation.cnav.fr/Pages/texte.aspx?Nom=CR_CN_2004020_13042004)

  
  	if (!options->NoMDA && !options->NoMDAAge)
  	{
      double majoage(0);
      
  		if (t >= 84 && l.an_leg>1982 && arr(12*agetest)>arr(12*l.AgeAnnDecRG))
  		{
  			double depasse = int((arr(12 * agetest) - arr(12 * l.AgeAnnDecRG)) / 3);    // = nb de trimestres d'ajournement au-delà de l'âge d'annulation de la décote
  
  			// législation entre 1984 et 2004 (modification en 2003 applicable seulement aux générations n'ayant pas encore atteint 60 ans => effet à partir de 2009 seulement)
  			if (t <= 108 || l.an_leg<2004)
  			{
  				// MDA pour âge au RG
  				if (duree_rg_maj>0 && duree_rg_maj<l.DureeProratRG)
  				{
  					majoage = min((l.DureeProratRG - duree_rg_maj), ArrTrimSup(0.025*depasse*duree_rg_maj));
  					duree_rg_maj += majoage;
  					duree_tot_maj += majoage;
  				}
  				// MDA pour âge chez les indépendants
  				if (duree_in_maj>0 && duree_in_maj<l.DureeProratRG)
  				{
  					majoage = min((l.DureeProratRG - duree_in_maj), ArrTrimSup(0.025*depasse*duree_in_maj));
  					duree_in_maj += majoage;
  					duree_tot_maj += majoage;
  				}
  			}
  
  			// législation à partir de 2004
  			else {
  				if ((duree_rg_maj + duree_in_maj>0) && (duree_tot_maj<l.DureeProratRG))  				{
  					majoage = min((l.DureeProratRG - duree_tot_maj), ArrTrimSup(0.025*depasse*(duree_rg_maj + duree_in_maj)));
  					if (duree_in_maj>0){ duree_rg_maj += ArrTrimSup(majoage*duree_rg_maj / (duree_rg_maj + duree_in_maj));
  					duree_in_maj += (majoage - ArrTrimSup(majoage*duree_rg_maj / (duree_rg_maj + duree_in_maj)));
  					duree_tot_maj += majoage;}
  					else{duree_rg_maj+=majoage;
  					  duree_tot_maj+=majoage;}
  				}
  			}
  
  		}
  	}
	
	
}

// Fonction DecoteSurcote
/*  Cette fonction calcule les durées de décote et surcote.  Remplit les variables 
  durdecote_rg, dursurcote_rg, durdecote_fp, dursurcote_fp.
  Cette fonction nécessite d'avoir auparavant lancé les fonctions de calcul des durées 
  (durees_majo et durees_base)
*/

void DroitsRetr::DecoteSurcote()
{
    int agedebsurcote = int_mois(l.AgeSurcote, X.moisnaiss);
    double agedebsurcote_rev = arr_mois(l.AgeSurcote, X.moisnaiss + 1);
    double dursurcote = 0;
  
      
    int a_min = max(agedebsurcote,2004-X.anaiss);
    for(int a=a_min; a <= age; a++) {
        if      (!in(X.statuts[a],Statuts_occ) ) dursurcote += 0.0 ;
        else if (a == agedebsurcote && a==age) dursurcote += max(0.0, ArrTrimInf(datetest-X.anaiss-agedebsurcote_rev));
        else if (a == agedebsurcote && a <age) dursurcote += max(0.0, agedebsurcote + 1 - ArrTrimSup(agedebsurcote_rev));
        else if (a<age                       ) dursurcote += 1 ;
        else if (a==age                      ) dursurcote += ArrTrimInf(datetest-1900-t);
    }
  
    // Calcul de la décote et de la surcote à la FP
    // RQ : dans le cas d'une seconde liquidation, on ne recalcule pas la décote et la surcote dans le régime déjà liquidé, cad la FP
    if (!X.retr->primoliq)
    {
        /// calcul de la durée de décote à la FP
        if (l.an_leg<2003)   durdecote_fp = 0;
        else durdecote_fp = max(0.0,
             min(ArrTrimSup(arr_mois(l.AgeAnnDecFP-agetest)),
                 ArrTrimSup(l.DureeCibFP-duree_tot_maj)));
  
        /// calcul de la durée de surcote à la FP
        dursurcote_fp = 0;
        if ( in(X.statuts[agedebsurcote], Statuts_occ) && (t>103) && (l.an_leg>=2003))
          dursurcote_fp = max(0.0,
              min( ArrTrimInf(duree_tot_maj-l.DureeCibFP) ,
                   ArrTrimInf(datetest)-ArrTrimSup(max(2004.0,arr_mois(X.anaiss+agedebsurcote_rev) ) ) ));
				  // if(X.Id==53074) {Rcout << " X.Id " << X.Id <<" agetest " << agetest << " l.DureeCibFP " << l.DureeCibFP << " duree_tot_maj " << duree_tot_maj<<
				   //" " <<ArrTrimInf(duree_tot_maj-l.DureeCibFP) << " " << ArrTrimInf(datetest)-ArrTrimSup(max(2004.0,arr_mois(X.anaiss+agedebsurcote_rev) ) ) << " decote " << durdecote_fp <<endl;} 
    }
  
    // calcul de la durée de décote au RG
    durdecote_rg     = max(0.0, min(ArrTrimSup(l.AgeAnnDecRG-agetest),
                                    ArrTrimSup(l.DureeCibRG-duree_tot_maj)));
  
    // calcul de la durée de surcote au RG  
    // (rq. on fait l'hypothèse que si la personne est en emploi, 
    //  elle l'a été continûment depuis son âge d'ouverture des droits
    if (t<83) {  dursurcote_rg = max(0.0, ArrTrimInf(agetest-l.AgeAnnDecRG)); }
    else if(arr_mois(X.anaiss+65,X.moisnaiss+1) <= 1983+3/12.0){
        dursurcote_rg = ArrTrimInf(min(arr_mois(1983-X.anaiss-l.AgeAnnDecRG,X.moisnaiss),3.0-1.0));
    }
    else
        dursurcote_rg = 0;
  
    if ( in(X.statuts[agedebsurcote], Statuts_occ) && (t>103) && (l.an_leg>2003))
        dursurcote_rg = max(0.0, min( ArrTrimInf(duree_tot_maj-l.DureeCibRG) , dursurcote ));
		
  
    // Annulation de la décote en cas de liquidation au titre de l'invalidité
    static vector<int> statuts_inval = {S_INVAL, S_INVALRG, S_INVALIND};
    double k_median_ho     = options->k_median_ho; 
    double k_median_fe     = options->k_median_fe;
    double k_def_q_ho      = options->k_def_q_ho ;
    double k_def_q_fe      = options->k_def_q_fe ;
    double k_val_q_ho      = options->k_val_q_ho ;
    double k_val_q_fe      = options->k_val_q_fe ;
    
    
    if ( !options->NoInaptLiqTauxPlein && in(X.statuts[age], statuts_inval) )
    {
      durdecote_rg = 0;
      if (!X.retr->primoliq) durdecote_fp = 0;
    }
    
    
    // Annulation de la décote en cas de liquidation au titre de l'inaptitude HORS INVALIDITE
    // -> pour les X% des hommes et Y% des femmes (% calibrés à partir de l'EIR 2008) ayant le taux de préférence pour l'inactivité le plus élevé,
    //    on considére qu'il y a reconnaissance de l'inaptitude au travail, et donc liquidation sans décote
    // (RQ : modification des valeurs le 28/03/2013 suite à la prise en compte des statuts invalidité => on diminue forfaitairement toutes les proportions d'inaptes de 6 points )
    if (k_val_q_ho != k_median_ho && k_val_q_fe != k_median_fe && !options->NoInaptLiqTauxPlein)
    {
        // Modif 2015/11/20 : rapprochement taux d'inaptes hommes / femmes
        /*static vector<double> inaptes_ho = {1910,0.24 , 1916,0.34 , 1925,0.07};
        static vector<double> inaptes_fe = {1910,0.09 , 1916,0.24 , 1928,0.14};*/
        static vector<double> inaptes_ho = {1910,0.24 , 1916,0.34 , 1925,0.07};
        static vector<double> inaptes_fe = {1910,0.09 , 1916,0.24 , 1928,0.14, 1943,0.14, 1953,0.07};
        
        double liminaptho = LogLogist(k_def_q_ho, k_val_q_ho  ,0.50, k_median_ho, (1-affn(X.anaiss,inaptes_ho))  );
        double liminaptfe = LogLogist(k_def_q_fe, k_val_q_fe  ,0.50, k_median_fe, (1-affn(X.anaiss,inaptes_fe))  );

        double k_inapte = options->inapte_exo ? 1.6 : X.k;
        
        if ( X.sexe == HOMME && k_inapte > liminaptho ) {
            durdecote_rg = 0;
            type_liq = liq_inapte;
        }
        else if( X.sexe == FEMME && k_inapte > liminaptfe) {
            durdecote_rg = 0;
            type_liq = liq_inapte;
        }
    }
    
    // Taux de liquidation à la FP
    if (!X.retr->primoliq)
    {
        if (arr_mois(agetest) < arr_mois(l.AgeMinFP))   
            tauxliq_fp = 0; 
        else  
            tauxliq_fp = 1 - l.DecoteFP * durdecote_fp + l.SurcoteFP * dursurcote_fp;
			//if(X.Id==53074) {Rcout << " X.Id " << X.Id <<" agetest " << agetest << " l.SurcoteFP " << l.SurcoteFP <<endl;} 
    }

    // Taux de liquidation au RG
    tauxliq_rg = (l.TauxPleinRG-l.DecoteRG * durdecote_rg
                      + l.SurcoteRG1               * max(dursurcote_rg  , 0.0)
                      +(l.SurcoteRG2-l.SurcoteRG1) * max(dursurcote_rg-1, 0.0)
                      +(l.SurcoteRG3-l.SurcoteRG2) * max(dursurcote_rg-2, 0.0))/l.TauxPleinRG;
	//if(X.Id==1434) {Rcout << " X.Id " << X.Id <<" agetest " << agetest << " durdecote_rg " << durdecote_rg <<endl;}
    // Taux de liquidation (coefficient d'abattement) 
    // dans les régimes complémentaires ARRCO et AGIRC
    static const vector<double> coeff_abatt55 = {59,0, 60,0.75, 65,1.00, 70,1.25};
    static const vector<double> coeff_abatt64 = {59,0, 60,0.78, 62,0.88, 65,1.00};
    static const vector<double> coeff_abatt83 = {49,0, 60,0.78, 62,0.88, 65,1.00};
  
    if      (t<55) tauxliq_ar = affn(int_mois(agetest),coeff_abatt55);
    else if (t<64) tauxliq_ar = affn(int_mois(agetest),coeff_abatt64);
    else if (t<83) tauxliq_ar = affn(int_mois(agetest),coeff_abatt83);
    else
    {
        if    (agetest<l.AgeSurcote-5)    tauxliq_ar=0.00;
        else if (agetest<l.AgeSurcote)    tauxliq_ar=0.78  -0.07*ArrTrimSup(l.AgeSurcote-agetest);
        else if (agetest<l.AgeMaxRG)      tauxliq_ar=1.00  -0.04*max(durdecote_rg  ,0.0)
                                                           -0.01*max(durdecote_rg-3,0.0);
        else                              tauxliq_ar=1.00;
    };
    // neutralisation de la décote s'il y a taux plein au RG 
    // (cas des liquidations pour inaptitude et des retraites anticipées pour carrière longue)
    if (agetest>=l.AgeMinRG && tauxliq_rg>=1)   
        tauxliq_ar = 1;
        

}



int DroitsRetr::AgeMin()   //fonction refondue le 11/04/2013
{
    
    double agedeb = 9999;
    
    dar=false;   //par défaut
    
    // calcul des durées nécessaires aux conditions
    durees_base();    // TODO: Utile ?
    durees_majo();    // TODO: Utile ?
    

    // ... sinon test de l'éligibilité au dispositif de retraite anticipée 
    // pour carrières longues défini par la réforme de 2003
    if (l.LegDRA>=2003 && t>=104 && agetest<l.AgeSurcote && !options->NoRetrAntCarrLongues)
    {
        // détermination de l'âge de début d'activité
        int a   = 14;
        int dur = 0;
        while (agedeb == 9999) ////TODOTODO
        {
            static vector<int> OCC = {S_NC, S_CAD, S_NONTIT, S_IND, S_FPAE, S_FPATH, S_FPSE, S_FPSTH};
            dur += in(X.statuts[a], Statuts_occ);
            if (dur>=1 || a>=30)   agedeb = a;
            a++;
        }
        
        // test des conditions de départ à la retraite anticipée pour carrière longue
		//if(X.Id==496) {Rcout <<" Id " << X.Id<< " max(l.AgeMinRG,l.AgeMinFP) " << max(l.AgeMinRG,l.AgeMinFP)<<endl;}
        int j = 0;
        while (j < 5 && l.AgeDRA[j] && l.AgeDRA[j]<99)
        {//if(X.Id==496 && agetest>=60) {Rcout <<" Id " << X.Id<< " agetest " << agetest<< " l.AgeDRA[j] " << l.AgeDRA[j] << " dureecotdra_tot " << dureecotdra_tot <<
		//" l.DureeValCibDRA " << l.DureeValCibDRA <<endl;}
            if (     (agedeb+1            <= l.DebActCibDRA[j]) // Modif YD 20160212 "<=" -> "<"   
                  && (dureecotdra_tot >= l.DureeCotCibDRA[j])
                  && (duree_tot_maj   >= l.DureeValCibDRA[j])
                  && (l.AgeDRA[j]       <= agetest)
                  && (arr(12*agetest)   <  arr(12*max(l.AgeMinRG,l.AgeMinFP))))
            {
                dar  =  true;
                l.AgeMinRG = min(l.AgeMinRG, agetest); // TODO: a changer 
                l.AgeMinFP = min(l.AgeMinFP, agetest);
				//if(X.Id==496) {Rcout <<" Id " << X.Id<< " AgeMinRG from AgeMin() " << l.AgeMinRG<<endl;}
            }
            j++;
            
        }  // fin de la boucle sur j ("scénario" de départ anticipé)
    }  // fin de la boucle sur LegDRA
    
    //on teste ici la condition de taux plein : 
    // cela permet de ne plus avoir à la retester ensuite, 
    // et donc d'optimiser le temps de calcul
    
    if (arr_mois(agetest) >= arr_mois(min(l.AgeMinRG, l.AgeMinFP)))
        DecoteSurcote();
  
    return ( arr_mois(agetest) >= arr_mois(min(l.AgeMinRG, l.AgeMinFP)) );
}

/* Fonction AgeMax */
double DroitsRetr::AgeMax()
{
    // necessite duree_fp , duree_fpa , tauxliq_fp , agetest
    static vector<int> statuts_FPA = {S_FPAE,S_FPATH};
    static vector<int> statuts_sal = {S_NC, S_CAD, S_NONTIT};
   
    if  ( in(X.statuts[age], statuts_FPA) ) {
      
      if (duree_fpa>=l.DureeMinFPA  && tauxliq_fp>=1)
          return l.AgeMaxFP;
      else if ( duree_fp>=l.DureeMinFP  && tauxliq_fp>=1 && agetest>=l.AgeSurcote )
          return l.AgeMaxFP;
      else
          return min(l.AgeMaxFP+5,l.AgeSurcote+5);
    }
      
    if ( in(X.statuts[age], Statuts_FP) ) 
        return l.AgeMaxFP;
        
    if ( in(X.statuts[age], statuts_sal) )       
        return l.AgeMaxRG;
                                                              
    return 70;
}
  
  
  
  //Calcul du salaire annuel moyen à partir des salaires versés
  //aux comptes
  double DroitsRetr::calc_sam(double* spc_begin, double* spc_end) {
      double sum=0;
      int cpt = (spc_end-spc_begin);
  
      while(spc_begin < spc_end)
          sum += *(spc_begin++);
          
      return cpt  ?  sum / cpt  :  0;
  }

  
  //Calcul du salaire annuel moyen à partir des salaires versés
  //aux comptes des n meilleurs années
  double DroitsRetr::calc_sam(double* spc_begin, double* spc_end, int duree_calc) {
      double* nth_elt = max(spc_begin,spc_end-duree_calc);
      int cpt= (spc_end - nth_elt);
      double sum=0;
      
      if(!cpt) return 0;
      
      nth_element(spc_begin, nth_elt, spc_end); // partition
      
      while(nth_elt < spc_end)
          sum += *(nth_elt++);
  
      return sum / cpt;
  }
  
  
  
  // Fonction SalBase
  // Calcule les salaires servant de base au calcul des pensions de base, ie. les
  //  SAM pour le RG et les régimes d'indépendants et le salaire de référence FP.
  //  Les résultats sont stockés dans sam_rg, sam_in et sr_fp.
     void DroitsRetr::SalBase(int AnneeRefAnticip) {
      static const vector<double> dureeCalcSAM_in1993 = {1933,10,1943,15,1953,25};
        
      int date = int_mois(X.anaiss + agetest,X.moisnaiss + 1) % 1900;
      int t = date;
  
      double spc_rg[100];        int spc_rg_cpt=0;
      double spc_in[100];        int spc_in_cpt=0;
      double spc_fp[100];        int spc_fp_cpt=0;
      double spc_uni[100];       int spc_uni_cpt=0;
      double spc_rgin[100];      int spc_rgin_cpt=0; 
  
      double duree_calc_sam_rg = l.DureeCalcSAM;
      double duree_calc_sam_in = l.DureeCalcSAM;
      if(t >= 93 && l.an_leg >= 1993){
          duree_calc_sam_in = affn(X.anaiss,dureeCalcSAM_in1993);
      }
  
      if(t >= 104 && l.an_leg >= 2004 && duree_rg_maj && duree_in_maj) {
          duree_calc_sam_rg *= max(1,arr(duree_rg_maj/duree_tot_maj)); 
          duree_calc_sam_in *= max(1,arr(duree_in_maj/duree_tot_maj));
      }
  
      int u = age;
      if(!options->FPCarTot) {
          while(!in(X.statuts[u], {311,321,331,312,322,332}) && u >= 15)   u--;		// Modif 22/06/2017 : on ne retient pas les années passées en invalidité dans la FP pour déterminer le dernier salaire FP
          // TODO: Annee ref anticipée
          sr_fp = X.salaires[u] * M->PointFP[date] / M->PointFP[max(date-age+u, 30)];
      }
      else {
          int revalcum = 1;
          int a_min = 14;
          int a_max = age - 1;
          for(int a=a_max; a >= a_min; a--) {
              int t = (X.anaiss + a ) % 1900;
              if( in(X.statuts[a], Statuts_FP )) {
                 spc_fp[spc_fp_cpt++] = min(X.salaires[a], M->PlafondSS[t]) * revalcum;
                 revalcum *= M->RevaloSPC[t]; // TODO: Annee ref anticipée
              }
          }
  
          t = (X.anaiss + age) % 1900;
  
          if (options->SpcToutesAnnees || t < 72)
              sr_fp = calc_sam(spc_fp, spc_fp + spc_fp_cpt);
          else
              sr_fp = calc_sam(spc_fp, spc_fp + spc_fp_cpt, l.DureeCalcSAM);
          
      }
      // Prise en compte du salaire hors primes
      sr_fp /= (1+X.taux_prim);
  
      t = date;
  
      // Si l'individu n'a pas atteint l'âge de liquider au RG, on s'arrête là, sinon on calcule les SAM et RAM au RG et pour les indépendants
  
      if (agetest < l.AgeMinRG) return;
  
      // Pour les carrières du privé, calcul des séquences de salaires revalorisés
      // et portés au compte (on exclut les années à salaire nul, et on range les
      //  valeurs par ancienneté croissante)
  
      double revalcum = (date <= AnneeRefAnticip) ? M->RevaloSPC[date] : M->Prix[date] / M->Prix[date-1];
  
      if (!options->SAMRgInUnique && !options->SAMUnique && (l.LegSAM < 2013 || date < 117 || options->SAMSepare)) {
          spc_rg_cpt = 0;
          spc_in_cpt = 0;
          for(int a=age-1; a >= 14 ; a--) {
              int t = X.anaiss%1900 + a;
              if(in(X.statuts[a], Statuts_SAL)) {
                  spc_rg[spc_rg_cpt++] = min(X.salaires[a], M->PlafondSS[t]) * revalcum;
              }
              else if(X.retr->primoliq && duree_fp ==0 && in(X.statuts[a],Statuts_FPAS) ) {
                  spc_rg[spc_rg_cpt++] = min(X.salaires[a], M->PlafondSS[t]) * revalcum;
              }
              else if(X.statuts[a] == S_IND) {
                  spc_in[spc_in_cpt++] = min(X.salaires[a], M->PlafondSS[t]) * revalcum;
              }
              else if(X.statuts[a] == S_AVPF && !options->NoAVPF && !options->NoSpcAVPF) {
                  spc_rg[spc_rg_cpt++] = min(M->SMIC[t-1], M->PlafondSS[t]) * revalcum; //SMIC de l'année précédente
              }
              revalcum *= (t<=AnneeRefAnticip) ? M->RevaloSPC[t] : M->Prix[t] / M->Prix[t-1];
			  //if(X.Id==9) {Rcout << " t " << t << " revalcum " << revalcum<<endl;}
          }
  
          if(options->SpcToutesAnnees || t < 72) {
              sam_rg = calc_sam(spc_rg, spc_rg + spc_rg_cpt);
              sam_in = calc_sam(spc_in, spc_in + spc_in_cpt);
          }
          else {
              sam_rg = calc_sam(spc_rg, spc_rg + spc_rg_cpt, duree_calc_sam_rg);
              sam_in = calc_sam(spc_in, spc_in + spc_in_cpt, duree_calc_sam_in);
          }
      }
      else if(options->SAMRgInUnique || ( l.LegSAM>=2013 && date>=117 && !options->SAMSepare )) {
          spc_rgin_cpt = 0;
          for(int a=age-1; a >= 14 ; a--) {
              int t = (X.anaiss + a) % 1900;
              
              if( in(X.statuts[a], Statuts_RGIN) ) {
                  spc_rgin[spc_rgin_cpt++] = min(X.salaires[a], M->PlafondSS[t]) * revalcum;
              }
              else if( !X.retr->primoliq && duree_fp == 0 && in(X.statuts[a], Statuts_FPAS ) ) {
                  spc_rgin[spc_rgin_cpt++] = min(X.salaires[a], M->PlafondSS[t]) * revalcum;
              }
              else if( X.statuts[a] == S_AVPF && !options->NoAVPF && !options->NoSpcAVPF) {
                  spc_rgin[spc_rgin_cpt++] = M->SMIC[t-1] * revalcum;//SMIC de l'année précédente
              }
              revalcum *= (t<=AnneeRefAnticip) ? M->RevaloSPC[t] : M->Prix[t] / M->Prix[t-1];
          }
  
          if(options->SpcToutesAnnees || t < 72)
              sam_rgin = calc_sam(spc_rgin, spc_rgin + spc_rgin_cpt, spc_rgin_cpt );
          else
              sam_rgin = calc_sam(spc_rgin, spc_rgin + spc_rgin_cpt, duree_calc_sam_rg);
      }
      else if(options->SAMUnique) {
          spc_uni_cpt = 0;
          for(int a=age-1; a >= 14 ; a--) {
              int t = (X.anaiss + a) % 1900;
              if( in(X.statuts[a], Statuts_RGIN)  ) {
                  spc_uni[spc_uni_cpt++] = min(X.salaires[a], M->PlafondSS[t]) * revalcum;
              }
              else if( X.retr->primoliq && duree_fp == 0 && in(X.statuts[a], Statuts_FPAS ) ) {
                  spc_uni[spc_uni_cpt++] = min(X.salaires[a], M->PlafondSS[t]) * revalcum;
              }
              else if( in(X.statuts[a], Statuts_FPAS ) ) {
                  spc_uni[spc_uni_cpt++] = min(X.salaires[a], M->PlafondSS[t]) * revalcum;
              }
              else if(X.statuts[a] == S_AVPF && !options->NoAVPF && !options->NoSpcAVPF ) {
                  spc_uni[spc_uni_cpt++] = M->SMIC[t-1] * revalcum;
              }
  
              revalcum *= (t<=AnneeRefAnticip) ? M->RevaloSPC[t] : M->Prix[t] / M->Prix[t-1];
          }
  
          if(options->SpcToutesAnnees || t < 72)
              sam_uni = calc_sam(spc_uni, spc_uni + spc_uni_cpt);
          else
              sam_uni = calc_sam(spc_uni, spc_uni + spc_uni_cpt, duree_calc_sam_rg);
      }
  }

/** 
 * \struct Retraite_comp
 * \brief Contient les éléments permettant de calculer les retraites complémentaires
 */  
struct Retraite_comp {
    double points_arrco=0;            ///< Points au régime Arrco
    double points_agirc=0;            ///< Points au régime Agirc
	  double maj_points_arrco=0;        ///< Nombre de points Arrco acquis pour majoration pour enfants
	  double maj_points_agirc=0;        ///< Nombre de points Agirc acquis pour majoration pour enfants
    double points_FP=0;               ///< Points Arrco "fictifs" calculés si le fonctionnaire n'atteint pas la durée minimale pour toucher une pension FP
    double assietteCad=0;             ///< Assiette de cotisations pour l'Agirc
    double assietteNC=0;              ///< Assiette de cotisations pour l'Arcco
    double assietteFP=0;              ///< Assiette de cotisations pour la Fonction publique
    int durpergratuit=0;              ///< Compteur de durée de périodes "gratuites" avec ouverture de droits sans cotisations
	  double points_agirc_arrco =0;     ///< Points au régime unifié complémentaire du privé
	  double maj_points_agirc_arrco=0;  ///< Nombre de points au régime unifié complémentaire du privé acquis pour majoration pour enfants
  } ;
  
  /**
    \fn Points

    Calcule les nombres de points accumulés dans les régimes ARRCO et AGIRC et stocke les résultats dans
    les variables points_arrco, points_agirc.
  */
  void DroitsRetr::Points(int AnneeRefAnticip) { // AnneeRefAnticip == Année de référence anticipée. Utilisée dans TestLiq afin de prendre en compte l'anticipation de l'évolution du salaire de référence.
      static const vector<int> Codes_sal   = {S_CAD, S_NC, S_NONTIT, S_FPAE, S_FPATH, S_FPSE, S_FPSTH};
      static const vector<int> Codes_salNC = {S_NC, S_NONTIT};
      static const vector<int> PRPRRG = {S_PR};
      double TauxARRCO_1, TauxARRCO_2, TauxAGIRC_B, TauxAGIRC_C;
      double salrefarrco;
      double salrefagirc;
	  double salrefagirc_arrco;
      
      if(agetest < l.AgeMinRG) 
          return;
  
      double agefin_finact = X.retr->primoliq ? min(X.retr->primoliq->agetest, agetest) : agetest;
      int    age_arr       = int_mois(agefin_finact, X.moisnaiss+1);
  
  
      int age_deb = max(14, 1948- X.anaiss); // pas d'acquisition de points dans les complémentaires avant 1957
      int age_max = age_arr;
      int u;
      double trunc;
      Retraite_comp rcp = {};
      
      // On recalcule tous les points déjà acquis
      for(int a = age_deb; a <= age_max; a++) {
          Retraite_comp rc = {};
          u = (X.anaiss + a) % 1900;
          trunc = (a == age_max) ? arr_mois(agefin_finact, X.moisnaiss+1) - age_arr : 1;
          
          double plafSS = M->PlafondSS[u];
                          
          TauxARRCO_1 = u >=58 ? M->TauxARRCO_1[u] : 0; // L'Unirs, ancêtre de l'Arrco, a été créée en 1957.
          TauxARRCO_2 = u >=58 ? M->TauxARRCO_2[u] : 0;
          TauxAGIRC_B = u >=48 ? M->TauxAGIRC_B[u] : 0;
          TauxAGIRC_C = u >=48 ? M->TauxAGIRC_C[u] : 0;
  
          if( in(X.statuts[a], Codes_sal) ) {
              rc.assietteCad = X.statuts[a] == S_CAD           ? X.salaires[a] : 0;
              rc.assietteNC  = in(X.statuts[a], Codes_salNC) ? X.salaires[a] : 0;
              rc.assietteFP  = in(X.statuts[a], Statuts_FP)    ? X.salaires[a] : 0;
              rc.durpergratuit = 0;
          }
          else if(!options->NoPtsGratuits) { // à adapter au régime unifié AGIRC-ARRCO
              if(in(X.statuts[a], Statuts_cho) && (u >= 68) && ((u>=81 &&(((a<50) & (rcp.durpergratuit<=1)) || ((a>=50) & (rcp.durpergratuit<=2)))) || ((u<81) && rcp.durpergratuit==0))) { 
			  //  on a codé les durées maximales de chomage indemnisé qui seules donnent droit à des pts gratuits 09/11/2016
                  rc.durpergratuit = rcp.durpergratuit + 1;
                  if((u < 97) && (rcp.assietteNC + rcp.assietteCad + rcp.assietteFP > 0)) { // on reprend ici "a peu près" les points obtenus l'année précédente
                      rc.assietteCad = rcp.assietteCad;
                      rc.assietteNC  = rcp.assietteNC;
                      rc.assietteFP  = 0;
                      TauxAGIRC_C = M->TauxAGIRC_C[u];
                  }
                  else if((u >= 97) && (rcp.assietteNC + rcp.assietteCad + rcp.assietteFP > 0)) { //le calcul se fait dorenavant a partir du SRJ calculé par Pole emploi sans la tranche C
                      rc.assietteCad = rcp.assietteCad;
                      rc.assietteNC  = rcp.assietteNC;
                      rc.assietteFP  = 0; // pas de points gratuits quand le chômage suit une période d'emploi de fonctionnaire
                      TauxAGIRC_C = 0;
                  }
              }
              else if(in(X.statuts[a], Statuts_PRPRRG) && u >= 68 && (rcp.durpergratuit <= 1 || a >= 50)){ // A relire.
                  rc.durpergratuit = rcp.durpergratuit + 1;
                  if((u < 96) && (rcp.points_arrco || rcp.points_agirc || rcp.points_FP)) {
                      rc.assietteCad = 0;
                      rc.assietteNC  = 0;
                      rc.assietteFP  = 0;
                  }
                  else if((u >= 96) && (rcp.points_arrco || rcp.points_agirc || rcp.points_FP)) {
                      rc.assietteCad = rcp.assietteCad * M->Prix[u] / M->Prix[u-1] ;
                      rc.assietteNC  = rcp.assietteNC * M->Prix[u] / M->Prix[u-1] ;
                      rc.assietteFP  = 0; // pas de points gratuits quand le chômage suit une période d'emploi de fonctionnaire
                      
                      if(u+1-rc.durpergratuit >= 97)
                          rc.assietteCad = min(rc.assietteCad, 2*plafSS);
                          
                      TauxARRCO_1 = (u+1-rc.durpergratuit<97)  ?      M->TauxARRCO_1[u]             :
                                    (u+1-rc.durpergratuit<100) ?      min(0.06,M->TauxARRCO_1[u])   :
                                                                      0.04;
                      TauxARRCO_2 = (u+1-rc.durpergratuit<100) ?      M->TauxARRCO_2[u]             :
                                                                      0.04;
                      TauxAGIRC_B = TauxAGIRC_B;
                      TauxAGIRC_C = 0;
                  }
              }
          }
  
		if(u<119 || l.an_leg <2015 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco) {
          if (u<=AnneeRefAnticip){
              salrefarrco = M->SalRefARRCO[u];
              salrefagirc = M->SalRefAGIRC[u];
          }
          else{
              salrefarrco = M->SalRefARRCO[AnneeRefAnticip]*(M->Prix[u]/M->Prix[AnneeRefAnticip]);
              salrefagirc = M->SalRefAGIRC[AnneeRefAnticip]*(M->Prix[u]/M->Prix[AnneeRefAnticip]);
          }
  
          if(rc.assietteCad && salrefarrco) {
              rc.points_arrco = part(rc.assietteCad, 0.0, plafSS) * TauxARRCO_1 / salrefarrco;
		  }
  
          if(rc.assietteCad && salrefagirc) {
              rc.points_agirc = 
                       part(rc.assietteCad,   plafSS, 4*plafSS) * TauxAGIRC_B/salrefagirc
                     + part(rc.assietteCad, 4*plafSS, 8*plafSS) * TauxAGIRC_C/salrefagirc;
              if(!options->NoPtsGratuits)
                  rc.points_agirc = max(M->GMP[u], rc.points_agirc);
          }
  
          if(rc.assietteNC && salrefarrco) {
              rc.points_arrco = 
                       part(rc.assietteNC,      0,   plafSS) * TauxARRCO_1 / salrefarrco
                     + part(rc.assietteNC, plafSS, 3*plafSS) * TauxARRCO_2 / salrefarrco;
          }
  
          if(rc.assietteFP && salrefarrco)
              rc.points_FP = 
                       part(rc.assietteFP,      0,    plafSS) * TauxARRCO_1 / salrefarrco
                     + part(rc.assietteFP, plafSS,  3*plafSS) * TauxARRCO_2 / salrefarrco;
		
		}
		
		else {
		  if (u<=AnneeRefAnticip){
              salrefagirc_arrco = M->SalRefAGIRC_ARRCO[u];
          }
          else{
              salrefagirc_arrco = M->SalRefAGIRC_ARRCO[AnneeRefAnticip]*(M->Prix[u]/M->Prix[AnneeRefAnticip]);
          }
		  
		  if((rc.assietteCad || rc.assietteNC) && salrefagirc_arrco) {
				rc.points_agirc_arrco =  part(rc.assietteCad + rc.assietteNC,      0,   plafSS) * M->TauxAGIRC_ARRCO_1[u] / salrefagirc_arrco
                     + part(rc.assietteCad + rc.assietteNC, plafSS, 8*plafSS) * M->TauxAGIRC_ARRCO_2[u] / salrefagirc_arrco;
					 }
		  
		  // Quid de la fonction publique ?
		}
  
          if(rc.points_arrco + rc.points_agirc + rc.points_FP + rc.points_agirc_arrco > 0) {
              rcp.assietteCad  =  rc.assietteCad;
              rcp.assietteNC   =  rc.assietteNC;
              rcp.assietteFP   =  rc.assietteFP;
              rcp.points_arrco += rc.points_arrco * trunc;
              rcp.points_agirc += rc.points_agirc * trunc;
              rcp.points_FP    += rc.points_FP    * trunc;
			  rcp.points_agirc_arrco += rc.points_agirc_arrco * trunc;
			  if ((l.an_leg>=2011) && (u>=112)) {
				  rcp.maj_points_arrco+= rc.points_arrco * trunc*0.1;
				  rcp.maj_points_agirc+= rc.points_agirc * trunc*0.1;
				  rcp.maj_points_agirc_arrco+=rc.points_agirc_arrco*trunc*0.1;}
			  else {
				  rcp.maj_points_arrco+= rc.points_arrco * trunc*0.05;
				  rcp.maj_points_agirc+= rc.points_agirc * trunc*min(0.24,(0.08+0.04*(X.nb_enf(age_max)-3)));
			  }
			  if(!rc.durpergratuit) {rcp.durpergratuit = 0;}
		      else {rcp.durpergratuit++;}
          }
		  else {
		  rcp.durpergratuit=0;
		  rcp.assietteCad  =0;
              rcp.assietteNC   =0;
              rcp.assietteFP   =0;}
		  
          /*else if(rc.durpergratuit){
              rcp.assietteCad  *= M->Prix[t] / M->Prix[t-1];
              rcp.assietteNC   *= M->Prix[t] / M->Prix[t-1];
              rcp.assietteFP   *= M->Prix[t] / M->Prix[t-1];
              rcp.durpergratuit++;
          }
          else{
              // pas de points gratuits quand le chômage suit une période d'emploi de fonctionnaire
              rcp.assietteCad  =0;
              rcp.assietteNC   =0;
              rcp.assietteFP   =0;
              rcp.durpergratuit=0;
          }*/
      }
      
	  
		  points_arrco = rcp.points_arrco;
	      points_agirc = rcp.points_agirc;
	      ntp_FP    = rcp.points_FP;
		  maj_points_arrco = rcp.maj_points_arrco;
		  maj_points_agirc = rcp.maj_points_agirc;
		  maj_points_agirc_arrco= rcp.maj_points_agirc_arrco;
	  
	     points_agirc_arrco = rcp.points_agirc_arrco;
  }
  
  
  /** Fonction AppliqueBonif

    Cette fonction applique les majorations de pensions pour enfants 
    à l'ensemble des pensions, de droit direct ou dérivé.
    
    Comme cette fonction n'est appelée que juste après le calcul des avantages principaux 
    de droit direct OU dérivé, on utilise les variables
    $agefin_primoliq, $agefin_totliq et $agerevliq pour savoir à quelles pensions la bonification 
    doit être appliquée (et, \textit{a contrario},
     auxquelles elle l'a déjà été au cours d'étapes précédentes).
     
    Remarque : on utilise notamment le fait que, pour les droits directs, 
    la fonction est appelée APRES le calcul du montant de pension (hors bonification)
    mais AVANT la définition des variables d'âge fin.
    
    Remarque : ne sont pas encore programmées ici :
    - la modification du taux de bonification à l'Agirc et à l'Arrco pour les points acquis après 2012
    - la proratisation de la limite de 1000 euros par an pour la bonification Agirc et Arrco après 2012
   */
  void DroitsRetr::AppliqueBonif()
  {
     if(X.nb_enf(age) < 3)
        return;
    
     double tauxmajo_rg;
     double tauxmajo_fp;
     double tauxmajo_in;
  
     //calcul des majorations
     tauxmajo_fp = (0.1+0.05*(X.nb_enf(age)-3));
     if (!X.retr->primoliq && pension_fp)
          tauxmajo_fp = max(0.0,min( (0.1+0.05*(X.nb_enf(age)-3)),(sr_fp/pension_fp-1)));
		  
     tauxmajo_rg = 0.1;
     
     
     tauxmajo_in = 0.1;
  
     // modification des valeurs des variables, selon les cas
     
     // primoliquidation des droits directs (à la FP)
     if (!X.retr->primoliq && pension_fp)   
     {
        majo_3enf_fp += tauxmajo_fp*pension_fp;
        pension_fp   *= (1+tauxmajo_fp);
        majo_min_fp  *= (1+tauxmajo_fp);
     }
     
     //liquidation des droits directs dans les régimes du privé
     if (!X.retr->totliq && (pension_rg + pension_ar + pension_ag + pension_in > 0) )  
     {
        majo_3enf_rg += tauxmajo_rg * pension_rg;
        pension_rg   *= (1+tauxmajo_rg);
        majo_min_rg  *= (1+tauxmajo_rg);
  
        if ((l.an_leg>=2011) && (t>=112)){
			majo_3enf_ar += min(  maj_points_arrco * M->ValPtARRCO[t],1000* M->ValPtARRCO[t]/ M->ValPtARRCO[111]);} 
			// le regime arrco ne tient pas compte du corfficient de minoration pour la majoration 3 enfants
		else  majo_3enf_ar += maj_points_arrco *  M->ValPtARRCO[t];	//pas de majoration avant 2012
        pension_ar   += majo_3enf_ar;
  
		if ((l.an_leg>=2011) && (t>=112)){
		majo_3enf_ag += min( tauxliq_ar * maj_points_agirc *  M->ValPtAGIRC[t],1000*  M->ValPtAGIRC[t]/  M->ValPtAGIRC[111]);}
		else  majo_3enf_ag +=tauxliq_ar * maj_points_agirc *  M->ValPtAGIRC[t];	//pas de majoration avant 2012
        pension_ag   += majo_3enf_ag;
		// on suppose ici que le plafond a été doublé mais sans info à ce sujet à cette date
		if ((l.an_leg>=2015) && (t>=119) && !options->NoAccordAgircArrco && !options->NoRegUniqAgircArrco){
		majo_3enf_ag_ar += min( tauxliq_ar * maj_points_agirc_arrco *  M->ValPtAGIRC_ARRCO[t],2000* M->ValPtARRCO[119]/ M->ValPtARRCO[111]*  M->ValPtAGIRC_ARRCO[t]/  M->ValPtAGIRC_ARRCO[119]);}
        pension_ag_ar   += majo_3enf_ag_ar;
		
        majo_3enf_in += tauxmajo_in * pension_in;
        pension_in   *= (1+tauxmajo_in);
        majo_min_in  *= (1+tauxmajo_in);
     }
     
     // pour la réversion (A REVOIR : ce serait sans doute mieux de traiter la réversion de manière spécifique ...)
  }
  
  
  
  /**
   * Fonction Mincont

    Calcul du montant attribuable au titre du minimum contributif. Suppose que les
    durées de cotisation sont connues, donc des appels préalables de DurBase et
    DurMajo (source, Doc N° 4, séance COR du 27 juin 2006)
   */
  void DroitsRetr::MinCont(int AnneeRefAnticip)
  {
      if(t < 83 || tauxliq_rg<1)   
          return;
  
      double mico1 = (t <= AnneeRefAnticip) ?  M->Mincont1[t]   :  M->Mincont1[AnneeRefAnticip] * M->Prix[t] / M->Prix[AnneeRefAnticip];
      double mico2 = (t <= AnneeRefAnticip) ?  M->Mincont2[t]   :  M->Mincont2[AnneeRefAnticip] * M->Prix[t] / M->Prix[AnneeRefAnticip];
      double majo  = mico2 - mico1;
      
      double taux_prorat_deplaf = 1+l.DeplafProrat / l.DureeProratRG;
      double taux_prorat_rg     = duree_rg_maj / l.DureeProratRG;
      double taux_prorat_in     = duree_in_maj / l.DureeProratRG;
      double taux_prorat_cotmin = dureecotmin_tot/l.DureeProratRG;
      double taux_tot_rg        = duree_rg_maj/duree_tot_maj;
      double taux_tot_in        = duree_in_maj/duree_tot_maj;    
  
      if( t > 83 && min(t, l.LegMin%1900) < 104) {
          min_cont    = mico1 * min(taux_prorat_deplaf, taux_prorat_rg);
          min_cont_in = mico1 * min(taux_prorat_deplaf, taux_prorat_in);
          // RQ : pour l'instant, on ne modélise pas la règle de non-cumul des minimum entre 1985 et 2004 -> à revoir dans AppliqueMin!
      }
      else if( t >= 104 && (datetest < 2005.5 || l.LegMin <= 2004)) {
          if(duree_tot_maj < l.DureeCibRG) {
              min_cont    = mico1 * min(taux_prorat_deplaf, taux_prorat_rg);
              min_cont_in = mico1 * min(taux_prorat_deplaf, taux_prorat_in);
          }
          else if(taux_tot_rg ==1 || taux_tot_in == 1 ||  duree_tot_maj <= 40) {
              min_cont    = mico2 * min(taux_prorat_deplaf, taux_prorat_rg);
              min_cont_in = mico2 * min(taux_prorat_deplaf, taux_prorat_in);
          }
          else {
              min_cont    = mico2 * min(taux_prorat_deplaf, taux_tot_rg);
              min_cont_in = mico2 * min(taux_prorat_deplaf, taux_tot_in);
          }
  
      }
      else if (datetest >=2005.5 ) {
          if ( datetest >= (2009+4/12.0) && l.LegMin >= 2009 && dureecotmin_tot < 120/4.0 )
              majo = 0;
          if ( (duree_rg_maj+duree_in_maj) == 0 ) {
              min_cont    = 0;
              min_cont_in = 0;
          }
          else if( taux_tot_rg ==1 || taux_tot_in == 1 ||  duree_tot_maj <= l.DureeCibRG ) {
              min_cont    = (mico1 + majo * min(taux_prorat_deplaf, taux_prorat_cotmin)) * min(1.0, taux_prorat_rg) ;
              min_cont_in = (mico1 + majo * min(taux_prorat_deplaf, taux_prorat_cotmin)) * min(1.0, taux_prorat_in) ;
          }
          else if( t < 116 || l.an_leg < 2013 ) {
              min_cont    = (mico1 + majo * min(taux_prorat_deplaf, taux_prorat_cotmin)) * min(1.0, taux_tot_rg) ;
              min_cont_in = (mico1 + majo * min(taux_prorat_deplaf, taux_prorat_cotmin)) * min(1.0, taux_tot_in) ;
          }
          else {  // ajout le 06-09-2013 pour prendre en compte réforme de 2013
              min_cont    = (mico1 + majo * min(taux_prorat_deplaf, taux_prorat_cotmin))
                             * duree_rg_maj / (duree_rg_maj + duree_in_maj)
                             * min(1.0, taux_tot_rg);
              min_cont_in = (mico1 + majo * min(taux_prorat_deplaf, taux_prorat_cotmin))
                             * duree_in_maj / (duree_rg_maj + duree_in_maj)
                             * min(1.0, taux_tot_in);
          }
      }
  }
  
  /** Fonction MinGaranti 
   * Calcul du montant attribuable au titre du minimum garanti : période transitoire
      entre 2004 et 2013 avec application progressive de la réforme. Suppose que
      duree_fp ait été renseignée, i.e. un appel préalable de DurBase.
   */
  double DroitsRetr::MinGaranti()
  {
     double ds=duree_fps;
     double da=duree_fpa;
     double d=duree_fp_maj;
     double coef=0;
    
     if (ds+da < l.DureeMinFP)  
        return 0.0;       // modif 09/12/2011
     else if (min(t, l.LegMin%1900)<111)   { // modif 13/12/2011 : condition selon Min($t,$LegMin) et non selon $t
       if(l.LegMin<2003){coef=216*(60.0+4.00*part(d,15,25)                       );}
       else if (t<=103) {coef=216*(60.0+4.00*part(d,15,25)                       );}
       else if (t==104) {coef=217*(59.7+3.80*part(d,15,25.5)+0.04*part(d,25.5,40));}
       else if (t==105) {coef=218*(59.4+3.60*part(d,15,26.0)+0.08*part(d,26.0,40));}
       else if (t==106) {coef=219*(59.1+3.40*part(d,15,26.5)+0.13*part(d,26.5,40));}
       else if (t==107) {coef=220*(58.8+3.20*part(d,15,27.0)+0.21*part(d,27.0,40));}
       else if (t==108) {coef=221*(58.5+3.10*part(d,15,27.5)+0.22*part(d,27.5,40));}
       else if (t==109) {coef=222*(58.2+3.00*part(d,15,28.0)+0.23*part(d,28.0,40));}
       else if (t==110) {coef=223*(57.9+2.85*part(d,15,28.5)+0.31*part(d,28.5,40));}
       else if (t==111) {coef=224*(57.6+2.75*part(d,15,29.0)+0.35*part(d,29.0,40));}
       else if (t==112) {coef=225*(57.5+2.65*part(d,15,29.5)+0.38*part(d,29.5,40));}
       else if (t>=113) {coef=227*(57.5+2.50*part(d,15,30.0)+0.50*part(d,30.0,40));}
     }
     else if (min(t, l.LegMin%1900) >=111) //Réforme 2010 Modif 27/10/2010 Marion
     {
	   bool indic15An = d<15;
       //if((age<l.AgeMinMG) && (tauxliq_fp<1) ) {coef=0;}   // Rq : modif le 11/04/2013 : tauxliq_fp au lieu de tauxliq_rg (précédemment utilisé) (???)
	   if(((age<l.AgeMinMG) && (tauxliq_fp<1)) || ds+da<2.0) {coef=0;}
       else if (t==111) {coef=224*(100*indic15An*d/l.DureeCibFP + (1-indic15An)*(57.6+2.75*part(d,15,29.0)+0.35*part(d,29.0,40)));} // Modif 17/02/2017
       else if (t==112) {coef=225*(100*indic15An*d/l.DureeCibFP + (1-indic15An)*(57.5+2.65*part(d,15,29.5)+0.38*part(d,29.5,40)));}
       else if (t>=113) {coef=227*(100*indic15An*d/l.DureeCibFP + (1-indic15An)*(57.5+2.50*part(d,15,30.0)+0.50*part(d,30.0,40)));}
     }
     return coef*M->PointFP[104]*(M->Prix[t]/M->Prix[104])/100.0;
	 
  }
  
  
  

  // Cette fonction applique les dispositifs de minimum contributif et de minimum garanti 
  //à l'ensemble des pensions, le calcul des avantages principaux
  //de droits directs hors min ayant été déjà réalisés au préalable.*/

  void DroitsRetr::AppliqueMin()
  {
     double date = arr_mois(X.anaiss + agetest, X.moisnaiss+1);
     double plafecret;
     double majomax;
     min_garanti =0;
     min_cont_in =0;
     min_cont    =0;
  
     if (tauxliq_rg>=1)                              MinCont();
     else                                            min_cont   = min_cont_in = 0;
     
     if (!X.retr->primoliq  && duree_fp > 0)         min_garanti=MinGaranti();
     else                                            min_garanti=0;
  

     if (min_cont+min_cont_in+min_garanti==0) return;
  
     if ( (l.LegMin>=2009) && (date>=2009.25)  )    /// PLFSS 2009 : la surcote s'apprécie après le calcul du minimum contributif et garanti
     {
        // modification le 06-09-2013 : correction de la formule de calcul, car la formule précédente (du type : $min_cont *= Max(1,X.tauxliq_rg)) était érronée
        if (min_cont>0    && tauxliq_rg>1)  min_cont       += (tauxliq_rg-1)/tauxliq_rg*pension_rg;
        if (min_cont_in>0 && tauxliq_rg>1)  min_cont_in    += (tauxliq_rg-1)/tauxliq_rg*pension_in;
        if (min_garanti>0 && tauxliq_fp>1)  min_garanti    += (tauxliq_fp-1)/tauxliq_fp*pension_fp;
     }
  
     // calcul du plafond d'écrêtement : revaloration comme le SMIC à partir de la valeur prévue pour 2012
     //if ((l.LegMin<2009) || (t<112))        plafecret = 999999;
     //else if ((l.LegMin<2013) || (t<114))   plafecret = (12*1005)*sqrt(M->SMIC[t]/M->SMIC[112]);  //modification pour la réforme 2013
     //else                                   plafecret = (12*1120)*sqrt(M->SMIC[t]/M->SMIC[114]);  //modification pour la réforme 2013
  
    // Modification 30/10/2014 : plafond d'écrêtement : indexation sur les prix ou sur le SMIC
    if(options->plafecretMinContSMPT) {
      if ((l.LegMin<2009) || (t<112))        plafecret = 999999;
      else if ((l.LegMin<2013) || (t<114))   plafecret = (12*1005)*(M->SMIC[t]/M->SMIC[112]);  
      else                                   plafecret = (12*1120)*(M->SMIC[t]/M->SMIC[114]);  
      
    }
    else {
      if ((l.LegMin<2009) || (t<112))        plafecret = 999999;
      else if ((l.LegMin<2013) || (t<114))   plafecret = (12*1005)*(M->Prix[t]/M->Prix[112]);  
      else                                   plafecret = (12*1120)*(M->Prix[t]/M->Prix[114]); 
    }
  
  
     majomax         = max(0.0, (plafecret - pension_rg-pension_ar-pension_ag
                                           - pension_fp-pension_in));
  
     double diff_mincont    = max(0.0, (min_cont-pension_rg));
     double diff_mincont_in = max(0.0, (min_cont_in-pension_in));
     double diff_mingaranti = max(0.0, (min_garanti-pension_fp));
  
     double diff_tot        = diff_mincont + diff_mincont_in + diff_mingaranti;
  
     if ( l.LegMin>=2009 && t>=112 && diff_tot>majomax )    // PLFSS 2009 : application à partir de 2012 d'un écrêtement des minimum selon une condition de pension tous régimes
     {
        if (X.retr->primoliq) // si les droits FP ont déjà été liquidés -> l'écrêtement ne concerne que le MICO au RG et au RSI
        {
           min_cont    = pension_rg + max(0.0, min ( diff_mincont    , majomax*diff_mincont   /(diff_mincont+diff_mincont_in)));
           min_cont_in = pension_in + max(0.0, min ( diff_mincont_in , majomax*diff_mincont_in/(diff_mincont+diff_mincont_in)));
        }
        else if ( !X.retr->primoliq && (pension_fp>0) && (duree_rg+duree_in>0) && (pension_rg+pension_in+VFU_rg==0) )
        {
           min_garanti = 0;    // pas de minimum garanti si tous les droits de tous les régimes ne sont pas liquidé en même temps
        }
        else
        {
           min_garanti = pension_fp + max(0.0, min(diff_mingaranti , (diff_mingaranti/diff_tot*majomax) ));
           min_cont_in = pension_in + max(0.0, min(diff_mincont_in , (diff_mincont_in/diff_tot*majomax) ));
           min_cont    = pension_rg + max(0.0, min(diff_mincont    , (diff_mincont   /diff_tot*majomax) ));
        }
     }
    
  
    if (!options->NoMC && pension_rg > 0)
    {
      if (pension_rg<min_cont)      {    indic_mc=true;     }
      majo_min_rg= max(0.0,min_cont-pension_rg);
      pension_rg = max(pension_rg, min_cont);
    }
    
    if (!options->NoMC && pension_in > 0)
    {
      if (pension_in < min_cont_in)  indic_mc_in=true;
      majo_min_in= max(0.0, min_cont_in-pension_in);
      pension_in = max(pension_in, min_cont_in);
    }
    
    if (!options->NoMG && pension_fp > 0 && !X.retr->primoliq)
    {
      if (pension_fp < min_garanti)   indic_mg=true;
      majo_min_fp= max(0.0, min_garanti-pension_fp);
      pension_fp=max(pension_fp, min_garanti) ;
    }
  }
  

  
  void DroitsRetr::LiqPrive(int AnneeRefAnticip)
  {
     double prorat = 0;
  
     majo_min_rg = 0;
     majo_min_in = 0;
     majo_3enf_rg = 0; 
     majo_3enf_ar = 0; 
     majo_3enf_ag = 0; 
     majo_3enf_in = 0; 
  
     //######### Sous module régime général ########################################
     //# Calculs du coeff de proratisation et du taux
     if ((l.LegSAM<2013) || (t<117) || (options->SAMSepare))
     {
        prorat = duree_rg_maj/l.DureeProratRG;
        if (!options->NoBorneCoeffProrat)  
            prorat = min( (1+l.DeplafProrat/l.DureeProratRG), prorat );

     }
     else
     {
        prorat = (duree_rg_maj+duree_in_maj) / l.DureeProratRG;
        
        if (!options->NoBorneCoeffProrat)  
            prorat=min((1+l.DeplafProrat/l.DureeProratRG),prorat);     

        
        if (duree_rg_maj+duree_in_maj>0)
            prorat *= (duree_rg_maj/(duree_rg_maj+duree_in_maj));   
        else                                
            prorat = 0;     
     }
  
     double taux = (agetest >= l.AgeMinRG)?   tauxliq_rg * l.TauxPleinRG   :   0;
        //if  {taux=0;}
        //else
        //{
        //$taux=$TauxPleinRG-$DecoteRG    *$durdecote_rg[$i]
        //      + $SurcoteRG1             *Max($dursurcote_rg[$i],0)
        //      +($SurcoteRG2-$SurcoteRG1)*Max($dursurcote_rg[$i]-1,0)
        //      +($SurcoteRG3-$SurcoteRG2)*Max($dursurcote_rg[$i]-2,0);
        //$tauxliq_rg[$i] = $taux/$TauxPleinRG;
        //taux = tauxliq_rg * l.TauxPleinRG;
        //}
  
     //# options de sam unique tous régimes
     if (options->SAMUnique)
        sam_rg = sam_uni;       
     
     if (options->SAMRgInUnique)
        sam_rg = sam_rgin;  
    
     if ((l.LegSAM>=2013) && (t>=117) && (!options->SAMSepare))
        sam_rg = sam_rgin;  

     //# cas (très rare) de personnes sans SAM (exemple : personne ayant que de l'AVPF, avec l'option NoSpcAVPF)
     if (sam_rg == UNDEF || sam_rg==0)
        sam_rg = 2*M->Mincont1[t];
        // A REVOIR !!!

     // Calcul de la pension avec condition de plafonnement
     if (datetest < 1983.25)
     {
        pension_rg = min(   
          M->PlafondSS[t] * (l.TauxRGMax + l.MajTauxRGMax * max(0.0,dursurcote_rg) ),
          taux*prorat*sam_rg                 
          );
     }
     else if (arr_mois(X.anaiss+65,X.moisnaiss)<arr_mois(1983,3))
        pension_rg=min(   taux*M->PlafondSS[t],    taux*prorat*sam_rg  );
     else
        pension_rg=min(   taux*M->PlafondSS[t],    taux*prorat*sam_rg  ); // TODO: ????

  
     //versement en VFU si le montant annuel de la pension est trop faible
     if (pension_rg <  M->SeuilVFURG[t] && !options->NoVFU && (l.an_leg<2014 || t<116))
     {
        VFU_rg     = 15 * pension_rg;
        pension_rg = 0;
     }
     
     taux_prorat_rg = prorat;
  
  
     //######## Sous module ARRCO/AGIRC ###########################################
  
     // Calcul des deux pensions
		
     if (t<=AnneeRefAnticip)
	 {
		pension_ar = tauxliq_ar * points_arrco * M->ValPtARRCO[t];
        pension_ag = tauxliq_ar * points_agirc * M->ValPtAGIRC[t];
		pension_ag_ar = tauxliq_ar * points_agirc_arrco * M->ValPtAGIRC_ARRCO[t];
     }
     else
     {
        pension_ar = tauxliq_ar * points_arrco * M->ValPtARRCO[AnneeRefAnticip] * (M->Prix[t]/M->Prix[AnneeRefAnticip]);
        pension_ag = tauxliq_ar * points_agirc * M->ValPtAGIRC[AnneeRefAnticip] * (M->Prix[t]/M->Prix[AnneeRefAnticip]);
		pension_ag_ar = tauxliq_ar * points_agirc_arrco *  M->ValPtAGIRC_ARRCO[AnneeRefAnticip] * (M->Prix[t]/M->Prix[AnneeRefAnticip]);
     }
     
     // versement en VFU si le montant annuel de la pension est trop faible
     // (pas de série historique des seuils de VFU retrouvées => on conserve la valeur seuil actuelle (exprimée en nombre de points en non en euros !) sur tout le passé
     //approximation de la valeur véritable, qui est normalement défini par un tableau selon l'âge (en fonction de l'espérance de vie)
     static const vector<double> VFU_espvie = {55, 21, 99, 2.5};
     
	 // à faire : VFU dans le régime unifié
     if (tauxliq_ar * points_arrco < 100  && !options->NoVFU)
     {
        VFU_ar     = affn(age, VFU_espvie) * pension_ar;    
        pension_ar = 0;
     }
     if (tauxliq_ar * points_agirc < 500 && !options->NoVFU)
     {
        VFU_ag     = affn(age, VFU_espvie) * pension_ag;
        pension_ag = 0;
     }
	 
	 //coeffTempo();
  
     
     //########## Sous module pension d'indépendant ###############################
     // Calcul reproduisant les règles régime général, à valider
     if (l.LegSAM<2013 || t<117 || options->SAMSepare)
     {
        prorat = duree_in_maj/l.DureeProratRG;
        
        if(!options->NoBorneCoeffProrat)    
            prorat=min( (1+l.DeplafProrat/l.DureeProratRG), prorat );
     }
     else
     {
        prorat = (duree_rg_maj+duree_in_maj)/l.DureeProratRG;
        
        if(!options->NoBorneCoeffProrat)  
            prorat=min((1+l.DeplafProrat/l.DureeProratRG),prorat);     
        
        if (duree_rg_maj+duree_in_maj>0)
            prorat *= (duree_in_maj/(duree_rg_maj+duree_in_maj));   
        else
            prorat = 0;                                                
     }
  
     taux = (agetest<l.AgeMinRG) ? 0 : tauxliq_rg * l.TauxPleinRG;
  
     // # options de sam unique tous régimes
     if (options->SAMUnique)                                
        sam_in = sam_uni;
     if (options->SAMRgInUnique)                            
        sam_in = sam_rgin;
     if (l.LegSAM>=2013 && t>=117 && !options->SAMSepare)   
        sam_in = sam_rgin;
        
  
     //# Calcul de la pension avec condition de plafonnement
     pension_in=min( taux*M->PlafondSS[t],  taux*prorat*sam_in);
  
     taux_prorat_in = prorat;
  }
  
  
  void DroitsRetr::LiqPublic()
  {
     double prorat;
     double taux  = 0;
  
     majo_min_fp = 0;
     
     if (majo_3enf_fp == 0) {  //TODO
        majo_3enf_fp = 0; 
     }
  
     //######## Sous module pension civile #########################################
     //# Calcul du coefficient de proratisation
     //# modification 09/12/2011
     
     if (options->NoBorneCoeffProrat)   
        prorat = duree_fp_maj / l.DureeProratFP; 
     else                              
        prorat=min(
          0.8*(1+l.DeplafProrat/l.DureeProratFP)/0.75,
          (min(duree_fp,(l.DureeProratFP+l.DeplafProrat))
              +(duree_fp_maj-duree_fp))/l.DureeProratFP
          );
  
     taux_prorat_fp = prorat;
  
     //# Calcul du taux
     if (agetest>=l.AgeMinFP)
        taux = 0.75 * tauxliq_fp;
  
     // Calcul de la pension
     pension_fp = taux * prorat * sr_fp;

  }  //fin de la fonction LiqPublic
  
  
  //Simule la liquidation a l'âge courant. Cette fonction n'a pas de valeur de
  //retour. Elle modifie les variables pension_rg,  pension_fp ... et pension et ageliq. 
  //TODO: Elle modifie aussi les  variables $statut_[$i][$age[$i]] et $statut[$i].
void DroitsRetr::Liq()
{
   if (arr_mois(agetest) < min(arr_mois(l.AgeMinFP), arr_mois(l.AgeMinRG))  || (duree_tot_maj == 0))
   {                            // TODO: verifier pas de modification AgeMinRG, AgeMinFP
      pension_rg     = 0;
      pension_ar     = 0;
      pension_ag     = 0;
      pension_in     = 0;
      pension_fp     = 0;
      pension        = 0;
      VFU_rg         = 0;
      VFU_ar         = 0;
      VFU_ag         = 0;
      agefin_primoliq= 0;
      agefin_totliq  = 0;
	  pension_ag_ar  = 0;
   }
   

   //# Calcul de l'ensemble des variables intermédiaires et de la distance au
   //# taux plein
   //# RQ : le calcul des durées et du taux de liquidation a déjà été effectué lors de l'appel de AgeMin(i) dans TestLiq(i)
   SalBase();
   Points();
   
   
   
   if (duree_fp==0){
      pension_fp    = 0;
   }
   else   {   
      LiqPublic();        
   }
   
   if ( arr_mois(agetest) < arr_mois(l.AgeMinRG)
        || ((options->SecondLiq || options->anLeg<2014 || int_mois(X.anaiss+agetest, X.moisnaiss+1)<2015 || duree_fp==0) && options->tp && tauxliq_rg<1)) 
   {
      pension_rg     = 0;
      pension_ar     = 0;
      pension_ag     = 0;
      pension_in     = 0;
      VFU_rg         = 0;
      VFU_ar         = 0;
      VFU_ag         = 0;
	  pension_ag_ar  = 0;
   }
   else    
      LiqPrive();

   // Modif 12/02/2015 : Simulation pour option correcteur demo
   if(options->coeff_demo && M->correct_demo.size()>0) {
      pension_rg *= M->correct_demo[t];
      pension_fp *= M->correct_demo[t];
      pension_in *= M->correct_demo[t];
   }

   min_cont = 0;
   min_cont_in = 0;
      
 
   /// Application du minimum contributif (ou garanti)
   if (!options->NoMC || !options->NoMG)    
      AppliqueMin();
	  
   /// Application des bonifications pour 3 enfants
   if (!options->NoBonif && (X.nb_enf(age)>=3))    
      AppliqueBonif();

	  
coeffTempo();
  
   // Modif 12/02/2015 : Simulation pour option correcteur demo
   //if(options->coeff_demo && M->correct_demo.size()>0) {
    //   pension_ar *= M->correct_demo[t] ;//* M->ValPtARRCO[115] / M->ValPtARRCO[t] ;
    //   pension_ag *= M->correct_demo[t] ;//* M->ValPtAGIRC[115] / M->ValPtAGIRC[t] ;
   //}

   //######### Mises a jour finales ##############################################
   pension            =    pension_rg + pension_ar + pension_ag + pension_ag_ar +
                             pension_fp + pension_in ;
                             //pension_cn_pri + pension_cn_fp;
   
   if  ( pension > 0 && (duree_rg + duree_in>0) && (pension_rg + pension_in + VFU_rg==0) )
   {
      // primo-liquidation
      ageprimoliq        = age;
      ageliq             = 0;
      agefin_primoliq    = agetest;
      agefin_totliq      = 0;
      pliq               = 0; 
      primoliq           = true;
      liq                = false;
      dar                = primoliq && dar  ;
	  
	  if(!options->SecondLiq && options->anLeg>=2014 && int_mois(X.anaiss+agetest, X.moisnaiss+1)>=2015) {
	  for(int a=int_mois(agetest, X.moisnaiss+1)+1; a<=X.ageMax; a++) {
	  pop[X.Id].statuts[a]=S_INA;
	  }
	  }
	
	// Avant la réforme de 2014, le versement de cotisations après la primo-liquidation permet d'acquérir de nouveaux si elles sont versées à un régime ne versant pas la primo-pension.
	if(options->anLeg<2014 || int_mois(X.anaiss+agetest, X.moisnaiss+1)<2015) {
	for(int a=int_mois(agetest, X.moisnaiss+1)+1; a<=X.ageMax; a++) {
	  if(in(pop[X.Id].statuts[a],Statuts_FP))
		{pop[X.Id].statuts[a]=S_NONTIT;}
	  }
	}
	
   }
   else
   {
      // liquidation totale
      ageprimoliq        = int_mois(agetest, X.moisnaiss + 1 );
      ageliq             = int_mois(agetest, X.moisnaiss + 1 );
      agefin_primoliq    = agetest;
      agefin_totliq      = agetest;
      pliq               = pension;
      primoliq           = true;
      liq                = true;
      dar                = primoliq && dar  ;
   }
   // ???
    
   
   tp = 0;

   //# détermination du taux plein (différent selon les régimes d'affiliation). RQ : on définit l'indicatrice taux plein même si tous les droits ne sont pas liquidés
   if ((pension+VFU_rg+VFU_ar+VFU_ag>0) && agefin_totliq)
   {
        if (tauxliq_rg >= 1) tp=1;
        else if(duree_fp_maj > l.DureeCibFP && tauxliq_fp > 1) tp=1;
        else tp=0;
   }
   
   return;
}

//Fonction SecondLiq
//Permet la liquidation finale de tous les droits pour les personnes qui liquident en 2 temps
//Par hypothèse, on considère que la liquidation finale a lieu dès que la possibilité de partir au taux plein dans le privé
//est atteinte

void DroitsRetr::SecondLiq() {

   int t = int_mois(X.anaiss + agetest, 1+X.moisnaiss)%1900;
   int age = t - X.anaiss%1900;   
   
   SalBase();
   Points();
   
   LiqPrive();
   
   // Modif  12/02/2015 : Simulation option correcteur demo
   if(options->coeff_demo && M->correct_demo.size()>0) {
      pension_rg *= M->correct_demo[t];
      pension_in *= M->correct_demo[t];
   }   
   
   if (!options->NoMC || !options->NoMG) 
       AppliqueMin();
   if(!options->NoBonif && X.nb_enf(age) >= 3)
       AppliqueBonif();

coeffTempo();
   // Modif 12/02/2015 : Simulation option correcteur demo
   //if(options->coeff_demo && M->correct_demo.size()>0) {
  //     pension_ar *= M->correct_demo[t];// * M->ValPtARRCO[115] / M->ValPtARRCO[t] ;
  //     pension_ag *= M->correct_demo[t];// * M->ValPtAGIRC[115] / M->ValPtAGIRC[t] ;
  // }

       // On reprend les valeurs de la première liquidation
       // pour la fonction publique
       pension_fp     = X.retr->primoliq->pension_fp ;
       duree_fp       = X.retr->primoliq->duree_fp ;
       duree_fpa      = X.retr->primoliq->duree_fpa ;
       duree_fps      = X.retr->primoliq->duree_fps ;
       duree_fp_maj   = X.retr->primoliq->duree_fp_maj ;
       durdecote_fp   = X.retr->primoliq->durdecote_fp ;
       tauxliq_fp     = X.retr->primoliq->tauxliq_fp;
       majo_min_fp    = X.retr->primoliq->majo_min_fp;
       majo_3enf_fp   = X.retr->primoliq->majo_3enf_fp;
       sr_fp          = X.retr->primoliq->sr_fp;
       ntp_FP         = X.retr->primoliq->ntp_FP;
       pension_fp     = X.retr->primoliq->pension_fp;
       taux_prorat_fp = X.retr->primoliq->taux_prorat_fp;
       indic_mg       = X.retr->primoliq->indic_mg;
       ageprimoliq    = X.retr->primoliq->ageprimoliq;
       agefin_primoliq= X.retr->primoliq->agefin_primoliq;
   
       pension = pension_rg + pension_ar + pension_ag + pension_fp + pension_in + pension_ag_ar;
               /* $pension_cn_pri[$i]+$pension_cn_fp[$i] */
   
   //TODO: verif avec Liq
   
   ageliq             = int_mois(agetest, X.moisnaiss + 1 );
   agefin_totliq      = agetest;
   pliq               = pension;
   liq                = 1;   
   
   if( pension || VFU_rg || VFU_ar || VFU_ag) {
         tp = 1;
         if(duree_fp_maj > 0 && tauxliq_rg < 1) {
              if(duree_fp_maj<l.DureeCibFP)
                  tp = 0;
              if(tauxliq_fp < 1)
                  tp=0;
         }
         if((duree_rg_maj || duree_in_maj) && tauxliq_rg < 1)
            tp = 0;            
   }
   
}
/* Prise en compte de l'accord AGIRC-ARRCO 2015 sur la mise en place d'un coefficient temporaire de décote*/
void DroitsRetr::coeffTempo() {
coeffTemp =1.0;
ageAnnulCoeffTemp = agetest;

if((!options->NoCoeffTemp) && (l.an_leg >= 2015) && (!options->NoAccordAgircArrco) && (tauxliq_rg >=1) && (t >=119) && (X.anaiss >=1957) && duree_emprg>0) {
        /*if(dursurcote_rg < 0.25){
          coeffTemp = 1.0;
          ageAnnulCoeffTemp = agetest;
        }*/
		
        if(dursurcote_rg < 1) {
          coeffTemp = 0.9;
          ageAnnulCoeffTemp = min(agetest+3.0, l.AgeAnnDecRG );
        }
        else if(dursurcote_rg< 2) {
          coeffTemp = 1.0;
          ageAnnulCoeffTemp = agetest;
        }
        else if(dursurcote_rg< 3) {
          coeffTemp = 1.1;
          ageAnnulCoeffTemp = agetest + 1.0;
		  //if(X.Id==53074) {Rcout << " X.Id " << X.Id << " t " << t << " agetest " << agetest <<  " l.AgeAnnDecRG " << l.AgeAnnDecRG <<endl;}
        }
        else if(dursurcote_rg< 4) {
          coeffTemp = 1.2;
          ageAnnulCoeffTemp = agetest + 1.0;
        }
        else {
          coeffTemp = 1.3;
          ageAnnulCoeffTemp = agetest + 1.0;
        }
		
		//Aménagements spécifiques
		
		double moisLiq = arr_mois(X.anaiss + agetest,X.moisnaiss+1) - int_mois(X.anaiss + agetest,X.moisnaiss+1); // mois de liquidation
		int ageConsidere=(moisLiq>=9/12 && moisLiq<=1.0) ? age-1 : age-2; // l'âge conidéré pour le calcul du revenu fiscal. Si moisLiq est compris entre septembre et décembre, on prend le revenu à l'âge age-1. Sinon, c'est le revenu à age-2.
		double salPrimoLiq = (X.retr->primoliq && !(X.retr->primoliq->liq)) ? X.retr->pension_age(ageConsidere,REG_FP) : 0;
		
		if(X.matri[ageConsidere] != MARIE) {
			if(X.salaires[ageConsidere] + salPrimoLiq <= M->SeuilExoCSG[X.date(ageConsidere)]) {
			coeffTemp = max(1.0,coeffTemp);
		    // on ne modife pas l'ageAnnulCoeffTemp : si coeffTemp > 1.0, l'ageAnnulCoeffTemp précédent est conservé, si coeffTemp=1.0, la valeur de ageAnnulCoeffTemp n'a aucune incidence.
			}
			else if(X.salaires[ageConsidere] + salPrimoLiq <= M->SeuilTxReduitCSG[X.date(ageConsidere)]) {
				if(dursurcote_rg < 1) {
			     coeffTemp = 0.95;
                 ageAnnulCoeffTemp = min(agetest+3.0, l.AgeAnnDecRG ); // inutile
				}
			}
		}
			
			
		else if(pop[X.conjoint[ageConsidere]].retr->totliq) { // Pour le calcul du revenu du foyer fiscal, le conjoint pris en compte est celui au 31/12 de l'année de déclaration. Les couples sont supposés remplir une déclaration commune.
			int anneeLiq = (pop[X.conjoint[ageConsidere]].anaiss)%1900 + pop[X.conjoint[ageConsidere]].retr->totliq->ageliq;
			if(anneeLiq <= (t-1)) {
			double pensionConjoint = pop[X.conjoint[ageConsidere]].retr->pension_age(pop[X.conjoint[ageConsidere]].age(X.date(ageConsidere)), REG_TOT);
				if(X.salaires[ageConsidere] + salPrimoLiq + pensionConjoint <= M->SeuilExoCSG2[X.date(ageConsidere)]) {
				coeffTemp = max(1.0,coeffTemp);
		    // on ne modife pas l'ageAnnulCoeffTemp car si coeffTemp > 1.0, l'ageAnnulCoeffTemp précédent est conservé, si coeffTemp=1.0, la valeur de ageAnnulCoeffTemp n'a aucune incidence.
			    }
				else if(X.salaires[ageConsidere] + salPrimoLiq + pensionConjoint <= M->SeuilTxReduitCSG2[X.date(ageConsidere)]) {
					if(dursurcote_rg < 1) {
			     coeffTemp = 0.95;
                 ageAnnulCoeffTemp = min(agetest+3.0, l.AgeAnnDecRG ); // inutile
				}
			}
			}
		}
		else if(pop[X.conjoint[ageConsidere]].retr->primoliq) {
			 int anneePrimoLiq = (pop[X.conjoint[ageConsidere]].anaiss)%1900 + pop[X.conjoint[ageConsidere]].retr->primoliq->ageprimoliq;
			if(anneePrimoLiq <= (t-1)) {
			double pensionPrimoConjoint = pop[X.conjoint[ageConsidere]].retr->pension_age(pop[X.conjoint[ageConsidere]].age(X.date(ageConsidere)), REG_FP);
				if(X.salaires[ageConsidere] + salPrimoLiq + pensionPrimoConjoint + pop[X.conjoint[ageConsidere]].salaires[pop[X.conjoint[ageConsidere]].age(X.date(ageConsidere))] <= M->SeuilExoCSG2[X.date(ageConsidere)]) {
				coeffTemp = max(1.0,coeffTemp);
			    }
			    else if(X.salaires[ageConsidere] + salPrimoLiq + pensionPrimoConjoint + pop[X.conjoint[ageConsidere]].salaires[pop[X.conjoint[ageConsidere]].age(X.date(ageConsidere))] <= M->SeuilTxReduitCSG2[X.date(ageConsidere)]) {
					if(dursurcote_rg < 1) {
			     coeffTemp = 0.95;
                 ageAnnulCoeffTemp = min(agetest+3.0, l.AgeAnnDecRG );
				}
			}
			}
		}
		else if(X.salaires[ageConsidere] + salPrimoLiq + pop[X.conjoint[ageConsidere]].salaires[pop[X.conjoint[ageConsidere]].age(X.date(ageConsidere))] <= M->SeuilExoCSG2[X.date(ageConsidere)]) {
			coeffTemp = max(1.0,coeffTemp);
			    }
		else if(X.salaires[ageConsidere] + salPrimoLiq + pop[X.conjoint[ageConsidere]].salaires[pop[X.conjoint[ageConsidere]].age(X.date(ageConsidere))] <= M->SeuilTxReduitCSG2[ageConsidere]) {
					if(dursurcote_rg < 1) {
			     coeffTemp = 0.95;
                 ageAnnulCoeffTemp = min(agetest+3.0, l.AgeAnnDecRG );
				}
			}
			
		
     }
	 
	 pension_ar *=coeffTemp;
	 pension_ag *=coeffTemp;
	 pension_ag_ar *=coeffTemp;

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
