#include "Salaires.h"
#include "OutilsBase.h"
#include "Statistiques.h"



//' Fonction destinieImputSal
//' @description Attribue des revenus d'activités aux individus. Les statuts d'activité sont déjà connus.
//' @return Retourne le même environnement,
//' mais avec des revenus d'activité.
//' @param envSim Environment 
//' @export
// [[Rcpp::export]]
void destinieImputSal(Environment envSim) 
{
    auto S = Simulation(envSim);

    // Remet les statuts cadre en non-cadre
    for(Indiv& X : pop) 
      for(int age : range(0,X.ageMax+1)) {
        if(X.statuts[age]==S_CAD) X.statuts[age] = S_NC;
        if(X.statuts[age]==S_AVPF) X.statuts[age] = S_INA;
      }
    
    // Crée Aleas lissés par génération
    auto sexe = vector<int>(pop.size(),0);
    auto gen = vector<int>(pop.size(),0);
    for(Indiv& X : pop) {
      sexe[X.Id] = X.sexe;
      gen[X.Id] = X.anaiss% 1900;
    }
    
    auto aleas = make_vector2(AGE_MAX,2,vector<double>());
    for(int age : range(AGE_MAX))
    for(int n : range(2))
        aleas[age][n] = lisseAlea(sexe,gen,alea()); // Rmq : idée : deux aléas par individu à chaque âge
    
    
    // Imputation des salaires avec les aléas lissés
    auto sal = Salaire(envSim);
    
    for(Indiv& X : pop) {
        double e1 = aleas[0][0][X.Id];
        double e2 = aleas[0][1][X.Id];
        
        for(int age : range(13,X.ageMax+1)) {
            if(in(X.statuts[age], Statuts_occ)) {
                double e3 = aleas[age][0][X.Id];
                double e4 = aleas[age][1][X.Id];
                X.salaires[age] = sal.imput_sal(X, age, e1, e2, e3, e4);
            } else {
                X.salaires[age] = 0;
            }
        }
    }
    pointage("début AVPF");
    for(int t : range(71,AN_NB))
      sal.imput_avpf(t);
    pointage("Imput statut cadre");
    for(int t : range(AN_NB))
      sal.imput_statut_cadre(t);
    pointage("Fin imput statut cadre");
    
    sal.input_taux_prim();
    

    pointage("Fin imput taux prime");

const  vector<int> statuts_emprg       = {S_NC, S_NONTIT, S_CAD}  ;
const  vector<int> statuts_cad         = {S_CAD}  ;
const  vector<int> statuts_prive         = {S_NC, S_CAD}  ;
const  vector<int> Statuts_occ_avpf= {S_NC, S_CAD, S_NONTIT, S_SCOEMP, S_FPAE, 
                                  S_FPSE, S_FPAAE, S_FPATH, S_FPSTH, 
                                  S_FPAATH, S_IND, S_CHOEMPL, S_AVPF};
const  vector<int> statuts_cho         = {S_CHO, S_CHOBIT, S_CHONONBIT} ;
static vector<int> OCC = {S_NC, S_CAD, S_NONTIT, S_IND, S_FPAE, S_FPATH, S_FPSE, S_FPSTH};

	
    // Récrée la table emp
    for(auto& X : pop) for(auto age : range(0,X.ageMax+1)) {
          if (X.Id == 0) continue;
          static Rdout df("emp", {"Id","age","statut","salaire"});
          df.push_line(X.Id, age, X.statuts[age], X.salaires[age]);
    }
    
    pointage("fin ecriture emp");
    // Récrée la table ech
    for(auto& X : pop) {
      if (X.Id == 0) continue;
      static Rdout df("ech", {"Id","sexe","anaiss","findet","ageMax","taux_prim","typeFP","neFrance","emigrant","moisnaiss","k","peudip","tresdip","mere","pere","dipl"});
      df.push_line(X.Id, X.sexe, X.anaiss, X.findet, X.ageMax, X.taux_prim, X.typeFP, X.neFrance,X.emigrant, X.moisnaiss, X.k,X.peudip,X.tresdip,X.mere,X.pere,X.dipl);
    }
    pointage("fin ecriture ech");
}









double Salaire::recode_statut(int st) {
  return (
  in(st,{311,312,321,322,11}) ? 1 :
  in(st,{1,2})                ? 0 : 
  (st==4)                     ? 2 : -99);
}

double Salaire::duree_statut(Indiv& X, int age) {
  int statut_ref = recode_statut(X.statuts[age]);
  int duree = 0;
  for(int a=age; a >13 ; a--) {
    if(statut_ref==recode_statut(X.statuts[a])) {  duree++; }
    else { break; }
  }
  return duree;
}

double Salaire::duree_emp(Indiv& X, int age) {
  int duree = 0;
  for(int a=age; a > 13 ; a--) {
    if(in(X.statuts[a], Statuts_occ)) {
      duree++;
    }
  }
  return duree;
}

double alea_norm1(double alea1, double alea2) {
  return sqrt(-2 * log(alea1)) * cos(2*3.1415*alea2);
}

double alea_norm2(double alea1, double alea2) {
  return sqrt(-2 * log(alea1)) * sin(2*3.1415*alea2);
}


void Salaire::imput_salaires() // Rmq : jamais utilisée
{
  // Remet les statuts cadre en non-cadre
  for(Indiv& X : pop) 
    for(int age : range(0,X.ageMax+1)) {
      if(X.statuts[age]==S_CAD) X.statuts[age] = S_NC;
      if(X.statuts[age]==S_AVPF) X.statuts[age] = S_INA;
    }
  
  // Crée Aleas lissés par génération
  auto sexe = vector<int>(pop.size(),0);
  auto gen = vector<int>(pop.size(),0);
  for(Indiv& X : pop) {
    sexe[X.Id] = X.sexe;
    gen[X.Id] = X.anaiss% 1900;
  }
  
  auto aleas = make_vector2(AGE_MAX,2,vector<double>());
  for(int age : range(AGE_MAX))
  for(int n : range(2))
      aleas[age][n] = lisseAlea(sexe,gen,alea());
  
  
  // Imputation des salaires avec les aléas lissés

  for(Indiv& X : pop) {
      double e1 = aleas[0][0][X.Id];
      double e2 = aleas[0][1][X.Id];
      
      for(int age : range(13,X.ageMax+1)) {
          if(in(X.statuts[age], Statuts_occ)) {
              double e3 = aleas[age][0][X.Id];
              double e4 = aleas[age][1][X.Id];
              X.salaires[age] = imput_sal(X, age, e1, e2, e3, e4); 
          } else {
              X.salaires[age] = 0;
          }
      }
  }
}
//

double Salaire::imput_sal(Indiv& X, int age,  
                 double ei1, double ei2, 
                 double e1, double e2) {
  
  int t = X.date(age);
  
  if(age >=55 && X.salaires[age-1] != 0)
    return X.salaires[age-1] * M->SMPT[t] / M->SMPT[t-1];
  
  double D = duree_emp(X,age);
  int typeSal = 
    in(X.statuts[age],{1,2,11})                ? ( D<=18 ? (X.sexe == HOMME ? PRI_H_deb : PRI_F_deb) : (X.sexe == HOMME ? PRI_H_fin : PRI_F_fin)) :
    in(X.statuts[age],{311,321}) ? (X.sexe == HOMME ? FPE_H : FPE_F) :
	in(X.statuts[age],{312,322}) ? (X.sexe == HOMME ? FPHT_H : FPHT_F) :
    (X.statuts[age]==4)                     ? (X.sexe == HOMME ? IND_H : IND_F) : -99;
	
  r_assert(typeSal >= 0 && typeSal < 10);
  auto& eq = eqs[typeSal];      
  
  
  int findet = min_max(X.findet, 14, 26) - 14;
  int findet2 = findet*findet;
  double gen = min_max((X.anaiss-1935)/40.0,0,1); // modif 2017    
  
  if(options->SalNoEffetGen) gen = 0;
  double adferelatif =  - 5
  + 0.414          * findet
    + (1-0.414)/12.0 * findet2
    + 3.468          * (1-gen) 
    + 0.307         * findet * (1-gen)
    - 0.0576         * findet2 * (1-gen); // modif 2017
    
    if(options->SalNoEffetLinGen60) {
      adferelatif += esc(X.anaiss,corr);
    }
    
    double alea_indiv1 = min_max(alea_norm1(ei1,ei2), -3, 3); // Rmq : verification faite (cf. OutilsBase.h) : ei1 et ei2 sont indep et de loi uniforme sur [0,1] => Box-Müller s'applique. 
    double alea_indiv2 = min_max(alea_norm2(ei1,ei2), -3, 3);
	
    if(options_sal.noAlea) {
      alea_indiv1= 0;
      alea_indiv2 = 0;
    }
    
    double stock_dur_statut = duree_statut(X,age);
    double stock_dur_emp = duree_emp(X,age);
    
    double stock_dur_statut6 = max(0.0,6.0-stock_dur_statut);
    double stock_dur_emp6 = max(0.0,6.0-stock_dur_emp);
    double stock_dur_statut2 = stock_dur_statut*stock_dur_statut;
    double stock_dur_emp2 = stock_dur_emp*stock_dur_emp;
	double log_fp = log(M->PointFP[X.date(age)]);
    
	
    double xbeta =                                      
      eq.Intercept +
      (stock_dur_statut == 1)             *  eq.E_1er     +
      stock_dur_statut6                   *  eq.E_0       +
      stock_dur_statut                    *  eq.E         +
      stock_dur_statut2                   *  eq.E2        +
      stock_dur_statut  * adferelatif     *  eq.FR_E      +
      stock_dur_statut2 * adferelatif     *  eq.FR_E2     +
      stock_dur_statut6 * adferelatif     *  eq.FR_E_0    +
      stock_dur_emp2    * adferelatif     *  eq.FR_D2     +
      stock_dur_emp                       *  eq.D         +
      stock_dur_emp2                      *  eq.D2        +
      stock_dur_emp * adferelatif         *  eq.FR_D      +
      stock_dur_emp6                      *  eq.D_0       +
      stock_dur_emp6 * adferelatif        *  eq.FR_D_0    +
	  log_fp * eq.log_fp +
      adferelatif                         *  eq.FR        ;
       
    
    
    // AJOUT on simule le niveau individuel et la pente individuelle selon leurs moyennes, variances et correlations
    
    double estpente =  eq.Sig2pente_indiv * alea_indiv2;
    
    double estniv = 
      eq.corr_indiv * eq.Sig2_indiv * alea_indiv2 +
      eq.Sig2_indiv * sqrt(1-eq.corr_indiv*eq.corr_indiv) * alea_indiv1;
    // rmq : le vecteur (estniv,pente)' a été construit de sorte à suivre une loi gaussienne avec une moyenne et une matrice de covariance issues d'estimations.
    
    double ecarttype_res=eq.Sig2_res;
	
    bool indic_alea = (age<55) && !(options_sal.noAlea);
    double eps = min_max(alea_norm1(e1,e2), -2.0, 2.0) * indic_alea; 
	// rmq : le vecteur (estniv,pente)' est indépendant de eps. En particulier, (estniv,pente,eps)' est gaussien (si on néglige le min_max...)
	
	double ln_w=xbeta + estpente*stock_dur_emp + estniv + ecarttype_res*eps;	
    double correctStruct = (X.sexe==1) ? cs.CORREC_HOMMES[t] : cs.CORREC_FEMMES[t];
    double salaire = exp(ln_w) * correctStruct * M->SMPT[t];
    
    if(t==109) {
      static Rdout df("calcul_salaires",
      {"Id","age","correctStruct","estpente","estniv","ecarttype_res","stock_dur_emp","stock_dur_statut","ln_w","new","un","deux"});
      df.push_line(
        X.Id , age , correctStruct , estpente , estniv , ecarttype_res , stock_dur_emp , stock_dur_statut , ln_w , salaire ,  eq.Sig2pente_indiv , alea_indiv2);
    }

    return salaire;
}  

void Salaire::imput_statut_cadre(int t) {
  const vector<double> taux_cadre = 
    {1930,0.05, 2005,0.20, 2015,0.23, 2025,0.24, 2050,0.25};

  for(Indiv& X : pop)
    if(X.est_present(t) && X.statuts[X.age(t)]==S_CAD)
      X.statuts[X.age(t)] = S_NC;
    
  quantile qt;
  for(Indiv& X : pop)
    if(X.est_present(t))
      if(X.salaires[X.age(t)] > 0 && X.statuts[X.age(t)]==S_NC)
        qt.push(X.salaires[X.age(t)]*(1+(X.statuts[X.age(t-1)]==S_CAD)));
  
  double seuil = qt.resultat(1.0-affn(t+1900,taux_cadre)); // Rmq : Moralement, si p désigne le taux de cadre, les cadres sont les salariés du privé faisant partie des p% plus hauts salaires. Donc seuil = quantile d'ordre 1-p des salaires.
  for(Indiv& X : pop)
    if(X.est_present(t) && X.statuts[X.age(t)]==S_NC) {
      if(X.salaires[X.age(t)]*(1+(X.statuts[X.age(t-1)]==S_CAD)) > seuil)
        X.statuts[X.age(t)] = S_CAD;
    }
}



/*
bool test_avpf(Indiv& X, int age) {
  int t = X.date(age);
  //int ecart_age = 0;
  double plafond = 0;
  //if(X.conjoint[age]) ecart_age = pop[X.conjoint[age]].age(t) - age;
  int nbenf = X.nb_enf(age,999);
  int nbenfACharge = X.NbEnfC(t);
  int nbenf3 = X.nb_enf(age,3);
  if(X.sexe == FEMME && t > 71 && X.statuts[age]==S_INA && (nbenf3>0 || nbenfACharge>=3)) {
    if(!X.conjoint[age]) {
      plafond = 
        (nbenf==1) ? M->PlafARS1[t] :
        (nbenf==2) ? M->PlafARS2[t] :
        (nbenf==3) ? M->PlafARS3[t] :
        (nbenf==4) ? M->PlafARS4[t] :
        M->PlafARS5[t]*(nbenf-4)+M->PlafARS4[t];
      return X.salaires[age] < plafond;
    }
    else {
      Indiv& Y = pop[X.conjoint[age]];
      int age_y = Y.age(t);
      double majo = 0;
      if(X.salaires[age] > 0 && Y.salaires[age_y] >0)
        majo = M->MajoPlafCF[t] * (X.salaires[age-1] + Y.salaires[age_y] < 12 * M->BMAF[t]);
      
      plafond = 
        (nbenf==1) ? M->PlafCF1[t] :
        (nbenf==2) ? M->PlafCF2[t] :
        (nbenf==3) ? M->PlafCF3[t] :
        (nbenf==4) ? M->PlafCF4[t] :
        M->PlafCF5[t]*(nbenf-4)+M->PlafCF4[t];
      
      return (X.salaires[age] + Y.salaires[age_y]) < (plafond+majo);
    }
  }
  else return false;
}
*/
double revenuCoupleNet( Indiv& X,int age) {
	int t = X.date(age);
    double rev = X.SalNet(t);
    int conjoint = X.conjoint[age];
	Indiv& Y = pop[X.conjoint[age]];
    if(conjoint && Y.est_present(X.date(age)))
    {
      rev += Y.SalNet(t);  
    } 
    return rev;
  }
int prestafam(Indiv& X, int age) { // Indique le type de prestation perçue par X à l'âge age.
 int presta=0;
 int t = X.date(age);
 int nbenfACharge = X.NbEnfC(t);
 int nbenf3 = X.NbEnfC(t,3);
 //int nbenf1 = X.NbEnfC(t,1);
 double revcoupl=0.0;
 if (X.matri[age]==X.matri[age-2] ||(X.matri[age]==2 && X.conjoint[age]==X.conjoint[age-2])){ revcoupl=revenuCoupleNet(X,age-2);}
 else {revcoupl=revenuCoupleNet(X,age);}
 double plafondmoins3ans = 0;
 double plafondplus3enfants = 0;
 if (t<=77){
	plafondmoins3ans = (nbenfACharge==1) ? M->PlafARS1[t] :
        (nbenfACharge==2) ? M->PlafARS2[t] :
        (nbenfACharge==3) ? M->PlafARS3[t] :
        (nbenfACharge==4) ? M->PlafARS4[t] :
        M->PlafARS5[t]*(nbenfACharge-4)+M->PlafARS4[t];
	plafondplus3enfants = plafondmoins3ans;	
	}
 else if(t<=103){
	plafondmoins3ans = (nbenfACharge==1) ? M->PlafCF1[t] :
        (nbenfACharge==2) ? M->PlafCF2[t] :
        (nbenfACharge==3) ? M->PlafCF3[t] :
        (nbenfACharge==4) ? M->PlafCF4[t] :
        M->PlafCF5[t]*(nbenfACharge-4)+M->PlafCF4[t];
	if (X.matri[age] != MARIE) {plafondmoins3ans += M->MajoPlafCF[t]; } 	
	plafondplus3enfants = plafondmoins3ans;	
	}
 else if (t>103){
	plafondmoins3ans = (nbenfACharge==1) ? M->PlafAB1[t] :
	(nbenfACharge==2) ? M->PlafAB2[t] :
	(nbenfACharge==3) ? M->PlafAB3[t] :
	(nbenfACharge==4) ? M->PlafAB4[t] :
	M->PlafAB5[t]*(nbenfACharge-4)+M->PlafAB4[t];
	if (X.matri[age] != MARIE) {plafondmoins3ans += M->MajoPlafCF[t]; } 
	plafondplus3enfants = (nbenfACharge==1) ? M->PlafCF1[t] :
        (nbenfACharge==2) ? M->PlafCF2[t] :
        (nbenfACharge==3) ? M->PlafCF3[t] :
        (nbenfACharge==4) ? M->PlafCF4[t] :
        M->PlafCF5[t]*(nbenfACharge-4)+M->PlafCF4[t];
	if (X.matri[age] != MARIE) {plafondplus3enfants += M->MajoPlafCF[t]; } 
	}
	
// détermination de la prestation	familiale
 if (nbenf3>=1)
 {	presta = (revcoupl<=plafondmoins3ans)		  ? AB	  :	0 ;
   //CF à partir de 1978//APJE (depuis 1985)//AB de la PAJE un enfant de moins de trois	ans (rajouter la différence entre taux min et taux max à partir de 2014 ?)
	  if (presta==AB){
	  presta=(t<=93&&nbenfACharge>=3&&(X.SalNet(t)<0.8*X.SalNet(t-1)||(X.statuts[age]==S_INA&&X.statuts[age-1]!=S_INA )))            ? AB_CLCA	:				//APE
	  (t<=103&&nbenfACharge>=2&&(X.SalNet(t)<0.8*X.SalNet(t-1)||(X.statuts[age]==S_INA&&X.statuts[age-1]!=S_INA )))					        ? AB_CLCA	:
	  //((t>103)&& (nbenf1==1)&&(nbenfACharge==1)&&(X.SalNet(t)<0.8*X.SalNet(t-1)||(X.statuts[age]==S_INA&&X.statuts[age-1]!=S_INA)))	? AB_CLCA	:  			//CLCA complément de libre choix d'activité
	  ((t>103)&& (nbenf3>=1)&&(nbenfACharge>=2)&&(X.SalNet(t)<0.8*X.SalNet(t-1)||(X.statuts[age]==S_INA&&(X.statuts[age-1]!=S_INA||X.prestafam[age-1]==3))))	? AB_CLCA  : AB ;		}
	  else {presta=(t<=93&&nbenfACharge>=3&&(X.SalNet(t)<0.8*X.SalNet(t-1)||(X.statuts[age]==S_INA&&(X.statuts[age-1]!=S_INA||X.prestafam[age-1]==3))))        ? CLCA	:				//APE
	    (t<=103&&nbenfACharge>=2&&(X.SalNet(t)<M->SMIC[t]||(X.statuts[age]==S_INA&&X.statuts[age-1]!=S_INA)))					        ? CLCA	:
	    //((t>103)&& (nbenf1==1)&&(nbenfACharge==1)&&(X.SalNet(t)<0.8*X.SalNet(t-1)||(X.statuts[age]==S_INA&&(X.statuts[age-1]!=S_INA||X.prestafam[age-1]==3))))	? CLCA	:  			//CLCA complément de libre choix d'activité 
	    ((t>103)&& (nbenf3>=1)&&(nbenfACharge>=2)&&(X.SalNet(t)<0.8*X.SalNet(t-1)||(X.statuts[age]==S_INA&&(X.statuts[age-1]!=S_INA||X.prestafam[age-1]==3))))	? CLCA  : 0 ;	
	    }
 }
 else if ((nbenfACharge>=3) &&  (revcoupl<=plafondplus3enfants)) {presta=CF3enfs;}	//CF complement familial pour 3 enfants à partir de 1978
 X.prestafam[age]=presta;
 return presta;
}

bool test_avpf(Indiv& X, int age) {
  int t = X.date(age);
  int nbenfACharge = X.NbEnfC(t);
  int presta=prestafam(X,age);
  double plafond=0;
  if (X.matri[age]!=2) {
	plafond = (nbenfACharge==1) ?  M->PlafARS1[t] :
    (nbenfACharge==2) ? M->PlafARS2[t] :
    (nbenfACharge==3) ? M->PlafARS3[t] :
    (nbenfACharge==4) ? M->PlafARS4[t] :
    M->PlafARS5[t]*(nbenfACharge-4)+M->PlafARS4[t];}
  else{
	if (presta==AB){plafond = (nbenfACharge==1) ?  M->PlafARS1[t] :
    (nbenfACharge==2) ? M->PlafARS2[t] :
    (nbenfACharge==3) ? M->PlafARS3[t] :
    (nbenfACharge==4) ? M->PlafARS4[t] :
    M->PlafARS5[t]*(nbenfACharge-4)+M->PlafARS4[t];
	}
	else {
		if (presta==CF3enfs&& t<=77){plafond = (nbenfACharge==1) ?  M->PlafARS1[t] :
		(nbenfACharge==2) ? M->PlafARS2[t] :
		(nbenfACharge==3) ? M->PlafARS3[t] :
		(nbenfACharge==4) ? M->PlafARS4[t] :
		M->PlafARS5[t]*(nbenfACharge-4)+M->PlafARS4[t];
		}
		else {
			if (presta==CF3enfs&&t>77){plafond = (nbenfACharge==1) ?  M->PlafCF1[t] :
			(nbenfACharge==2) ? M->PlafCF2[t] :
			(nbenfACharge==3) ? M->PlafCF3[t] :
			(nbenfACharge==4) ? M->PlafCF4[t] :
			M->PlafCF5[t]*(nbenfACharge-4)+M->PlafCF4[t];
			}
			else {
				if ((presta==CLCA||presta==AB_CLCA)&&t<=84){
				plafond=0;}
				else {
					if ((presta==CLCA||presta==AB_CLCA) && t>84) plafond = (nbenfACharge==1) ?  M->PlafCF1[t] :
			(nbenfACharge==2) ? M->PlafCF2[t] :
			(nbenfACharge==3) ? M->PlafCF3[t] :
			(nbenfACharge==4) ? M->PlafCF4[t] :
			M->PlafCF5[t]*(nbenfACharge-4)+M->PlafCF4[t];
					}
				}
		}
	}
  }
double revcoupl=0.0;
  if (X.matri[age]==X.matri[age-2] ||(X.matri[age]==2 && X.conjoint[age]==X.conjoint[age-2])){ revcoupl=revenuCoupleNet(X,age-2);}
  else {revcoupl=revenuCoupleNet(X,age);}
  bool condrev=(revcoupl<=plafond);
  bool condact= (X.matri[age]!=2) ? TRUE :            // conditions de faible activite uniquement pour les couples
    ((X.SalNet(t)<12*M->BMAF[t]) && (presta==CF3enfs || presta==AB || presta==AB_CLCA))  ? TRUE : 
    ((X.SalNet(t)<0.63*M->PlafondSS[t]||X.statuts[X.age(t)]==S_INA) && (presta==CLCA || presta==AB_CLCA))                  ? TRUE : FALSE ;
  if (t<=84&&X.statuts[X.age(t)]!=S_INA) condact = FALSE ;
  return (condact  && condrev && (presta>0));
}

void Salaire::imput_avpf(int t) { // indique le nombre d'individus touchant l'AVPF par prestations
  int compt2=0;
  int compt2avpf=0;
  int compt1=0;
  int compt1avpf=0;
  int compt3=0;
  int compt3avpf=0;

  for(Indiv& X : pop){
    if (X.est_present(t)){ 
      int presta=0;
      if (X.age(t)>14) presta=prestafam(X,X.age(t));
      if(presta>0 &&(X.age(t)>14)&&X.sexe==FEMME){
        bool testavpf=test_avpf(X,X.age(t));
        if (testavpf && (X.statuts[X.age(t)] == S_INA) ) X.statuts[X.age(t)] = S_AVPF;
        if(presta==2) compt2++;	
        if(presta==2&&testavpf) compt2avpf++;	
        if(presta==3||presta==4) compt3++;	
        if((presta==3)&&testavpf) compt3avpf++;	
        if(presta==1||presta==4) compt1++;	
        if((presta==1||presta==4)&&testavpf) compt1avpf++;	
        for(int e=0; e < 6; e++) {
          Indiv& Enf = pop[X.enf[e]];
          if(Enf.est_present(t)) Enf.enfprestafam[Enf.age(t)]=1;
        }
      }
    }
  }  
if (t>=79){  //generalisation aux hommes
  for(Indiv& X :pop){
    if (X.est_present(t)){ 
      int presta=0;
      if (X.age(t)>14) presta=prestafam(X,X.age(t));
      if(presta>0 &&(X.age(t)>14)&&X.sexe==HOMME){
        bool testavpf=test_avpf(X,X.age(t));
        if (testavpf&& (X.statuts[X.age(t)] == S_INA) ) X.statuts[X.age(t)] = S_AVPF;
        if(presta==2) compt2++;	
        if(presta==2&&testavpf) compt2avpf++;	
        if(presta==3||presta==4) compt3++;	
        if(presta==3&&testavpf) compt3avpf++;	
        if(presta==1||presta==4) compt1++;	
        if((presta==1||presta==4)&&testavpf) compt1avpf++;	
        for(int e=0; e < 6; e++) {
          Indiv& Enf = pop[X.enf[e]];
          if(Enf.est_present(t)) Enf.enfprestafam[Enf.age(t)]=1;
        }
      }
    }
  }  
} 
int comptavpf=compt2avpf+compt3avpf+compt1avpf;
	 static auto df0 = Rdout("statprestafam",
    {"annee","CF","AVPFparCF",
     "AB","AVPFparAB","CLCA","AVPFparCLCA","totavpf"});
    df0.push_line(t+1900,compt2*M->poids,compt2avpf*M->poids,compt1*M->poids,compt1avpf*M->poids,
                  compt3*M->poids,compt3avpf*M->poids,comptavpf*M->poids);	
}

/*Fonction qui réimpute le taux de prime des fonctionnaires, à partir du niveau du dernier salaire relatif (jusqu'à 60 ans)
(les paramètres sont estimés dans l'EIR 2008, pour les fonctionnaires de la FPE (aucune donnée n'est malheureusement disponible
sur les taux de prime à la CNRACL) */

void Salaire::input_taux_prim() {
  static const vector<double> taux_prim = {0 , 2.5 , 7.5 , 12.5 , 17.5 , 22.5 , 27.5 , 32.5 , 37.5 , 42.5 , 47.5 , 52.5 , 57.5 , 62.5 , 67.5 , 72.5 , 77.5 , 82.5 , 87.5 , 92.5 , 100};
  static const vector3<double> dist_prim =
    {  
      {
        {0.22 , 0.554 , 0.096 , 0.045 , 0.019 , 0.003 , 0.01 , 0.009 , 0.004 , 0 , 0.006 , 0 , 0 , 0.002 , 0.003 , 0.002 , 0 , 0 , 0 , 0.004 , 0.023 },
        {0.065 , 0.389 , 0.258 , 0.126 , 0.082 , 0.027 , 0.016 , 0.009 , 0.002 , 0.001 , 0 , 0.003 , 0 , 0.003 , 0.004 , 0.001 , 0.003 , 0.002 , 0 , 0 , 0.009 },
        {0.034 , 0.159 , 0.169 , 0.225 , 0.167 , 0.113 , 0.046 , 0.024 , 0.006 , 0.009 , 0.004 , 0.002 , 0.004 , 0.001 , 0.006 , 0.004 , 0.001 , 0.001 , 0.002 , 0 , 0.023 },
        {0.039 , 0.264 , 0.139 , 0.085 , 0.051 , 0.061 , 0.194 , 0.064 , 0.051 , 0.012 , 0.01 , 0.005 , 0.004 , 0.005 , 0.004 , 0.002 , 0.002 , 0.001 , 0 , 0 , 0.007 },
        {0.009 , 0.254 , 0.272 , 0.137 , 0.066 , 0.054 , 0.055 , 0.041 , 0.042 , 0.02 , 0.01 , 0.007 , 0.005 , 0.001 , 0.006 , 0.005 , 0.005 , 0 , 0.002 , 0.001 , 0.008 },
        {0.013 , 0.123 , 0.238 , 0.17 , 0.126 , 0.061 , 0.042 , 0.042 , 0.051 , 0.029 , 0.014 , 0.026 , 0.016 , 0.008 , 0.009 , 0.005 , 0.001 , 0.005 , 0.003 , 0 , 0.018 },
        {0.01  , 0.066 , 0.187 , 0.179 , 0.135 , 0.111 , 0.041 , 0.044 , 0.032 , 0.062 , 0.034 , 0.027 , 0.012 , 0.012 , 0.004 , 0.007 , 0.016 , 0.003 , 0 , 0.005 , 0.013 },
        {0.004 , 0.105 , 0.128 , 0.167 , 0.079 , 0.084 , 0.052 , 0.047 , 0.025 , 0.025 , 0.07 , 0.054 , 0.066 , 0.036 , 0.013 , 0.005 , 0.008 , 0.002 , 0.005 , 0.004 , 0.021 },
        {0.027 , 0.071 , 0.065 , 0.07 , 0.058 , 0.05 , 0.037 , 0.027 , 0.024 , 0.044 , 0.074 , 0.04 , 0.044 , 0.055 , 0.027 , 0.025 , 0.034 , 0.012 , 0.035 , 0.017 , 0.164 }
      },
      {
        {0.191 , 0.488 , 0.113 , 0.089 , 0.047 , 0.017 , 0.009 , 0.008 , 0.001 , 0.006 , 0 , 0 , 0.002 , 0.013 , 0.003 , 0 , 0.002 , 0 , 0 , 0 , 0.011 },
        {0.047 , 0.437 , 0.201 , 0.124 , 0.081 , 0.035 , 0.014 , 0.005 , 0.004 , 0.001 , 0.004 , 0 , 0.001 , 0.004 , 0.015 , 0.011 , 0.004 , 0.002 , 0 , 0 , 0.01 },
        {0.035 , 0.266 , 0.202 , 0.129 , 0.1 , 0.081 , 0.053 , 0.028 , 0.009 , 0.014 , 0.005 , 0.002 , 0.002 , 0.003 , 0.021 , 0.015 , 0.005 , 0.001 , 0.001 , 0 , 0.028 },
        {0.095 , 0.506 , 0.239 , 0.061 , 0.02 , 0.012 , 0.02 , 0.016 , 0.008 , 0.004 , 0.003 , 0.004 , 0.001 , 0.002 , 0.001 , 0.001 , 0 , 0 , 0 , 0 , 0.007 },
        {0.017 , 0.329 , 0.333 , 0.156 , 0.055 , 0.018 , 0.02 , 0.012 , 0.011 , 0.015 , 0.008 , 0.007 , 0.004 , 0.005 , 0.004 , 0.002 , 0 , 0 , 0.001 , 0.001 , 0.002 },
        {0.021 , 0.172 , 0.273 , 0.179 , 0.102 , 0.04 , 0.027 , 0.019 , 0.02 , 0.068 , 0.022 , 0.022 , 0.002 , 0.011 , 0.003 , 0 , 0 , 0.002 , 0 , 0.003 , 0.014 },
        {0.011 , 0.126 , 0.254 , 0.201 , 0.085 , 0.028 , 0.023 , 0.013 , 0.025 , 0.047 , 0.031 , 0.055 , 0.028 , 0.013 , 0.015 , 0.006 , 0 , 0 , 0.002 , 0 , 0.037 },
        {0.014 , 0.084 , 0.201 , 0.186 , 0.11 , 0.102 , 0.043 , 0.049 , 0.029 , 0.043 , 0 , 0 , 0.084 , 0.006 , 0 , 0.02 , 0.02 , 0 , 0 , 0 , 0 },
        {0     , 0.076 , 0.037 , 0.061 , 0.04 , 0.067 , 0.066 , 0.067 , 0.114 , 0.045 , 0.046 , 0.044 , 0.066 , 0.005 , 0.015 , 0.041 , 0.016 , 0.012 , 0.057 , 0 , 0.125 }
      }
    };
  
  for(Indiv& X : pop) {
    int age60_max = min_max(X.ageMax,14,60);
    int age = 0;
    for(int a : range(age60_max-14))
      if(in(X.statuts[age60_max-a],{311,321,331,312,322,332})) { // on considère le dernier statut dans la FP hors invalidité
        age = age60_max - a;
        break;
      }
    if(age) {
      int srel= min_max(int(4.0*(X.salaires[age] / M->SMPT[X.date(age)]))-1,0,8 /* 2.25*4*/);
      X.taux_prim = taux_prim.at(Multinom(alea(),dist_prim.at(X.sexe-1).at(srel))) / 100;
      
      static Rdout df0("test",{"Id","multi","srel","taux_prim"});
      df0.push_line(X.Id, dist_prim.at(X.sexe-1).at(srel).at(5),srel,X.taux_prim);
    } else {
      X.taux_prim = 0.0;
    }
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