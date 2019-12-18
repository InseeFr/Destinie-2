#include "Simulation.h"
#include "Retraite.h"
#include "Indicateurs_annee_COR.h"
#include "Indicateurs_gen_age.h"
#include "Transitions.h"
#include "Salaires.h"
#include "OutilsBase.h"

double statut(Indiv& X,int age, const vector<int>& vs)
{
  r_assert(X.retr != nullptr);
  return X.est_present(X.date(age)) ?  
    X.retr->partavtliq(X.date(age)) * in(X.statuts[age],vs) : 0;
}

//' Fonction destinieCalageSalaires
//' @description Cale les salaires imputés pour respecter
//' la trajectoire du Salaire Moyen Par Tête (SMPT)
//' @return Retourne le meme environnement avec des salaires modifiés
//' @param envSim Environment 
//' @export
// [[Rcpp::export]]
void destinieCalageSalaires(Environment envSim) {
  auto S = Simulation(envSim);
  // Option "effet_hrzn" : 
  // décale l'âge de fin d'activité de 2 ans
  // à partir de la génération 1951
  if(options->effet_hrzn) { 
    for(Indiv& X : pop) {
      if(X.anaiss >= 1951) {
        int agefinAct = 0;
        
        for(int age : range(min(70,X.ageMax))) 
          if(in(X.statuts[age],Statuts_occ)) 
            agefinAct = age;
        
        if(agefinAct >= max(55,2010-X.anaiss)) {
          double decalage = 2 * partTx(X.anaiss, 1951, 1955);
          int agefinActNouv = agefinAct + int(decalage + alea());
          
          if(agefinAct+1 <= agefinActNouv && X.statuts[agefinAct+1] != 0) {
            X.statuts[agefinAct+1] = X.statuts[agefinAct];
            X.salaires[agefinAct+1] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+1)] / M->SMPT[X.date(agefinAct)];
          }
          if(agefinAct+2 <= agefinActNouv && X.statuts[agefinAct+2] != 0) {
            X.statuts[agefinAct+2] = X.statuts[agefinAct];
            X.salaires[agefinAct+2] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+2)] / M->SMPT[X.date(agefinAct)];
          }
        }
      }
    }
  }
  if(options->super_effet_hrzn)
    for(Indiv& X : pop)
      if(X.anaiss >= 1951) {
        int agefinAct = 0;
        
        for(int age : range(min(70,X.ageMax))) 
          if(in(X.statuts[age],Statuts_act)) 
            agefinAct = age;
        
        if(agefinAct >= max(50,2010-X.anaiss)) {
          double decalage = affn(X.anaiss,{1951,0,1955,2,1973,3.5});
          int agefinActNouv = agefinAct + int(decalage + alea());
          
          if(agefinAct+1 <= agefinActNouv && X.statuts[agefinAct+1] != 0) {
            X.statuts[agefinAct+1] = X.statuts[agefinAct];
            X.salaires[agefinAct+1] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+1)] / M->SMPT[X.date(agefinAct)];
          }
          if(agefinAct+2 <= agefinActNouv && X.statuts[agefinAct+2] != 0) {
            X.statuts[agefinAct+2] = X.statuts[agefinAct];
            X.salaires[agefinAct+2] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+2)] / M->SMPT[X.date(agefinAct)];
          }
          if(agefinAct+3 <= agefinActNouv && X.statuts[agefinAct+3] != 0) {
            X.statuts[agefinAct+3] = X.statuts[agefinAct];
            X.salaires[agefinAct+3] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+3)] / M->SMPT[X.date(agefinAct)];
          }
          if(agefinAct+4 <= agefinActNouv && X.statuts[agefinAct+4] != 0) {
            X.statuts[agefinAct+4] = X.statuts[agefinAct];
            X.salaires[agefinAct+4] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+4)] / M->SMPT[X.date(agefinAct)];
          }
        }
      }
      
  // Applique le même salaire relatifs par âge à tout le monde
  if(options->redresseSal) {
    auto vsal = make_vector2(AGE_MAX,3,Moyenne());
    pointage("debut redressement des salaires");
    for(Indiv& X : pop) for(int age : range(X.ageMax))
      if(X.est_present(X.date(age)) && in(X.statuts[age],Statuts_occ)) {
        vsal[age][X.sexe].push(X.salaires[age]/M->SMPT[X.date(age)]);
      }
    pointage("debut redressement des salaires");
    for(Indiv& X : pop) for(int age : range(X.ageMax)) 
      if(X.est_present(X.date(age)) && in(X.statuts[age],Statuts_occ)) {
        X.salaires[age] = vsal[age][X.sexe].resultat() * M->SMPT[X.date(age)];
      }
  }
  if(options->redresseSalNoFem || options->redresseFindet || options->redresseCar) {
    auto vsal = make_vector2(AGE_MAX,3,Moyenne());
    pointage("debut redressement des salaires");
    for(Indiv& X : pop) for(int age : range(X.ageMax))
      if(X.est_present(X.date(age)) && in(X.statuts[age],Statuts_occ)) {
        vsal[age][X.sexe].push(X.salaires[age]/M->SMPT[X.date(age)]);
      }
    if(options->redresseCar || options->redresseFindet) {
      if(options->redresseFindet) {
        for(Indiv& X : pop) {
            X.findet = 16;
          }
      }
      for(Indiv& X : pop) for(int age : range(X.findet,X.ageMax))
        if(X.est_present(X.date(age))) {
          X.statuts[age] = S_NC;
        }
    }
    pointage("debut redressement des salaires no car fem");
    for(Indiv& X : pop) for(int age : range(X.ageMax)) 
      if(X.est_present(X.date(age)) && in(X.statuts[age],Statuts_occ)) {
        X.salaires[age] = vsal[age][1].resultat() * M->SMPT[X.date(age)];
      }
  }  

  double ref(1);
  
  pointage("debut calcul des salaires");
  // boucle sur chaque année
  for(int t : range(50,AN_NB)) {

      double som(0), nb(0), som55(0), correct(1);
      // Calcul sommes salaires avant corrections
      // boucle sur chaque individu présent
      for(Indiv& X : pop)   if(X.est_present(t) && X.age(t) > 14) {
          int age = X.age(t);
          
          // calcul si liquidation
          if(age >= 50 && !X.retr->totliq) {
            X.retr->SimDir(age);
          }
          	  	 
          X.retr->revaloDir(t);
          
          
          double ind_senior1 = (age>=55) * 
              statut(X,age,Statuts_occ) * 
              statut(X,age-1,Statuts_occ);
          
          double ind_occ = statut(X,age,Statuts_occ);
          nb  += ind_occ;
          som += (ind_occ-ind_senior1) * X.salaires[age];
          som55 += ind_senior1 * X.salaires[age-1] * M->SMPT[t] / M->SMPT[t-1];
          // cumul des salaires de l'année
      }

      // Calcul du coefficient de correction de l'année
      correct = (t <= AN_BASE) ? 1   : ((M->SMPT[t] * nb * ref - som55) / som);
      ref     = (t != AN_BASE) ? ref : ((som + som55) / nb) / M->SMPT[t];
    
      // Application du coefficient de correction
      for(Indiv& X : pop) if(statut(X, X.age(t), Statuts_occ)>0)
      {
          int age = X.age(t);
          X.salaires[age] = (age >= 55 && statut(X, age-1, Statuts_occ))  ?
                            X.salaires[age-1] * M->SMPT[t] / M->SMPT[t-1] :
                            X.salaires[age] * correct ;
      }
      // Controle moyenne salaires après correction
      Moyenne msal;
      for(Indiv& X : pop) 
      {
        if(statut(X, X.age(t), Statuts_occ)>0)
        {
            int age = X.age(t);
            msal.push(true, 
                      X.salaires[age] / M->SMPT[t], 
                      statut(X, age, Statuts_occ));
        }
      } 
        
      // export des controls
      static Rdout df("correct_sal",{"annee","correct","SMPT.new"});
      df.push_line(t+1900, correct, msal.resultat());

  }

  // Sort les salaires moyens gen*sexe*age
  auto salMoy_sga = make_vector3(3,AN_NB,AGE_MAX,Moyenne());
  auto tauxActMoy_sga = make_vector3(3,AN_NB,AGE_MAX,Somme());
  for(Indiv& X : pop) for(int age : range(X.ageMax)) { 
    double poids = statut(X, age, Statuts_occ);
    bool ind_champ = poids > 0;
    double salaire = X.salaires[age] / M->SMPT[X.date(age)];  
    salMoy_sga[X.sexe][X.anaiss%1900][age].push(ind_champ,salaire,poids);
    salMoy_sga[0][X.anaiss%1900][age].push(ind_champ,salaire,poids);
    tauxActMoy_sga[X.sexe][X.anaiss%1900][age].push(poids);
    tauxActMoy_sga[0][X.anaiss%1900][age].push(poids);
  }
  
  static Rdout df_salMoy("SalMoy",{"sexe","gen","age","smpt"});
  for(auto i : range(3)) for(auto j : range(AN_NB)) for(auto k : range(AGE_MAX)) {
      df_salMoy.push_line(i,j+1900,k, salMoy_sga[i][j][k]);
  }
  
	
   // Réécrit la table emp
  for(Indiv& X : pop) for(auto age : range(X.ageMax)) {
      if (X.Id == 0) continue;
      if (!in(X.statuts[age],Statuts_occ))
        X.salaires[age] = 0;
      
      static Rdout df_emp("emp",{"Id","age","statut","salaire"});
      df_emp.push_line(X.Id,age,X.statuts[age],X.salaires[age]);
  }
  
}

//Note: la description qui suit permet de documenter le package R.

//' Fonction destinieSim
//' @description Calcule les droits retraites des individus.
//' Lance une simulation en utilisant la fonction SimDir. 
//' La fonction boucle sur chaque année de simulation. 
//' Pour chaque année elle boucle sur chaque individu pour 
//' le calcul des droits directs, puis boucle à nouveau sur
//'  chaque individu pour le calcul des droits indirects et 
//'  enfin boucle sur chaque individu pour le calculdu minimum
//'   vieillesse. L'avantage de la fonction destinieSim sur la 
//'   fonction destinieSimIndiv est qu'elle calcul les droits 
//'   indirects et le minimum vieillesse.
//' @return Retourne le meme environnement,
//' mais avec les droits retraites
//' @param envSim Environment 
//' @export
// [[Rcpp::export]]
void destinieSim(Environment envSim) {
  auto S = Simulation(envSim);
  
  // boucle sur chaque année

  for(int t=50; t <= AN_FIN; t++) {
        
      // Boucle pour les droits directs
      for(Indiv & X: pop) 
      if(X.est_present(t)) {
          if(X.age(t) >= 50 && !X.retr->totliq) {
              X.retr->SimDir(X.age(t));
          }
          X.retr->revaloDir(t); 
      }
      
      // Boucle pour les droits dérivés
      for(Indiv & X: pop) 
      if(X.est_present(t)) { //TODO: prendre en compte les situations remariage dans l'année
          int age = X.age(t);
          
          r_assert(X.conjoint[age] >= 0 && X.conjoint[age] <= int(pop.size()));
          Indiv& Y = pop[X.conjoint[age]];
          if(Y.est_present(t-1) && !Y.est_present(t) &&Y.emigrant==0) {
              X.retr->SimDer(Y,t);
          }
          else  if(t==AN_BASE &&X.conjoint[age]>0 && X.matri[age]==VEUF ) {
              X.retr->SimDer(Y,t);
          }
          X.retr->revaloDer(t);
      }
      
      // Boucle pour revalorisations et le minimum vieillesse
      for(Indiv& X: pop) 
      if(X.est_present(t)) {          
          // Test minimum vieillesse
          // sur les 65 ans et plus
          // personnes de référence du ménage
		  X.retr->min_vieil=0;
          if(X.age(t) >=65 && X.est_persRef(t))
              X.retr->minvieil(t);
      }

	  // Boucle pour calcul retraite nette
      for(Indiv& X: pop) 
      if(X.est_present(t)) {          
		X.retr->retraite_nette(t);
      }
      ecriture_retraites(t);
      if(t >= 50) {

          ecriture_indicateurs_an(t);
          ecriture_indicateurs_gen_age(t);
      }
                  
  }
  // Renvoie les résultats en retour de la fonction
  pointage("fin sim");
  ecriture_liquidations();
  
  
  auto cumEmp = make_vector3(AN_NB,AGE_MAX,3, Ratio());
  for(auto& X: pop) {
    int cum_emp = 0;
    for(int age : range(0,X.ageMax)) if(X.est_present(X.date(age))) {
      cum_emp += statut(X,age,Statuts_occ);
      cumEmp[X.anaiss%1900][age][X.sexe].push(true, cum_emp, 1); 
    }
  }
  static Rdout df0("Cumul_emploi",{"gen","age","sexe","txEmp"});
  
  for(int g = 0; g < AN_NB; g++)
    for(int age = 0; age < AGE_MAX; age++)
      for(auto sexe : {1,2})
        df0.push_line(g+1900,age,sexe,cumEmp[g][age][sexe]);
 ecriture_cotisations();
 ecriture_salairenet();

 ecriture_aide();
}

// [[Rcpp::export]]
void destinieIndiv(Environment envSim, int Id) {
  
  // importation des paramètres
  auto S = Simulation(envSim);
  
  options->ecrit_dr_test = true;
  Indiv& X = pop[Id];
  // boucle sur chaque age
  for(int age=50; age < X.ageMax; age++) 
  if(X.est_present(X.date(age))) {               
      if(age >= 50 && !X.retr->totliq) {
          X.retr->SimDir(age); // On teste la liquidation
      }
  }

  
}

//' Fonction destinieSimIndiv
//' @description Calcule les droits retraites de droit direct
//' (pas le minimum vieillesse ou les réversions) 
//' @return Retourne le meme environnement,
//' mais avec les droits directs uniquement.
//' @param envSim Environment 
//' @export
// [[Rcpp::export]]
void destinieSimIndiv(Environment envSim) {
  
  // importation des paramètres
  auto S = Simulation(envSim);
  pointage("findeb");
  // Pour les sorties
  
  // boucle sur chaque individu
  for(Indiv& X : pop) {
    // boucle sur chaque age
    for(int age=14; age < X.ageMax; age++) {
      if(X.est_present(X.date(age))) {
        int t = X.date(age);
        
        X.retr->revaloDir(t);
                      
        if(age >= 50 && !X.retr->totliq) {
            X.retr->SimDir(age); // On teste la liquidation
        }
      }
    }
  }
  
  
  ecriture_liquidations();
  
  
}


// [[Rcpp::export]]
void destinieSimAgefin(Environment envSim) {
  
  // importation des paramètres
  auto S = Simulation(envSim);
  
  // Pour les sorties
  const int AGE_MIN = 50*12;
  const int AGE_MAX = 80*12;
  const int GEN_MIN = 1940;
  const int GEN_MAX = 2011;
  const int AGE_NB = AGE_MAX - AGE_MIN;
  const int GEN_NB = GEN_MAX - GEN_MIN;
  
  auto retraites = make_vector3<Moyenne>(3, GEN_NB, AGE_NB);
  auto actifs    = make_vector3<Moyenne>(3, GEN_NB, AGE_NB);
  auto emplois   = make_vector3<Moyenne>(3, GEN_NB, AGE_NB);
  auto dates     = make_vector3<double>(3, GEN_NB, AGE_NB);
  static Rdout df_t("fincar",{"Id","gen","ageFinAn", "age","sexe","retraite","actif","emploi","statut"});
  static Rdout df_retraites("ind_agefin", {"sexe","gen","age","retraite","actif","emploi"});
  
  for(auto age : range(AGE_NB))
    for(auto j : range(GEN_NB))
      for(auto k : range(3))
        dates[k][j][age] = arr_mois(age,1/12.0);
  
  
  vector<int> mnaiss = vector<int>(AN_NB,0);
  for(Indiv& X : pop) {
    mnaiss[X.anaiss%1900] += 1;
    mnaiss[X.anaiss%1900] %= 12;
    X.moisnaiss = mnaiss[X.anaiss%1900];
  }
  
  // boucle sur chaque individu
  for(Indiv& X : pop) {
    // boucle sur chaque age
    for(auto age : range(14,X.ageMax)) {
      int t = X.date(age);
      if(!X.est_present(t))
        continue;
      
      if(age >= 50 && !X.retr->totliq) {
        X.retr->SimDir(age); // On test la liquidation
      }
    }
    // calcul des indicateurs
    for(auto age : range(50, min(80,X.ageMax)))
      if(X.anaiss >= 1940 && X.anaiss < 2011) 
      {
        double agefin_primoliq = X.retr->primoliq ? arr_mois(X.retr->primoliq->agefin_primoliq) : 999;
        for(auto mois : range(0, 12)) {
          int age_finan = int_mois(age,mois+X.moisnaiss);
          if(!X.est_present(X.date(age_finan))) continue;  
          
          bool ind_actif = in(X.statuts[age_finan],Statuts_act);
          bool ind_emploi = in(X.statuts[age_finan],Statuts_occ);
          bool ind_retraite = arr_mois(age,mois) >= agefin_primoliq;
          int s = X.sexe;
          int g = X.anaiss - GEN_MIN;
          int a = age*12+mois-AGE_MIN;
          retraites[s][g][a].push(ind_retraite);
          actifs[s][g][a].push(!ind_retraite && ind_actif);
          emplois[s][g][a].push(!ind_retraite && ind_emploi);
          int statut = ind_retraite ? 8 : X.statuts[age_finan];
          df_t.push_line(X.Id, X.anaiss, age_finan, age+mois/12.0, X.sexe, ind_retraite, ind_actif && !ind_retraite,  ind_emploi && !ind_retraite, statut);
        }
      }
  }
  
  
  for(auto i : range(AGE_NB))
    for(auto j : range(GEN_NB))
      for(auto k : range(3)) 
      {
        df_retraites.push_line(k,
        j+GEN_MIN, i+AGE_MIN, retraites[k][j][i],
        actifs[k][j][i], emplois[k][j][i]);
      }
      
  
}

/**
 * \struct Opt
 * \brief Structure utilisée pour coder destinieFinCar.
 */
struct Opt { bool _(redress_avpf);};

// [[Rcpp::export]]
void destinieFinCar(Environment envSim) {
  
  auto S = Simulation(envSim);
  auto option_sauv = make_shared<Options>(Rdin<Options>("options"));
  Transitions trans(envSim);
  Salaire sal(envSim);
  
  auto opt = Rdin<Opt>("options");
  if(opt.redress_avpf)  {
    for(int t : range(0,AN_NB))  
      sal.imput_avpf(t);
  }

  // Pour recoder les statuts pour sorties stats
  auto code_st = make_vector(700,0);
  code_st[  0] = 0 ;code_st[  1] = 1 ;code_st[  2] = 2 ;
  code_st[ 11] = 3 ;code_st[ 13] = 4 ;code_st[311] = 5 ;
  code_st[312] = 6 ;code_st[321] = 7 ;code_st[322] = 8 ;
  code_st[  4] = 9 ;code_st[ 51] = 10;code_st[ 52] = 11;
  code_st[  6] = 12;code_st[  9] = 13;code_st[ 63] = 14;
  code_st[621] = 15;code_st[623] = 16;code_st[624] = 17;
  code_st[  7] = 18;code_st[  8] = 19;
  

  // Pour sorties stats
  auto stats_statuts = make_vector4(3, AN_NB, AGE_MAX, 20, double(0));
  auto stats_statuts2 = make_vector4(3, AN_NB, AGE_MAX, 20, double(0));
  auto stats_statuts3 = make_vector4(3, AN_NB, AGE_MAX, 20, double(0));
  auto stats_statuts4 = make_vector4(3, AN_NB, AGE_MAX, 20, double(0));
  auto stats_statuts_cum = make_vector4(3, AN_NB, AGE_MAX, 20, double(0));


  // Recalcul des mois de naissance
  vector<int> mnaiss = vector<int>(AN_NB,0);
  for(auto& elt : mnaiss)
    elt = Multinom(alea(),vector<double>(12,1/12));
  for(Indiv& X : pop) {
    mnaiss[X.anaiss%1900] += 1;
    mnaiss[X.anaiss%1900] %= 12;
    X.moisnaiss = mnaiss[X.anaiss%1900];
  }
    
  auto moy_ageliq = vector<Moyenne>(AN_NB);
  
  // calcul des ages de liquidation des retraites
  // boucle sur chaque individu
  if(options->effet_hrzn_indiv || options->super_effet_hrzn || options->effet_hrzn) {
    for(Indiv& X : pop) {
      // on efface la retraite
      X.retr->primoliq.reset();
      X.retr->totliq.reset();
      X.retr->liq.reset();
      if(X.Id == 0) continue;
      
      //////////////////////////
      // Calcul législation 2009
      
      options->anLeg = 2011;
      options->pas1 = 1;
      options->pas2 = 1;
      options->comp = comp_tp;
      // boucle sur chaque age
      for(int age=14; age < 70; age++) 
        if(X.est_present(X.date(age))) 
          if(age >= 50 && !X.retr->totliq)
            X.retr->SimDir(age); // On test la liquidation

      X.age_exo = (X.retr->liq) ? X.retr->liq->agefin_primoliq : 0;

      // on efface la retraite
      X.retr->primoliq.reset();
      X.retr->totliq.reset();
      X.retr->liq.reset();
      
      //////////////////////////
      // Calcul législation 2014
      // On recommence
      
      options->anLeg = 2014;
      options->pas1 = 1;
      options->pas2 = 1;
      options->comp = comp_tp;
      // boucle sur chaque age
      for(int age=54; age < 72; age++) 
        if(X.est_present(X.date(age))) 
          if(age >= 54 && !X.retr->totliq)
            X.retr->SimDir(age); // On test la liquidation
      double ageliq = X.retr->liq ? X.retr->liq->agefin_primoliq : 0;
      moy_ageliq[X.anaiss%1900].push(ageliq,ageliq,1);
    }
    pointage("Fin calcul retraites");
  
    for(Indiv& X : pop)
      for(int age : range(0,X.ageMax))
        if(X.statuts[age]==2) X.statuts[age] = 1;      
    
    // Si effet hrzn macro: décalage de la génération  
    if(options->super_effet_hrzn)
      for(Indiv& X : pop)
        X.age_exo = moy_ageliq[X.anaiss%1900].resultat();      
    
    static auto df_ageliq = Rdout("ageliq",{"Id","ageexo"});
    for(Indiv& X : pop)
      df_ageliq.push_line(X.Id,X.age_exo);
  }
  
  if(options->effet_hrzn)
    for(Indiv& X : pop)
      if(X.anaiss >= 1951) {
        int agefinAct = 0;
        
        for(int age : range(min(70,X.ageMax))) 
          if(in(X.statuts[age],Statuts_act)) 
            agefinAct = age;
        
        if(agefinAct >= max(50,2010-X.anaiss)) {
          double decalage = 2 * partTx(X.anaiss, 1951, 1955);
          int agefinActNouv = agefinAct + int(decalage + alea());
          
          if(agefinAct+1 <= agefinActNouv && X.statuts[agefinAct+1] != 0) {
            X.statuts[agefinAct+1] = X.statuts[agefinAct];
            X.salaires[agefinAct+1] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+1)] / M->SMPT[X.date(agefinAct)];
          }
          if(agefinAct+2 <= agefinActNouv && X.statuts[agefinAct+2] != 0) {
            X.statuts[agefinAct+2] = X.statuts[agefinAct];
            X.salaires[agefinAct+2] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+2)] / M->SMPT[X.date(agefinAct)];
          }
          if(agefinAct+3 <= agefinActNouv && X.statuts[agefinAct+3] != 0) {
            X.statuts[agefinAct+3] = X.statuts[agefinAct];
            X.salaires[agefinAct+3] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+3)] / M->SMPT[X.date(agefinAct)];
          }
          if(agefinAct+4 <= agefinActNouv && X.statuts[agefinAct+4] != 0) {
            X.statuts[agefinAct+4] = X.statuts[agefinAct];
            X.salaires[agefinAct+4] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+4)] / M->SMPT[X.date(agefinAct)];
          }
        }
      }
  
   if(options->super_effet_hrzn || options->effet_hrzn_indiv)
    for(Indiv& X : pop)
      if(X.anaiss >= 1951) {
        int agefinAct = 0;
        
        for(int age : range(min(70,X.ageMax))) 
          if(in(X.statuts[age],Statuts_act)) 
            agefinAct = age;
        
        if(agefinAct >= max(50,2010-X.anaiss)) {
          double decalage = min_max(X.age_exo-60,0,7);
          int agefinActNouv = agefinAct + int(decalage + alea());
          
          if(agefinAct+1 <= agefinActNouv && X.statuts[agefinAct+1] != 0) {
            X.statuts[agefinAct+1] = X.statuts[agefinAct];
            X.salaires[agefinAct+1] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+1)] / M->SMPT[X.date(agefinAct)];
          }
          if(agefinAct+2 <= agefinActNouv && X.statuts[agefinAct+2] != 0) {
            X.statuts[agefinAct+2] = X.statuts[agefinAct];
            X.salaires[agefinAct+2] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+2)] / M->SMPT[X.date(agefinAct)];
          }
          if(agefinAct+3 <= agefinActNouv && X.statuts[agefinAct+3] != 0) {
            X.statuts[agefinAct+3] = X.statuts[agefinAct];
            X.salaires[agefinAct+3] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+3)] / M->SMPT[X.date(agefinAct)];
          }
          if(agefinAct+4 <= agefinActNouv && X.statuts[agefinAct+4] != 0) {
            X.statuts[agefinAct+4] = X.statuts[agefinAct];
            X.salaires[agefinAct+4] = X.salaires[agefinAct] * M->SMPT[X.date(agefinAct+4)] / M->SMPT[X.date(agefinAct)];
          }
        }
      }
  
  // Recalcul de l'âge de départ à la retraite   
  std::swap(options,option_sauv);

  // boucle sur chaque individu
  for(Indiv& X : pop) {
    // boucle sur chaque age
    for(auto age : range(14,X.ageMax)) 
    {
      int t = X.date(age);
      if(!X.est_present(t))
        continue;
      
      if(age >= 50 && !X.retr->totliq) {
        X.retr->SimDir(age); // On test la liquidation
      }
    }
    // calcul des indicateurs
    
    for(auto age : range(0, min(75,X.ageMax)))
    {
      double agefin_primoliq = X.retr->liq ? arr_mois(X.retr->liq->agefin_primoliq) : 999;
      for(auto mois : range(0, 12)) {
        int age_finan = int_mois(age,mois+X.moisnaiss); // age au 31/12
        if(!X.est_present(X.date(age_finan))) continue;  
      
        bool ind_retraite = arr_mois(age,mois) >= agefin_primoliq;
        int statut = ind_retraite ? 8 : X.statuts[age_finan];
        int t = int_mois(age+X.anaiss%1900,mois+X.moisnaiss);
        // année civile * age révolu
        stats_statuts[X.sexe][t][int_mois(age,mois)][code_st[statut]] += 1;
        // génération * age révolu
        stats_statuts2[X.sexe][X.anaiss%1900][int_mois(age,mois)][code_st[statut]] += 1;
        
        // année civile * age 31/12
        stats_statuts3[X.sexe][t][age_finan][code_st[statut]] += 1;
        // génération * age 31/12
        stats_statuts4[X.sexe][X.anaiss%1900][age_finan][code_st[statut]] += 1;
      }
    }
  } // fin boucle indiv
  
  static auto dfemp = Rdout("emp",{"Id","age","statut","salaire"});
  for(auto& X : pop)
  for(auto age : range(0,X.ageMax))
    if(X.statuts[age] != 0) {
      if(X.retr->liq && age >= X.retr->liq->ageprimoliq) X.statuts[age] = 8;
      dfemp.push_line(X.Id,age,X.statuts[age],X.salaires[age]);
    }
  
  
  ecriture_liquidations();


  pointage("fin calcul stats");
    
  static auto df_stats_st1 = Rdout("statuts1",{"sexe","annee","age","statut","effectif"});
  static auto df_stats_st2 = Rdout("statuts2",{"sexe","gen","age","statut","effectif"});
  static auto df_stats_st3 = Rdout("statuts3",{"sexe","annee","age","statut","effectif"});
  static auto df_stats_st4 = Rdout("statuts4",{"sexe","gen","age","statut","effectif"});
  for(auto i : range(3))
    for(auto j : range(AN_NB))
      for(auto k : range(AGE_MAX))
        for(auto l : range(20)) {
          df_stats_st1.push_line(i,j,k,l,stats_statuts[i][j][k][l]);
          df_stats_st2.push_line(i,j,k,l,stats_statuts2[i][j][k][l]);
          df_stats_st3.push_line(i,j,k,l,stats_statuts3[i][j][k][l]);
          df_stats_st4.push_line(i,j,k,l,stats_statuts4[i][j][k][l]);
        }
}

/**
 * \struct Option_optim
 * \brief Structure utilisée pour coder destinieSimOptim.
 */

struct Option_optim {
  bool _(min_prix), _(min_smpt), _(prix);
};

// [[Rcpp::export]]
void destinieSimOptim(Environment envSim) {
  auto S = Simulation(envSim);
  auto options_optim = Rdin<Option_optim>("options_optim");
  
  // Pour les sauvegardes à chaque itération
  auto primo = vector<ptr<DroitsRetr>>(pop.size(), NULL);
  auto liq = vector<ptr<DroitsRetr>>(pop.size(), NULL);
  auto tot = vector<ptr<DroitsRetr>>(pop.size(), NULL);
  
  // boucle sur chaque année
  auto mm_sal = vector<double>(AN_NB,0.0);
  auto mm_pens = vector<double>(AN_NB,0.0);
  
  
  
  for(int t=50; t <= 200; t++) {
    pointage(to_string(t));
    // Stocke la situation en t-1
    for(Indiv & X: pop) {
      primo[X.Id] = X.retr->primoliq;
      tot[X.Id] = X.retr->totliq;
      liq[X.Id] = X.retr->liq;
    }
    double cible_max = 1;
    double cible_min = 0;
    double min_revalo = options_optim.min_prix ? M->Prix[t] / M->Prix[t-1] :
                        options_optim.min_smpt ? M->SMPT[t] / M->SMPT[t-1] : 0;

    int nb_iter = (t > 115) ? 20 : 1; 
    for(int it : range(nb_iter) ) {
      mm_sal[t] = 0;
      mm_pens[t] = 0;
      Rcout << "*";
      if(t > 115) M->correct_demo[t] = (cible_max+cible_min)/2.0;
      if(t > 115 && it == nb_iter-1 && (options_optim.prix || options_optim.min_smpt)) M->correct_demo[t] = 
        (M->correct_demo[t-3] + M->correct_demo[t-2] + M->correct_demo[t-1]  + M->correct_demo[t] ) / 4.0;
      if(t > 115) M->RevaloRG[t] = max(M->SMPT[t] / M->SMPT[t-1] * M->correct_demo[t] / M->correct_demo[t-1],min_revalo);
      if(t > 115 && options_optim.min_smpt) M->RevaloRG[t] = min_revalo;
      if(t > 115 && options_optim.prix) M->RevaloRG[t] = M->Prix[t] / M->Prix[t-1];
      if(t > 115) M->RevaloCumRG[t] = M->RevaloCumRG[t-1] *  M->RevaloRG[t];
      if(t > 115) M->RevaloFP[t] = max(M->SMPT[t] / M->SMPT[t-1] * M->correct_demo[t] / M->correct_demo[t-1],min_revalo);
      if(t > 115 && options_optim.min_smpt) M->RevaloFP[t] = min_revalo;
      if(t > 115 && options_optim.prix) M->RevaloFP[t] = M->Prix[t] / M->Prix[t-1];
      if(t > 115) M->RevaloCumFP[t] = M->RevaloCumFP[t-1] *  M->RevaloFP[t];
      
      // Reprend la situation de t-1
      for(Indiv & X: pop) {
        X.retr->primoliq = primo[X.Id];
        X.retr->totliq = tot[X.Id];
        X.retr->liq = liq[X.Id];
      }
      
      // Boucle pour les droits directs
      for(Indiv & X: pop) 
      if(X.est_present(t)) {
          if(X.age(t) >= 50 && !X.retr->totliq)
              X.retr->SimDir(X.age(t));
           
          X.retr->revaloDir(t); 
      }
      
      // Boucle pour les droits dérivés
     // for(Indiv & X: pop) 
     // if(X.est_present(t)) { //TODO: prendre en compte les situations remariage dans l'année
     //     int age = X.age(t);
     //     
     //     r_assert(X.conjoint[age] >= 0 && X.conjoint[age] <= int(pop.size()));
     //     Indiv& Y = pop[X.conjoint[age]];
     //     if(Y.est_present(t-1) && !Y.est_present(t)) {
     //         X.retr->SimDer(Y,t);
     //     }
     //     else  if(t==AN_BASE && X.pseudo_conjoint > 0) {
     //         Indiv &Y = pop[X.pseudo_conjoint] ; 
     //         X.retr->SimDer(Y,t);
     //     }
     //     X.retr->revaloDer(t);
     // }
      
      // Boucle pour le minimum vieillesse
      for(Indiv& X: pop) 
      if(X.est_present(t)) {          
          // Test minimum vieillesse
          // sur les 65 ans et plus
          // personnes de référence du ménage
          if(X.age(t) >=65 && X.est_persRef(t))
              X.retr->minvieil(t);
          
          // Calcul des indicateurs
          mm_pens[t] += (X.retr->pension_rg + X.retr->pension_fp + X.retr->pension_in) * (1-X.retr->partavtliq(t));
          mm_sal[t] += X.salaires[X.age(t)] * X.retr->partavtliq(t);
      }
      if(t>115) {
        if(mm_pens[t]/mm_sal[t] > mm_pens[115]/mm_sal[115]) {
          cible_max = (cible_min+cible_max)/2.0;
        }
        else {
          cible_min = (cible_min+cible_max)/2.0;
        }
      }
      
      
      
      static auto df_iter = Rdout("iterOptim",{"annee","Masses sal","Masses pens","iter","correct_demo","revaloPens"});
      df_iter.push_line(t+1900,mm_sal[t],mm_pens[t],it,M->correct_demo[t],M->RevaloRG[t]);
      
    }
    
    //ecriture_retraites(t);
    if(t >= 50) {
        ecriture_indicateurs_an(t);
        ecriture_indicateurs_gen_age(t);
    }
                  
  }
  // Renvoie les résultats en retour de la fonction
  pointage("fin sim");
  ecriture_liquidations();
  
   
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