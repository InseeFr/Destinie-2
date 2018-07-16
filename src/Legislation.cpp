#include "Legislation.h"

using namespace std;

typedef vector<int> VI;
typedef vector<double> VD;

double Leg::an_ouv_droit_fp(const Indiv & X, int age) {
    durfpa = 0;
    double durfp = 0, durprive = 0;
    int age_max = min(int_mois(AgeMinFP-1,X.moisnaiss+1), age-1);
    for(int a = 15; a <= age_max; a++) {
        if(in(X.statuts[a], Statuts_FPA))    durfpa++;
        if(in(X.statuts[a], Statuts_FPAS))   durfp++;
        if(in(X.statuts[a], Statuts_Priv))   durprive++;
        
        if (durfpa>=DureeMinFPA && a>=54 && !options->NoRetrAntFPA) {
            return X.anaiss + a + 1; 
        }
        
        if (X.sexe == FEMME && X.enf[2] > 0 && X.anaissEnf[2] <= X.anaiss+a && !options->NoRetrAntFP3Enf) {
            if (an_leg < 2010 && durfp>=DureeMinFP) {
                return X.anaiss+a+1; 
            }
            else if(an_leg >= 2010 && durfp>=15 && X.anaiss+a<2012) {
                return X.anaiss+a+1; 
            }
        }
    }
    return int_mois(X.anaiss + AgeMinFP, X.moisnaiss);
}

void Leg::leg_priv(const Indiv & X, int age) {
   VD TauxRGMax83      = { 1972,0.40, 1975,0.50};
   VD DureeCibRG2003   = { 1933,37.5, 1943,40};
   VD DureeCalcSAM2003 = { 1933,10  , 1948,25};
   VD DureeCalcSAM2007 = { 1933,10  , 1948,25};

   VD DureeCibRG2007 = { 1933,37.5,1943,40,1948,40,1952,41,1953,41.25,1954,41.25,1955,41.5,1957,41.5,1958,41.75};
   VD DureeCibRG2013 = { 1933,37.5,1943,40,1948,40,1952,41,1953,41.25,1954,41.25,1955,41.5,1957,41.5,1958,41.75,
                         1960,41.75,1961,42,1963,42,1964,42.25,1966,42.25,1967,42.5,1969,42.5,1970,42.75,1972,42.75,1973,43};
   VD DureeCibRG2013ProlongDuree = 
                        { 1933,37.5,1943,40,1948,40,1952,41,1953,41.25,1954,41.25,1955,41.5,1957,41.5,1958,41.75,
						 1960,41.75,1961,42,1963,42,1964,42.25,1966,42.25,1967,42.5,1969,42.5,1970,42.75,1972,42.75,1973,43
                         ,1975,43,1976,43.25,1978,43.25,1979,43.5,1981,43.5,1982,43.75,1984,43.75,1985,44,1987,44,1988,44.25,
                         1990,44.25,1991,44.5};
   
   VD DureeCibRGProlongDuree = {
     1958,41.75 , 1961, 41.75, 1962,42   , 1964,42   , 1965,42.25, 1967,42.25, 1968,42.5, 
     1970,42.5  , 1971,42.75, 1974,42.75, 1975,43   , 1977,43,    1978,43.25, 1981,43.25, 
     1982,43.5  , 1984,43.5 , 1985,43.75, 1988,43.75, 1989,44,    1991,44   , 1992,44.25, 
     1995,44.25 , 1996,44.5 , 1999,44.5 , 2000,44.75 };
	 
   VD DureeCibRGDuree1960 = {1933,37.5,1943,40,1948,40,1952,41,1953,41.25,1954,41.25,1955,41.5,1957,41.5,1958,41.75,
                         1960,41.75};
   
   VD DureeProratRG2007 = { 1943,37.5,1948,40,1952,41,1953,41.25,1954,41.25,1955,41.5,1957,41.5,1958,41.75}; //modif  09/12/2011
   
   VD DureeProratRG2013 = { 1943,37.5,1948,40,1952,41,1953,41.25,1954,41.25,1955,41.5,1957,41.5,1958,41.75,
                    1960,41.75,1961,42,1963,42,1964,42.25,1966,42.25,1967,42.5,1969,42.5,1970,42.75,1972,42.75,1973,43}; //modifs  30/08/2013

   VD DureeProratRG2013ProlongDuree = DureeCibRG2013ProlongDuree;
   
   VD DureeProratRGDuree1960 = DureeCibRGDuree1960;
   VD DecoteRG2007   = {1943,0.05 ,1953,0.0250 };
   VD SurcoteRG12007 = {2003,0    ,2004,0.0150 };
   VD SurcoteRG22007 = {2003,0    ,2004,0.0150 };
   VD SurcoteRG32007 = {2003,0    ,2004,0.0150 };
   
   VD AgeMinRG2010 = { 1950,60.0,1956,62.0};
   VD AgeAnnDecRG2010 = { 1950,65.0,1956,67.0};   
   VD acc2012 = { 1951,0.0,1955,(4/12.0),1956,0.0};
   
   int g = X.anaiss;
   int t = X.anaiss%1900 + age;
   
   if (options->age60_65ans) {
      // Annule la modifiation des âges légaux
      // sans annuler les reformes utltérieures à 2010
      AgeMinRG2010 = { 1950,60.0,1956,60.0};
      AgeAnnDecRG2010 = { 1950,65.0,1956,65.0};   
      acc2012 = { 1951,0.0,1955,0.0,1956,0.0};     
   }
   if (options->age60_67ans) {
      // Annule la modifiation des âges légaux
      // sans annuler les reformes utltérieures à 2010
      AgeMinRG2010 = { 1950,60.0,1956,62.0};
      AgeAnnDecRG2010 = { 1950,65.0,1956,65.0};   
      acc2012 = { 1951,0.0,1955,(4/12.0),1956,0.0};     
   }
    if (options->age64_67ans) {
      // Prolonge les modifications des ages légaux suivant proposition du sénat plfss2015
      AgeMinRG2010 = { 1950,60.0,1956,62.0};
      AgeAnnDecRG2010 = { 1950,65.0,1956,67.0};   
      acc2012 = { 1951,0.0,1955,(4/12.0),1956,(5/12.0),1959,(20/12.0),1960,2};     
   }
     if (options->age64_69ans) {
      // Prolonge les modifications des ages légaux suivant proposition du sénat plfss2015
      AgeAnnDecRG2010 = { 1950,65.0,1956,67.0};   
      acc2012 = { 1951,0.0,1955,(4/12.0),1956,(5/12.0),1959,(20/12.0),1960,2}; 
      AgeMinRG2010 = { 1950,60.0,1956,62.0};    
   }  

   if (an_leg < 1946)
   {
      DureeCibRG     =99;
      DureeProratRG  =1;
      DureeCalcSAM   =0;
      TauxPleinRG    =0;
      DecoteRG       =0;
      SurcoteRG1     =0.0;
      SurcoteRG2     =0.0;
      SurcoteRG3     =0.0;
      TauxRGMax      =0.0;
      MajTauxRGMax   =0.0;
   }
   else if (an_leg < 1972)
   {
      DureeCibRG     = 99;
      DureeProratRG  = 30;
      DureeCalcSAM   = 10;
      TauxPleinRG    = 0.4;
      DecoteRG       = 0.040;
      SurcoteRG1     = 0.040;
      SurcoteRG2     = 0.040;
      SurcoteRG3     = 0.040;
      TauxRGMax      = 0.40;
      MajTauxRGMax   = 0.04;
   }
   else if (an_leg < 1983)
   {
      DureeCibRG     = 99;
      DureeProratRG  = 37.5;
      DureeCalcSAM   = 10;
      TauxPleinRG    = 0.5;
      DecoteRG       = 0.05;
      SurcoteRG1     = 0.05;
      SurcoteRG2     = 0.05;
      SurcoteRG3     = 0.05;
      TauxRGMax      = affn(an_leg, TauxRGMax83);       //
      MajTauxRGMax   = affn(an_leg, TauxRGMax83) / 10.0;  //TODO: modif rapide
   }
   else if (an_leg < 1993)
   {
      DureeCibRG     = 37.5;
      DureeProratRG  = 37.5;
      DureeCalcSAM   = 10;
      TauxPleinRG    = 0.5;
      DecoteRG       = 0.05;
      SurcoteRG1     = 0.0;
      SurcoteRG2     = 0.0;
      SurcoteRG3     = 0.0;
      TauxRGMax      = 0.5 ;
      MajTauxRGMax   = 0.0;
   }
   else if (an_leg < 2003)
   {
      DureeCibRG     = affn(g, DureeCibRG2003);
      DureeProratRG  = 37.5;
      DureeCalcSAM   = affn(g, DureeCalcSAM2003);
      TauxPleinRG    = 0.5;
      DecoteRG       = 0.05;
      SurcoteRG1     = 0.0;
      SurcoteRG2     = 0.0;
      SurcoteRG3     = 0.0;
      TauxRGMax      = 0.5 ;
      MajTauxRGMax   = 0.0;
   }   
   else if (an_leg < 2007)
   {
      DureeCibRG     = affn(g, DureeCibRG2007);
      DureeProratRG  = affn(g, DureeProratRG2007);  //modif 09/12/2011
      if (g >= 1943 && options->BlockDuree40)
      {
        DureeCibRG    = min(40.0,DureeCibRG);
        DureeProratRG = DureeCibRG;
      }
      DureeCalcSAM   = affn(g,DureeCalcSAM2003);
      TauxPleinRG    = 0.5;
      DecoteRG       = affn(g,        DecoteRG2007  );  //modif 09/12/2011
      SurcoteRG1     = affn(an_leg, SurcoteRG12007);
      SurcoteRG2     = affn(an_leg, SurcoteRG22007);
      SurcoteRG3     = affn(an_leg, SurcoteRG32007);
      TauxRGMax      = 0.5;
      MajTauxRGMax   = 0.0;
   }  
   else if (an_leg < 2009)
   {
      DureeCibRG     = affn(g, DureeCibRG2007);
      DureeProratRG  = affn(g, DureeProratRG2007);  //modif 09/12/2011
      if(g >= 1958 && options->ProlongDuree)
      {
        DureeCibRG    = affn(g, DureeCibRGProlongDuree);
        DureeProratRG = DureeCibRG;         
      }
	  if(g >= 1958 && options->BlockDureegen1960)
      {
        DureeCibRG    = affn(g,DureeCibRGDuree1960);
        DureeProratRG = DureeCibRG;         
      }
      if (g >= 1943 && options->BlockDuree40)
      {
        DureeCibRG    = min(40.0,DureeCibRG);
        DureeProratRG = DureeCibRG;
      }
      DureeCalcSAM   = affn(g,DureeCalcSAM2003);
      TauxPleinRG    = 0.5;
      DecoteRG       = affn(g,        DecoteRG2007  );  //modif 09/12/2011
      SurcoteRG1     = 0.015;
      SurcoteRG2     = 0.02;
      SurcoteRG3     = 0.025;
      TauxRGMax      = 0.5;
      MajTauxRGMax   = 0.0;
   } 
   else if (an_leg < 2010) // ajout qui permet de prendre en compte LFSS 70 ans et non plus 65 ans  (modif : 2010 et non 2011 !)
   { 
      AgeMaxRG       = 70; // modifs 10-11
      AgeAnnDecRG    = 65; // modifs 10-11   
      DureeCibRG     = affn(g, DureeCibRG2007);
      DureeProratRG  = affn(g, DureeProratRG2007);  //modif 09/12/2011
      if(g >= 1958 && options->ProlongDuree)
      {
        DureeCibRG    = affn(g, DureeCibRGProlongDuree);
        DureeProratRG = DureeCibRG;         
      }
	  if(g >= 1958 && options->BlockDureegen1960)
      {
        DureeCibRG    = affn(g,DureeCibRGDuree1960);
        DureeProratRG = DureeCibRG;         
      }
      if (g >= 1943 && options->BlockDuree40)
      {
        DureeCibRG    = min(40.0,DureeCibRG);
        DureeProratRG = DureeCibRG;
      }
      DureeCalcSAM   = affn(g,DureeCalcSAM2003);
      TauxPleinRG    = 0.5;
      DecoteRG       = affn(g,        DecoteRG2007  );  //modif 09/12/2011
      SurcoteRG1     = 0.025;  // soit 1.25% * 4 * 50% = 2.5% pas de différenciation entre les 4 premiers et les suivants
      SurcoteRG2     = 0.025;
      SurcoteRG3     = 0.025;
      TauxRGMax      = 0.5;
      MajTauxRGMax   = 0.0;
   }
   else if (an_leg < 2013) // Réforme 2010 modification AgeMinRG et AgeMaxRG
   { 
      AgeMaxRG       = 70; // modifs 10-11
      
      DureeCibRG     = affn(g, DureeCibRG2007);
      DureeProratRG  = affn(g, DureeProratRG2007);  //modif 09/12/2011
      if(g >= 1958 && options->ProlongDuree)
      {
        DureeCibRG    = affn(g, DureeCibRGProlongDuree);
        DureeProratRG = DureeCibRG;         
      }	  
	  if(g >= 1958 && options->BlockDureegen1960)
      {
        DureeCibRG    = affn(g,DureeCibRGDuree1960);
        DureeProratRG = DureeCibRG;         
      }
      if (g >= 1943 && options->BlockDuree40)
      {
        DureeCibRG    = min(40.0,DureeCibRG);
        DureeProratRG = DureeCibRG;
      }
      DureeCalcSAM   = affn(g,DureeCalcSAM2003);
      TauxPleinRG    = 0.5;
      DecoteRG       = affn(g,        DecoteRG2007  );  //modif 09/12/2011
      SurcoteRG1     = 0.025;  // soit 1.25% * 4 * 50% = 2.5% pas de différenciation entre les 4 premiers et les suivants
      SurcoteRG2     = 0.025;
      SurcoteRG3     = 0.025;
      TauxRGMax      = 0.5;
      MajTauxRGMax   = 0.0;
      
      
      AgeMinRG = affn(g, AgeMinRG2010);
      AgeAnnDecRG = affn(g, AgeAnnDecRG2010);
      
      if (g==1951 && X.moisnaiss<6)
      {
         AgeMinRG    = 60;
         AgeAnnDecRG = 65;
      }
      if (an_leg>=2011 && t>=112)  // accélération de la réforme, cf. PLFSS 2012 rectifié
      {
        AgeMinRG       += affn(g, acc2012);
        AgeAnnDecRG    += affn(g, acc2012);
      }
      AgeSurcote     = AgeMinRG;      
   } 
 else if (options->age64_67ans)// Réforme 2013    modification duree cotisation
   { 
      AgeMaxRG       = 70; 
      
      DureeCibRG     = affn(g, DureeCibRG2013);     //modifs  30/08/2013
      DureeProratRG  = affn(g, DureeProratRG2013);  //modifs  30/08/2013
      if(g >= 1958 && options->ProlongDuree)
      {
        DureeCibRG    = affn(g, DureeCibRGProlongDuree);
        DureeProratRG = DureeCibRG;         
      }
	  if(g >= 1958 && options->BlockDureegen1960)
      {
        DureeCibRG    = affn(g,DureeCibRGDuree1960);
        DureeProratRG = DureeCibRG;         
      }
      if (g >= 1943 && options->BlockDuree40)
      {
        DureeCibRG    = min(40.0,DureeCibRG);
        DureeProratRG = DureeCibRG;
      }
      DureeCalcSAM   = affn(g,DureeCalcSAM2003);
      TauxPleinRG    = 0.5;
      DecoteRG       = affn(g,        DecoteRG2007  );  //modif  09/12/2011
      SurcoteRG1     = 0.025;  // soit 1.25% * 4 * 50% = 2.5% pas de différenciation entre les 4 premiers et les suivants
      SurcoteRG2     = 0.025;
      SurcoteRG3     = 0.025;
      TauxRGMax      = 0.5;
      MajTauxRGMax   = 0.0;
      
      
      AgeMinRG = affn(g, AgeMinRG2010);
      AgeAnnDecRG = affn(g, AgeAnnDecRG2010);
      
      if (g==1951 && X.moisnaiss+1 < 7)
      {
         AgeMinRG    = 60;
         AgeAnnDecRG = 65;
      }
      if (an_leg>=2011 && t>=112)  // accélération de la réforme, cf. PLFSS 2012 rectifié
      {
        AgeMinRG       += affn(g, acc2012);
        AgeAnnDecRG    += affn(g, acc2012);
		    AgeAnnDecRG		= min(67.0,AgeAnnDecRG);
      }
      AgeSurcote     = AgeMinRG;      
   }
   else  // Réforme 2013    modification duree cotisation
   { 
      AgeMaxRG       = 70; 
      
      DureeCibRG     = affn(g, DureeCibRG2013);     //modifs  30/08/2013
      DureeProratRG  = affn(g, DureeProratRG2013);  //modifs  30/08/2013
      if(g >= 1958 && options->ProlongDuree)
      {
        DureeCibRG    = affn(g, DureeCibRGProlongDuree);
        DureeProratRG = DureeCibRG;         
      }
	  if(g >= 1958 && options->BlockDureegen1960)
      {
        DureeCibRG    = affn(g,DureeCibRGDuree1960);
        DureeProratRG = DureeCibRG;         
      }
      if (g >= 1943 && options->BlockDuree40)
      {
        DureeCibRG    = min(40.0,DureeCibRG);
        DureeProratRG = DureeCibRG;
      }
      DureeCalcSAM   = affn(g,DureeCalcSAM2003);
      TauxPleinRG    = 0.5;
      DecoteRG       = affn(g,        DecoteRG2007  );  //modif  09/12/2011
      SurcoteRG1     = 0.025;  // soit 1.25% * 4 * 50% = 2.5% pas de différenciation entre les 4 premiers et les suivants
      SurcoteRG2     = 0.025;
      SurcoteRG3     = 0.025;
      TauxRGMax      = 0.5;
      MajTauxRGMax   = 0.0;
      
      
      AgeMinRG = affn(g, AgeMinRG2010);
      AgeAnnDecRG = affn(g, AgeAnnDecRG2010);
      
      if (g==1951 && X.moisnaiss+1 < 7)
      {
         AgeMinRG    = 60;
         AgeAnnDecRG = 65;
      }
      if (an_leg>=2011 && t>=112)  // accélération de la réforme, cf. PLFSS 2012 rectifié
      {
        AgeMinRG       += affn(g, acc2012);
        AgeAnnDecRG    += affn(g, acc2012);
      }
	  
	  AgeSurcote     = AgeMinRG;
	  
	        
   }    
   
   if (an_leg >= 2013 && options->prolongeDuree2013) {
     DureeCibRG     = affn(g, DureeCibRG2013ProlongDuree);     //modifs  30/08/2013
     DureeProratRG  = affn(g, DureeProratRG2013ProlongDuree);  //modifs  30/08/2013
   }
   
}

void Leg::leg_pub(const Indiv & X, int age) { // AnOuvDroitFP


   VD DureeCibFP2010_an = {2003,37.5,2008,  40, 2012,41, 2013,41.25, 2014,41.25, 2015,41.5, 2017,41.5, 2018,41.75};
   VD DureeCibProlongDur_an = {
                    2018,41.75, 2021,41.75, 2022,42, 2024,42, 2025,42.25, 2027,42.25, 2028,42.5, 2030,42.5, 
                    2031,42.75, 2034,42.75, 2035,43, 2037,43, 2038,43.25, 2041,43.25, 2042,43.5, 2044,43.5, 
                    2045,43.75, 2048,43.75, 2049,44, 2051,44, 2052,44.25, 2055,44.25, 2056,44.5, 2059,44.5, 
                    2060,44.75 };
   VD SurcoteFP2010 = { 2003,   0, 2004,0.03, 2008,0.03, 2009,0.05};

   VD DecoteFP2010 = {2005,   0,2015,0.05};
   VD AgeAnnDecFP2010 = {2005,0, 2006,1, 2008,2, 2020,5};
   VD AgeMinFPacc2011 = {1956,0,1957,1/12.0,1958,2/12.0,1959,3/12.0,1960,4/12.0,1961,0};

   VD DureeCibFPA2010 = {1938,37.5,1948,37.5,1953,40,1957,41,1958,41.25,1959,41.25,1960,41.5,1962,41.5,1963,41.75};
   VD DureeCibFP2010 = {1933,37.5,1943,37.5,1948,40,1952,41,1953,41.25,1954,41.25,1955,41.5,1957,41.5,1958,41.75};
   VD DureeCibFPA2013= {1938,37.5 , 1948,37.5, 1949,38,1950,38.5,1951,39,1952,39.5,1953,40,1954,40.25,1955,40.5, 1957,41, 1958,41.25, 1959,41.5, 1960,41.5, 1961,41.75 , 1963,41.75,
                 1964,42, 1966,42, 1967,42.25, 1969,42.25, 1970,42.5 , 1972,42.5, 1973,42.75, 1975,42.75, 1976,43};
   VD DureeCibFP2013 = {1933,37.5 , 1943,37.5, 1948,40, 1952,41   , 1953,41.25, 1954,41.25, 1955,41.5, 1957,41.5 , 1958,41.75,
                 1960,41.75, 1961,42, 1963,42, 1964,42.25, 1966,42.25, 1967,42.5 , 1969,42.5, 1970,42.75, 1972,42.75, 1973,43};                    

   
   // Modif  : Création option prolongeDuree2013 2016/04/04
    VD DureeCibFPA2013ProlongDuree= 
                      {1938,37.5 , 1948,37.5, 1949,38,1950,38.5,1951,39,1952,39.5,1953,40,1954,40.25,1955,40.5, 1957,41, 1958,41.25, 1959,41.5, 1960,41.5, 1961,41.75 , 1963,41.75,
                 1964,42, 1966,42, 1967,42.25, 1969,42.25, 1970,42.5 , 1972,42.5, 1973,42.75, 1975,42.75, 1976,43,1978,43,
                        1979,43.25,1981,43.25,1982,43.5,1984,43.5,1985,43.75,1987,43.75,1988,44,1990,44,1991,44.25,1993,44.25,1994,44.5}; 
   VD DureeCibFP2013ProlongDuree = 
                      {1933,37.5 , 1943,37.5, 1948,40, 1952,41   , 1953,41.25, 1954,41.25, 1955,41.5, 1957,41.5 , 1958,41.75,
                        1960,41.75, 1961,42, 1963,42, 1964,42.25, 1966,42.25, 1967,42.5 , 1969,42.5, 1970,42.75, 1972,42.75, 1973,43,
                        1975,43,1976,43.25,1978,43.25,1979,43.5,1981,43.5,1982,43.75,1984,43.75,1985,44,1987,44,1988,44.25,
                         1990,44.25,1991,44.5};                    
    
   
   VD AgeMinMG2010 = {2010,0, 2011,0.5, 2015,3.5, 2016,4, 2020,5};
      
   
   VD DureeCibFPProlongDur= {
                1958,41.75 ,1961,41.75, 1962,42, 1964,42, 1965,42.25, 1967,42.25, 1968,42.5, 1970,42.5, 1971,42.75, 1974,42.75, 
                1975,43, 1977,43, 1978,43.25, 1981,43.25, 1982,43.5, 1984,43.5, 1985,43.75, 1988,43.75, 1989,44, 1991,44, 
                1992,44.25, 1995,44.25, 1996,44.5, 1999,44.5, 2000,44.75 };
                

   VD DureeCibFP1960= {1933,37.5 , 1943,37.5, 1948,40, 1952,41   , 1953,41.25, 1954,41.25, 1955,41.5, 1957,41.5 , 1958,41.75,
                        1960,41.75};
   VD DureeCibFPA1960= {1938,37.5 , 1948,37.5, 1953,40, 1957,41   , 1958,41.25, 1959,41.25, 1960,41.5 };               
               

   VD ageouvdroitfp2010 = {1950,60,1956,62};
   VD AgeMinFP2011      = {1955,55,1961,57};
   VD ageouvdroitfpacc2011 = {1951,0,1955,4/12.0,1956,0};


   if (options->age60_65ans) {
      // Annule la modifiation des âges légaux
      // sans annuler les reformes utltérieures à 2010
      ageouvdroitfp2010 = { 1950,60.0,1956,60.0};
      AgeMinFP2011 = { 1950,55.0,1956,55.0};
      AgeAnnDecFP2010 = {2005,0, 2006,0, 2008,0, 2020,0};   
      ageouvdroitfp2010 = { 1951,0.0,1955,0.0,1956,0.0};  
      ageouvdroitfpacc2011 = {1951,0,1955,0,1956,0};
      AgeMinFPacc2011 = {1956,0,1960,0,1961,0};
      AgeMinMG2010 = {2010,0, 2011,0, 2015,0, 2016,0, 2020,0};
   }
   if (options->age60_67ans) {
      // Annule la modifiation des âges légaux
      // sans annuler les reformes utltérieures à 2010
      ageouvdroitfp2010 = { 1950,60.0,1956,60.0};
      AgeMinFP2011 = { 1950,55.0,1956,55.0};
      AgeAnnDecFP2010 = {2005,0, 2006,1, 2008,2, 2020,5};   
      ageouvdroitfp2010 = { 1951,0.0,1955,0.0,1956,0.0};  
      ageouvdroitfpacc2011 = {1951,0,1955,0,1956,0};
      AgeMinFPacc2011 = {1956,0,1960,0,1961,0};
      AgeMinMG2010 = {2010,0, 2011,0, 2015,0, 2016,0, 2020,0};
   }

   if (options->age64_69ans || options->age64_67ans) {
      // 
	  AgeMinFP2011      = {1955,55,1961,57};
	  ageouvdroitfp2010 = {1950,60,1956,62};
      ageouvdroitfpacc2011 = {1951,0,1955,4/12.0,1956,5/12.0,1959,20/12.0,1960,2};
	  AgeMinFPacc2011 = {1956,0,1960,4/12.0,1961,5/12.0,1964,20/12.0,1965,2};
   }


   int t = X.anaiss%1900 + age;
   int g = X.anaiss;
   
   if  (an_leg<2003)
   {
      DureeCibFP  = 37.5;
      DureeProratFP = DureeCibFP;
      DecoteFP    = 0  ;
      SurcoteFP   = 0  ;
      AgeAnnDecFP = AgeMinFP  ;
      AgeMinMG    = AgeMinFP  ;
      
      AgeMaxFP    = (age>0 && in(X.statuts[age], Statuts_FPA))   ?   60  : 65;
   }
   else if (an_leg < 2010 || t<111) // modif  09/12/2011 : pour prendre en compte l'année d'ouverture des droits, et non la génération
   {
      DureeCibFP  = affn(AnOuvDroitFP, DureeCibFP2010_an);  // modif  16/03/2012
      
      if (AnOuvDroitFP>2018 && options->ProlongDuree)  
          DureeCibFP    = affn(AnOuvDroitFP-60, DureeCibFPProlongDur); 
          // Modif  30-04-2013 Hypothèses de COR
      
      if (options->BlockDuree40) 
          DureeCibFP = min(40.0,DureeCibFP);  
      
      DecoteFP    = affn(AnOuvDroitFP, DecoteFP2010);
      SurcoteFP   = affn(an_leg, SurcoteFP2010);
      AgeAnnDecFP = AgeMinFP + affn(AnOuvDroitFP,AgeAnnDecFP2010);
       
      if (AgeMinFP<55)
          AgeAnnDecFP = AgeMinFP;
          // Jamais de décote pour les mères de trois enfants
      
      AgeMinMG    = AgeMinFP  ;
      
      if ( age > 0 && in(X.statuts[age], Statuts_FPA))      
          AgeMaxFP    = 60;
      else   
          AgeMaxFP    = 65;
      
      DureeProratFP = DureeCibFP;
   }
   else if (options->age64_67ans)
   {
      if ( (AgeMinFP<55 && g+60-5<2011)  || (durfpa>=DureeMinFPA && g+55<2011) || (g+60<2011) )
      {
        // les parents de 3 enfants à moins de 5 ans de la retraite gardent l'ancien mode de calcul
        DureeCibFP  = affn(AnOuvDroitFP,DureeCibFP2010_an);  //modif  16/03/2012
        
        
        if (AnOuvDroitFP>2018 && options->ProlongDuree)
          DureeCibFP    = affn(AnOuvDroitFP, DureeCibFPProlongDur); // TODO : vérifier
          //Modif  30-04-2013 Hypothèses de COR
          
        if (options->BlockDuree40)
          DureeCibFP = min(40.0,DureeCibFP);
        
        DecoteFP    = affn(AnOuvDroitFP,DecoteFP2010);
        SurcoteFP   = affn(an_leg,SurcoteFP2010);
            
        //jamais de décote pour les mères de trois enfants    
        AgeAnnDecFP = (AgeMinFP<55) ? AgeMinFP : 
                                          AgeMinFP + affn(AnOuvDroitFP, AgeAnnDecFP2010);
            
        AgeMinMG    = AgeMinFP  ;
        
        // TODO: condition étrange : age > 0 ???
        AgeMaxFP = (age > 0 && in(X.statuts[age],Statuts_FPA)) ? 60 : 65 ;
            
        DureeProratFP = DureeCibFP;
      }
      else if (durfpa>=DureeMinFPA)
      {
        AgeMinFP  = affn(g,AgeMinFP2011);
        if (g==1955 && X.moisnaiss<6)
            AgeMinFP    = 55;
        if (an_leg>=2011 && t>111)
            AgeMinFP += affn(g,AgeMinFPacc2011);   // accélération de la réforme, cf. PLFSS 2012 rectifié
        
        AnOuvDroitFP = int_mois(X.anaiss + AgeMinFP, X.moisnaiss); //TODO: verif
        AgeAnnDecFP = AgeMinFP + affn(AnOuvDroitFP, AgeAnnDecFP2010);
		AgeAnnDecFP = min(62.0,AgeAnnDecFP);
        DecoteFP    = affn(AnOuvDroitFP, DecoteFP2010);
        

        DureeCibFP  = (an_leg < 2013) ? affn(g,DureeCibFPA2010) : affn(g,DureeCibFPA2013);
        // modifs  30/08/2013
        
        // Modif : Création option prolongeDuree2013 2016/04/04
        if(options->prolongeDuree2013 && an_leg >= 2013) {
          DureeCibFP  = affn(g,DureeCibFPA2013ProlongDuree);
        }

        //modifs  31-01-2011
        if (g>1963 && options->ProlongDuree)
            DureeCibFP    = affn(g-5, DureeCibFPProlongDur); //TODO: verif
        if ( g>1958 && options->BlockDureegen1960 ){
			DureeCibFP    = affn(g, DureeCibFPA1960); 
		} 			
        //modifs  30-04-2013 Hypothèses de COR
        if (options->BlockDuree40)
            DureeCibFP = min(40.0,DureeCibFP); 
            
        AgeMinMG    = AgeMinFP + affn(AnOuvDroitFP, AgeMinMG2010);
        AgeMinMG    = min(AgeMinMG,AgeAnnDecFP);
        AgeMaxFP = (age>0 && in(X.statuts[age], Statuts_FPA)) ? AgeMinFP+5 : AgeMinFP+10;

        DureeProratFP = DureeCibFP;
          
      }
      else if (AgeMinFP<60) // mères de 3 enfants ayant toutes les conditions avant 2011, mais moins de 55 ans : conservent le droit de départ, mais avec les paramètres de leur génération
      {
        ageouvdroitfp  = affn(g, ageouvdroitfp2010);
        if (g==1951 && X.moisnaiss < 6)        {   ageouvdroitfp    = 60;  }
        if (an_leg>=2011 && t>111)           {   ageouvdroitfp += affn(g, ageouvdroitfpacc2011);  }  // accélération de la réforme, cf. PLFSS 2012 rectifié
        
        AnOuvDroitFP = int_mois(X.anaiss + ageouvdroitfp, X.moisnaiss );
        AgeAnnDecFP = 60 + affn(AnOuvDroitFP, AgeAnnDecFP2010);
		AgeAnnDecFP = min(67.0,AgeAnnDecFP);
        DecoteFP    = affn(AnOuvDroitFP, DecoteFP2010);
        
        DureeCibFP  = (an_leg < 2013) ? affn(g,DureeCibFP2010) : affn(g,DureeCibFP2013);
            // modifs  31-01-2011
            // modifs  30-08-2013
        
        // Modif : Création option prolongeDuree2013 2016/04/04
        if(options->prolongeDuree2013 && an_leg >= 2013) {
          DureeCibFP  = affn(g,DureeCibFP2013ProlongDuree);
        }

        if ( g>1958 && options->ProlongDuree )  // modifs  30-04-2013 Hypothèses de COR
            DureeCibFP    = affn(g, DureeCibFPProlongDur); 
        if ( g>1958 && options->BlockDureegen1960 ){
			DureeCibFP    = affn(g, DureeCibFP1960); 
		}    
        if (options->BlockDuree40)
            DureeCibFP = min(40.0,DureeCibFP);
            
        AgeMinMG    = 60 + affn(AnOuvDroitFP,AgeMinMG2010);
		AgeMinMG	= min(AgeMinMG,AgeAnnDecFP);
        AgeMaxFP    = ageouvdroitfp+5;
        DureeProratFP = DureeCibFP;
      }
      else
      {
        AgeMinFP  = affn(g, ageouvdroitfp2010);
        if (g==1951 && X.moisnaiss<6)        {   AgeMinFP    = 60;  }
        if (an_leg>=2011 && t>111)         {   AgeMinFP += affn(g,ageouvdroitfpacc2011);  }  //accélération de la réforme, cf. PLFSS 2012 rectifié
        AnOuvDroitFP = int_mois(X.anaiss+AgeMinFP, X.moisnaiss);
        AgeAnnDecFP = AgeMinFP + affn(AnOuvDroitFP, AgeAnnDecFP2010);
		AgeAnnDecFP = min(67.0,AgeAnnDecFP);
        DecoteFP    = affn(AnOuvDroitFP, DecoteFP2010);
        
        DureeCibFP  = (an_leg < 2013) ? affn(g,DureeCibFP2010) : affn(g,DureeCibFP2013);
            // modifs  31-01-2011
            // modifs  30-08-2013
        
        // Modif : Création option prolongeDuree2013 2016/04/04
        if(options->prolongeDuree2013 && an_leg >= 2013) {
          DureeCibFP  = affn(g,DureeCibFP2013ProlongDuree);
        }
        
            
        if ( g>1958 && options->ProlongDuree )
            DureeCibFP    = affn(g, DureeCibFPProlongDur); // modifs  30-04-2013 Hypothèses de COR
        if ( g>1958 && options->BlockDureegen1960 ){
			 DureeCibFP    = affn(g, DureeCibFP1960); 
		}	 
		if (options->BlockDuree40)
            DureeCibFP = min(40.0,DureeCibFP);
            
        AgeMinMG      = AgeMinFP + affn(AnOuvDroitFP, AgeMinMG2010);
		AgeMinMG      = min(AgeMinMG   ,AgeAnnDecFP);
        AgeMaxFP      = AgeMinFP+5;
        DureeProratFP = DureeCibFP;
      }
      SurcoteFP   = 0.05;
   }
   else
   {
      if ( (AgeMinFP<55 && g+60-5<2011)  || (durfpa>=DureeMinFPA && g+55<2011) || (g+60<2011) )
      {
        // les parents de 3 enfants à moins de 5 ans de la retraite gardent l'ancien mode de calcul
        DureeCibFP  = affn(AnOuvDroitFP,DureeCibFP2010_an);  //modif  16/03/2012
        
        
        if (AnOuvDroitFP>2018 && options->ProlongDuree)
          DureeCibFP    = affn(AnOuvDroitFP, DureeCibFPProlongDur); // TODO : vérifier
          //Modif  30-04-2013 Hypothèses de COR
          
        if (options->BlockDuree40)
          DureeCibFP = min(40.0,DureeCibFP);
        
        DecoteFP    = affn(AnOuvDroitFP,DecoteFP2010);
        SurcoteFP   = affn(an_leg,SurcoteFP2010);
            
        //jamais de décote pour les mères de trois enfants    
        AgeAnnDecFP = (AgeMinFP<55) ? AgeMinFP : 
                                          AgeMinFP + affn(AnOuvDroitFP, AgeAnnDecFP2010);
            
        AgeMinMG    = AgeMinFP  ;
        
        // TODO: condition étrange : age > 0 ???
        AgeMaxFP = (age > 0 && in(X.statuts[age],Statuts_FPA)) ? 60 : 65 ;
            
        DureeProratFP = DureeCibFP;
      }
      else if (durfpa>=DureeMinFPA)
      {
        AgeMinFP  = affn(g,AgeMinFP2011);
        if (g==1955 && X.moisnaiss<6)
            AgeMinFP    = 55;
        if (an_leg>=2011 && t>111)
            AgeMinFP += affn(g,AgeMinFPacc2011);   // accélération de la réforme, cf. PLFSS 2012 rectifié
        
        AnOuvDroitFP = int_mois(X.anaiss + AgeMinFP, X.moisnaiss); //TODO: verif
        AgeAnnDecFP = AgeMinFP + affn(AnOuvDroitFP, AgeAnnDecFP2010);
        DecoteFP    = affn(AnOuvDroitFP, DecoteFP2010);
        

        DureeCibFP  = (an_leg < 2013) ? affn(g,DureeCibFPA2010) : affn(g,DureeCibFPA2013);
        // modifs  30/08/2013
        
        // Modif : Création option prolongeDuree2013 2016/04/04
        if(options->prolongeDuree2013 && an_leg >= 2013) {
          DureeCibFP  = affn(g,DureeCibFPA2013ProlongDuree);
        }

        //modifs  31-01-2011
        if (g>1963 && options->ProlongDuree)
            DureeCibFP    = affn(g-5, DureeCibFPProlongDur); //TODO: verif
        if ( g>1958 && options->BlockDureegen1960 ){
			DureeCibFP    = affn(g, DureeCibFPA1960); 
		} 			
        //modifs  30-04-2013 Hypothèses de COR
        if (options->BlockDuree40)
            DureeCibFP = min(40.0,DureeCibFP); 
            
        AgeMinMG    = AgeMinFP + affn(AnOuvDroitFP, AgeMinMG2010);
        
        AgeMaxFP = (age>0 && in(X.statuts[age], Statuts_FPA)) ? AgeMinFP+5 : AgeMinFP+10;

        DureeProratFP = DureeCibFP;
          
      }
      else if (AgeMinFP<60) // mères de 3 enfants ayant toutes les conditions avant 2011, mais moins de 55 ans : conservent le droit de départ, mais avec les paramètres de leur génération
      {
        ageouvdroitfp  = affn(g, ageouvdroitfp2010);
        if (g==1951 && X.moisnaiss < 6)        {   ageouvdroitfp    = 60;  }
        if (an_leg>=2011 && t>111)           {   ageouvdroitfp += affn(g, ageouvdroitfpacc2011);  }  // accélération de la réforme, cf. PLFSS 2012 rectifié
        
        AnOuvDroitFP = int_mois(X.anaiss + ageouvdroitfp, X.moisnaiss );
        AgeAnnDecFP = 60 + affn(AnOuvDroitFP, AgeAnnDecFP2010);
        DecoteFP    = affn(AnOuvDroitFP, DecoteFP2010);
        
        DureeCibFP  = (an_leg < 2013) ? affn(g,DureeCibFP2010) : affn(g,DureeCibFP2013);
            // modifs  31-01-2011
            // modifs  30-08-2013
        
        // Modif : Création option prolongeDuree2013 2016/04/04
        if(options->prolongeDuree2013 && an_leg >= 2013) {
          DureeCibFP  = affn(g,DureeCibFPA2013ProlongDuree);
        }

        if ( g>1958 && options->ProlongDuree )  // modifs  30-04-2013 Hypothèses de COR
            DureeCibFP    = affn(g, DureeCibFPProlongDur); 
        if ( g>1958 &&options->BlockDureegen1960 ){
			DureeCibFP    = affn(g, DureeCibFP1960); 
		}    
            
        if (options->BlockDuree40)
            DureeCibFP = min(40.0,DureeCibFP);
            
        AgeMinMG    = 60 + affn(AnOuvDroitFP,AgeMinMG2010);
        AgeMaxFP    = ageouvdroitfp+5;
        DureeProratFP = DureeCibFP;
      }
      else
      {
        AgeMinFP  = affn(g, ageouvdroitfp2010);
        if (g==1951 && X.moisnaiss<6)        {   AgeMinFP    = 60;  }
        if (an_leg>=2011 && t>111)         {   AgeMinFP += affn(g,ageouvdroitfpacc2011);  }  //accélération de la réforme, cf. PLFSS 2012 rectifié
		
	  
        AnOuvDroitFP = int_mois(X.anaiss+AgeMinFP, X.moisnaiss);
        AgeAnnDecFP = AgeMinFP + affn(AnOuvDroitFP, AgeAnnDecFP2010);
        DecoteFP    = affn(AnOuvDroitFP, DecoteFP2010);
        
        DureeCibFP  = (an_leg < 2013) ? affn(g,DureeCibFP2010) : affn(g,DureeCibFP2013);
            // modifs  31-01-2011
            // modifs  30-08-2013
        
        // Modif : Création option prolongeDuree2013 2016/04/04
        if(options->prolongeDuree2013 && an_leg >= 2013) {
          DureeCibFP  = affn(g,DureeCibFP2013ProlongDuree);
        }
        
            
        if ( g>1958 && options->ProlongDuree )
            DureeCibFP    = affn(g, DureeCibFPProlongDur); // modifs  30-04-2013 Hypothèses de COR
        if ( g>1958 && options->BlockDureegen1960 ){
			 DureeCibFP    = affn(g, DureeCibFP1960); 
		}	
        if (options->BlockDuree40)
            DureeCibFP = min(40.0,DureeCibFP);
            
        AgeMinMG      = AgeMinFP + affn(AnOuvDroitFP, AgeMinMG2010);
        AgeMaxFP      = AgeMinFP+5;
        DureeProratFP = DureeCibFP;
      }
      SurcoteFP   = 0.05;
   }
}

void Leg::leg_spe(const Indiv & X, int age) {
   // b2- Régimes spéciaux de salariés (EDF, SNCF, etc.)
   if ( (0) && (an_leg<2008) )
   {
      DureeCibFP  = 37.5;
      DecoteFP    = 0  ;
      SurcoteFP   = 0  ;
      AgeAnnDecFP = AgeMinFP  ;
      AgeMinMG    = AgeMinFP  ;
      AgeMaxFP    = AgeMinFP  ;  // mise à la retraite d'office
   }
   else if ( (0) && (an_leg>=2008) )
   {
   }   
}

void Leg::leg_dra(const Indiv & X, int age) {
   static VD ageDRA = {2010,56,2016,58};
   static VD ageDRA_acc = {2011,0,2015,(4/12),2016,0};
  
   int t = X.date(age);
   int g = X.anaiss;
   
   if(an_leg < 2003) {
      return;
   }
   else if(an_leg < 2009 || t < 109) {
      DureeValCibDRA[0] = 42;
      DureeCotCibDRA[0] = 42;   
      DebActCibDRA[0]   = 16;   
      AgeDRA[0]         = 56;   
       
      DureeValCibDRA[1] = 42;   
      DureeCotCibDRA[1] = 41;   
      DebActCibDRA[1]   = 16;
      AgeDRA[1]         = 58;

      DureeValCibDRA[2] = 42;
      DureeCotCibDRA[2] = 40;
      DebActCibDRA[2]   = 17;
      AgeDRA[2]         = 59;
   }
   else if (an_leg<2010 || t<111)
   {
      DureeValCibDRA[0] = 42;
      DureeCotCibDRA[0] = DureeCibRG+2;
      DebActCibDRA[0]   = 16;
      AgeDRA[0]         = 56;
    
      DureeValCibDRA[1] = 42;
      DureeCotCibDRA[1] = DureeCibRG+1;
      DebActCibDRA[1]   = 16;
      AgeDRA[1]         = 58;
    
      DureeValCibDRA[2] = 42;
      DureeCotCibDRA[2] = DureeCibRG;
      DebActCibDRA[2]   = 17;
      AgeDRA[2]         = 59;
   }   
   else
   {
      DureeValCibDRA[0] = DureeCibRG+2;
      DureeCotCibDRA[0] = DureeCibRG+2;
      DebActCibDRA[0]   = 16;
      AgeDRA[0]         = affn((g+56),ageDRA);
      if (arr_mois(g+56,X.moisnaiss) < arr_mois(2011,7))        
           AgeDRA[0]=56;
  
      DureeValCibDRA[1] = DureeCibRG+2;
      DureeCotCibDRA[1] = DureeCibRG+1;
      DebActCibDRA[1]   = 16;
      AgeDRA[1]         = affn((g+58),ageDRA)+2;
      if (arr_mois(g+58,X.moisnaiss) < arr_mois(2011,7))        
           AgeDRA[0]=58;
      
      DureeValCibDRA[2] = DureeCibRG+2;
      DureeCotCibDRA[2] = DureeCibRG;
      DebActCibDRA[2]   = 17;
      AgeDRA[2]         = affn((g+59),ageDRA)+3;        // rq : cette condition s'éteint d'elle même au bout d'un moment ...
      if (arr_mois(g+59,X.moisnaiss) < arr_mois(2011,7))        
           AgeDRA[0]=59;
           
      DureeValCibDRA[3] = DureeCibRG+2;
      DureeCotCibDRA[3] = DureeCibRG;
      DebActCibDRA[3]   = 18;
      AgeDRA[3]         = 60;
  
      if (an_leg>=2011 && t>=112)  //accélération de la réforme
      {
        AgeDRA[0]         += affn(g+56, ageDRA_acc);
        AgeDRA[1]         += affn(g+58, ageDRA_acc);
        AgeDRA[2]         += affn(g+59, ageDRA_acc);
      }
      
      if (an_leg>=2012 && t>=112) //nouvelles modalités de départ au titre des carrières longues
      {
        DureeValCibDRA[4] = DureeCibRG;
        DureeCotCibDRA[4] = DureeCibRG;
        DebActCibDRA[4]   = 20;
        
        if(arr_mois(X.anaiss+60,X.moisnaiss) < arr_mois(2012,10) )
              AgeDRA[4] = arr_mois(2012,10)-arr_mois(X.anaiss,X.moisnaiss);
        else
              AgeDRA[4] = 60;
      }
   }
}

Leg::Leg(const Indiv & X, int age, int f_an_leg)
{
   //int age = int_mois(agetest,X.moisnaiss + 1);
   int t = (X.anaiss + age)%1900;
   an_leg = f_an_leg;
   an_leg_DRA = an_leg;
   an_leg_SAM = an_leg; // TODO: Option an DRA et SAM
   
   int g=X.anaiss; 
   
   // Paramètres fixes dans les hypothèses de référence
   LegMin = an_leg;
   LegDRA = an_leg_DRA;
   LegSAM = an_leg_SAM;
   AgeMinRG = 60;
   AgeMaxRG = 65;
   AgeMaxFP    = 65;
   AgeAnnDecRG = 65;
   AgeMinMG    = 60;
   DureeMinFP  = 15;
   DureeMinFPA = 15;       //Modif  23/09/2011 Ajout Variable $DureeMinFPA = 15
   AgeMinMG    = UNDEF;
   AnOuvDroitFP= UNDEF;
   AgeSurcote  = 60;
   DureeCalcSAM= 10;
   DeplafProrat= 0;  
   
   // ajout  09/12/2011 : on prévoit 4 "jeux" de paramètres d'accès à 
   // la DRA + par défaut, la durée requise pour la retraite anticipée est 
   //fixée à une valeur abusivement élevée
   for(int d=0; d < 5;d++){
     DureeValCibDRA[d] = 99; 
     DureeCotCibDRA[d] = 99;
     DebActCibDRA[d]   = 0 ;
     AgeDRA[d]         = 99;
   }


   // AgeMinFP   
   static VD age_min_fp2010     = { 1950, 60.0,               1956, 62.0};
   static VD age_min_fp_acc2011 = { 1951,  0, 1955, (4/12.0), 1956, 0};
   
   
   if (options->age60_65ans) {
      // Annule la modifiation des âges légaux
      // sans annuler les reformes utltérieures à 2010
      age_min_fp2010     = { 1950, 60,               1956, 60};
      age_min_fp_acc2011 = { 1951,  0, 1955, 0, 1956, 0};
   }
   if (options->age64_67ans||options->age64_69ans) {
      // Annule la modifiation des âges légaux
      // sans annuler les reformes utltérieures à 2010
      age_min_fp2010     = { 1950, 60,               1956, 62.0};
      age_min_fp_acc2011 = { 1951,  0, 1955, (4/12.0), 1956, (5/12.0),1959,(20/12.0),1960,2};
   }
   
   
   if (an_leg < 2010) {
         AgeMinFP = 60;
   }
   else if (an_leg == 2010 || t<=111) {  // TODO: vérifier cette condition
         AgeMinFP  = (g == 1951 && X.moisnaiss < 6 )
                       ?  60
                       :  affn(g, age_min_fp2010);
   }
   else if (an_leg >= 2011) {
         AgeMinFP  = ((g==1951) && (X.moisnaiss<6))
                       ?  60
                       :  affn(g, age_min_fp2010) + affn(g,age_min_fp_acc2011);
   }
   
  
   // DureeMinFP et FPA
   
   static VD duree_min_FPA2011 = { 111,15, 112,(15+8/12), 116,17};
   static VD duree_min_FPA2012 = { 111,15, 112,(15+9/12), 115,17};
   
   
   if (an_leg < 2010 || t < 111) {
      DureeMinFPA = 15;
      DureeMinFP  = 15;
   }
   else if (an_leg == 2011  || t < 112) {
      //$DureeMinFPA = affn($Leg,2011,15,2012,(15+8/12),2016,17);       #RQ : on ignore la condition de 15+4/12 pour les nés au second semestre 2011
      DureeMinFPA = affn(t,duree_min_FPA2011);        //modif  10/07/2012
      DureeMinFP  = 2;
   }
   else {
      //$DureeMinFPA = affn($Leg,2011,15,2012,(15+8/12),2016,17);       #RQ : on ignore la condition de 15+4/12 pour les nés au second semestre 2011
      DureeMinFPA = affn(t,duree_min_FPA2012);        //modif  10/07/2012
      DureeMinFP  = 2;
   }
   
   AnOuvDroitFP = an_ouv_droit_fp(X, age); // durFPA
   AgeMinFP = min(AgeMinFP,AnOuvDroitFP-X.anaiss);
   
   
   leg_priv(X, age);
   leg_pub(X, age);
   leg_dra(X, age);
   
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