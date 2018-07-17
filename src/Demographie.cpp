#include "Simulation.h"
#include "Indiv.h"
#include "Migration.h"
#include "Naissance.h"
#include "Mortalite.h"
#include "Separations.h"
#include "Sante.h"

enum {CAT_ACTIVE=1,CAT_SEDENTAIRE};

/**
 * \fn calcul_catFP(Indiv& X)
 * \brief Fonction utilisée sur les individus de l'échantillon de départ afin de déterminer la catégorie (active ou sédentaire) pour
 *  les titulaires FP. Celle-ci est déterminée à partir de l'âge de fin d'études, des durées dans le privé, le public et au chômage, et d'un âge
 *  de liquidation imputé.
 */
int calcul_catFP(Indiv& X) { 
      
  // calcul variables retrospectives 
  int ageret(0), durpub(0),  duremp_priv(0), durcho(0);
  for(int age : range(0,X.ageMax)) {
    if(X.statuts[age]==S_RET && ageret==0) ageret = age; 
    else if(X.statuts[age] == S_NC) duremp_priv++;
    else if(X.statuts[age] == S_NONTIT) duremp_priv++;
    else if(X.statuts[age] == S_IND) duremp_priv++;
    else if(X.statuts[age] == 3) durpub++;
    else if(X.statuts[age] == S_CHOBIT) durcho++;
    else if(X.statuts[age] == S_CHONONBIT) durcho++;
  }
  
  if(ageret > 0 && ageret < 60) {

    // carrières longues
    bool carlongues = 
        ((duremp_priv + durpub + durcho) >= 42 && (duremp_priv + durpub) >= 42 && (X.findet + 1) <= 16 && ageret >= 56) ||
        ((duremp_priv + durpub + durcho) >= 42 && (duremp_priv + durpub) >= 41 && (X.findet + 1) <= 16 && ageret >= 58) ||
        ((duremp_priv + durpub + durcho) >= 42 && (duremp_priv + durpub) >= 40 && (X.findet + 1) <= 17 && ageret >= 59);
    // retraite anticipée pour 3 enf
    bool retAnt3enf = X.sexe==FEMME && durpub >= 15 && X.enf[2] > 0;
    return (carlongues || retAnt3enf) ? CAT_SEDENTAIRE : CAT_ACTIVE; 
  }
  else {
    return (alea() > 0.21) ? CAT_SEDENTAIRE : CAT_ACTIVE; 
  }
  return 0;
}

/**
 * \fn imput_statut(Indiv& X, int age)
 * \brief Fonction appelée par imputations_base() afin de compléter les trous de carrière liés à l'âge de fin d'études. imput_statut renvoie
 * le premier statut, hors scolarité, de l'individu X après l'âge age. La valeur par défaut est le statut non-cadre du privé.
 */
int imput_statut(Indiv& X, int age) {
  for(int k : range(age+1,X.ageMax)) {
    if(!in(X.statuts[k],{0,999,S_SCO})) return X.statuts[k];
  }
  return S_NC; // salarié du privé par défaut
}

/*int dernier_statut_emploi(Indiv& X, int age) { // rmq : jamais utilisée
  for(int a : range(1,age)) {
    if(in(X.statuts[age-a],{S_NC,S_NONTIT,S_IND,S_FPAE,S_FPATH,S_FPSE,S_FPSTH,S_INVALRG,S_INVALFP,S_INVALIND})) 
      return X.statuts[age];
  }
  return S_NC;
} */
/*
int age_arrivee(Indiv& X) {
  if (X.neFrance==0){
    int age=X.age(AN_BASE);
    for(int a : range(1,age+1)) {
      if(X.statuts[age-a]==0) 
      {return (age-a+1);}
      }
  }
  else return 0;
}*/

/**
 * \fn imputations_base()
 * \brief Fonction appelée par destinieDemographie avant la simulation des événements démographiques et familiaux, afin de réaliser des opérations
 *  élémentaires (imputations, mise en forme) sur l'échantillon de départ. Parmi ces operations :
 *       - tri du vecteur des enfants par année de naissance
 *       - écrêtement du nombre d'enfants par individus à NB_ENF_MAX (qui vaut 6 en pratique)
 *       - mise en forme des statuts antérieurs à l'âge de fin d'études
 *       - imputation du cumul emploi-études lorsque age==X.findet
 *       - pour les statuts manquants après l'âge de sortie des études et jusqu'au décès, prolongation du dernier statut hors scolarité
 *       - affinage du statut FP selon la catégorie (active ou sédentaire), et du statut invalide selon le secteur (privé, public, indépendant)
 *       - attribution d'un pseudo-conjoint aux personnes veuves en 2010 afin d'initialiser le stock de réversataires. Nécessaire en l'absence,
 *       dans l'enquête Patrimoine, d'informations sur les liens conjuguaux avant 2010
 */
void imputations_base() {
  
  for(Indiv& X : pop) if(X.Id != 0){
    r_assert(X.date(X.ageMax)==AN_BASE+1);
    
    // Reordonne les enfants
    int nbEnf = X.nb_enf(X.age(AN_BASE+1));
    if(nbEnf > 0) {
      auto enf = vector<int>(begin(X.enf),begin(X.enf)+nbEnf);
      auto anaissEnf = vector<int>(begin(X.anaissEnf),begin(X.anaissEnf)+nbEnf);
      order(enf,anaissEnf);
      for(int e : range(NB_ENF_MAX)) {
        X.enf[e] = e < nbEnf ? enf[e] : 0;
        X.anaissEnf[e] = e < nbEnf ? pop[X.enf[e]].anaiss : 0;
      }
    }
    
    // impute les statuts manquants et prolonge les carrières durant la retraite
    for(int age : range(0,X.ageMax)) {
      if(age < X.findet && X.statuts[age] == 999) {
        X.statuts[age] = S_SCO;
      }
      else if(age == X.findet && X.statuts[age]==999) {
        // impute les trous de carrière liés à l'âge de fin d'étude
        bool cumEmpEtude = alea() < 0.75 * partTx(age,16,30) * partTx(X.anaiss,1950,1990);
        X.statuts[age] = cumEmpEtude ? imput_statut(X,age) : S_SCO ;
      }
      else if(X.statuts[age]==999) {
        // impute les trous de carrière liés à l'âge de fin d'étude
        X.statuts[age] = !in(X.statuts[age-1],{0,999,63}) ? X.statuts[age-1] : imput_statut(X,age);
      }
    }
    
    // affine les codes de statut : recode la FP et la maladie/invalidite
    int catFP = 0;
	//int age_arr=age_arrivee(X);
    for(int age : range(0,X.ageMax)) {
      if(age < X.findet&&X.statuts[age] !=0) X.statuts[age] = S_SCO;
      else if(X.statuts[age] == 3) {
        X.typeFP = (X.typeFP != NONFP) ? X.typeFP : (alea() <= 0.2) ? FPTH : FPE; // Rmq 15/02/2017 : inutile dorénavant
        catFP = catFP ? catFP : calcul_catFP(X);
        X.statuts[age] = (X.typeFP == FPE) ? 
            (catFP == CAT_ACTIVE ? S_FPAE : S_FPSE) : 
            (catFP == CAT_ACTIVE ? S_FPATH : S_FPSTH);
      }
      else if(X.statuts[age] == 62) {
        X.statuts[age] = in(X.statuts[age-1],{S_FPAE,S_FPATH,S_FPSE,S_FPSTH,S_INVALFP}) ? S_INVALFP :
                         in(X.statuts[age-1],{S_IND,S_INVALIND}) ? S_INVALIND :
                            S_INVALRG;
      }
      else if(X.statuts[age]==S_RET) {
        // Prolonge retraite
        r_assert(age >1); 
        X.statuts[age] = X.statuts[age-1];
      }
    }
  }
  
  // Pseudo-conjoints
  deque<array<double,3>> veufs, maries, veuves, mariees;
  
  for(Indiv& X : pop) {
    int age = X.age(AN_BASE);
    // remarque : on ajoute un aléa pour éviter des scores identiques
    double anaiss = (X.anaiss - (X.sexe == HOMME) * 2 + alea()/10.0)/AN_BASE; 
    double findet = partTx(X.findet + alea()/10.0,16,25)/AN_BASE;
    array<double,3> p_score = {double(X.Id),findet,anaiss};
    if(X.matri[age] == MARIE && X.sexe==FEMME) mariees.push_back(p_score);
    else if(X.matri[age] == MARIE && X.sexe==HOMME) maries.push_back(p_score);
    else if(X.matri[age] == VEUF && X.sexe==FEMME) veuves.push_back(p_score);
    else if(X.matri[age] == VEUF && X.sexe==HOMME) veufs.push_back(p_score);
  }
  
  auto pseudos_conjoints1 = appariement<3>(veuves,maries,{0.8,0.2},AVEC_REMISE);
  auto pseudos_conjoints2 = appariement<3>(veufs,mariees,{0.8,0.2},AVEC_REMISE);
  for(auto q : {&pseudos_conjoints1,&pseudos_conjoints2})
  for(auto v : *q) {
    if (v[1] != -1) pop[v[0]].pseudo_conjoint = v[1]; 
  }
  
   if (options->sante){
	auto indiv_par_sexe_cage = make_vector2(3,9,vector<int>());
	for(Indiv& X : pop) {
		int age = X.age(AN_BASE);
		if (age>=50){
			int sexe = X.sexe;
			int cage= (age<=54) ? 0 : 
					(age<=59) ? 1 : 
					(age<=64) ? 2 : 
					(age<=69) ? 3 : 
					(age<=74) ? 4 :
					(age<=79) ? 5 : 
					(age<=84) ? 6 : 
					(age<=89) ? 7 : 8 ;
			indiv_par_sexe_cage[sexe][cage].push_back(X.Id);
		}
	}
	for(int sexe : {HOMME,FEMME}) {
		for (int cage : range(0,9)) {
		premiere_sante(AN_BASE,indiv_par_sexe_cage[sexe][cage],sexe);
		}
	}	
  } 
}

//' Fonction destinieDemographie
//' @description Simule en projection les évènements démographiques des individus:
//' décès, immigration, mise en couple, naissances 
//' @return Retourne le meme environnement,
//' mais projeté jusqu'à la fin de la simulation
//' @param envSim Environment 
//' @export
// [[Rcpp::export]]
void destinieDemographie(Environment env) {
  auto S = Simulation(env); // Importation de l'échantillon et des paramètres macro
                            // tables: ech, emp, fam, macro, survie, options

  Rcout<<"Taux de ponderation:"<<M->poids<<endl;
  imputations_base();
  pointage("fin sim redress");

  int max_sim=(options->AN_MAX%1900)+1;
  
  
  if (options->sante) { 
    for(int t : range(AN_BASE+1,max_sim)) {
      mortalite(t,false,true,options->sante,false); //void mortalite(int t, bool option_mort_diff, bool option_mort_tirageCale,bool option_sante=false)
      imputsante(t);
      migrant(t);
      separation(t);
      mise_en_couple(t);
      naissance(t,options->tirage_simple_naiss);
    }
  }
  else {
    for(int t : range(AN_BASE+1,max_sim)) {
      mortalite(t,options->mort_diff,true,options->sante,options->mort_diff_dip);
      migrant(t);
      separation(t);
      mise_en_couple(t);
      naissance(t,options->tirage_simple_naiss);
	}
  } 
  // sortie des résultats
  static auto df_ech = Rdout("ech",{"Id","sexe","anaiss","findet","ageMax","mere","pere","neFrance","emigrant","moisnaiss","taux_prim","typeFP","k","enf1","enf2","enf3","enf4","enf5","enf6","pseudo_conjoint","peudip","tresdip","dipl"});
  for(Indiv& X : pop) {
    if(X.Id!=0)
    df_ech.push_line(X.Id,X.sexe,X.anaiss,X.findet,X.ageMax,X.mere,X.pere,X.neFrance,X.emigrant,X.moisnaiss,X.taux_prim,X.typeFP,X.k,X.enf[0],X.enf[1],X.enf[2],X.enf[3],X.enf[4],X.enf[5],X.pseudo_conjoint,X.peudip,X.tresdip,X.dipl);
  }

  if (options->sante){
  static auto df_sante = Rdout("sante",{"Id","age","sante"});
  for(Indiv& X : pop) {
    for(int age : range(50,X.ageMax))
      df_sante.push_line(X.Id,age,X.sante[age]);
  }
  }
  static auto df_emp = Rdout("emp",{"Id","age","statut","salaire"});
  for(Indiv& X : pop) {
    for(int age : range(X.ageMax)) {
      df_emp.push_line(X.Id,age,X.statuts[age],X.salaires[age]);
	  }
  }

  static auto df_fam = Rdout("fam",{"Id","annee","enf1","enf2","enf3","enf4","enf5","enf6","conjoint","matri","pere","mere"});
  for(Indiv& X : pop) {
    if(X.Id!=0){
	if (X.ageMax==0)
		df_fam.push_line(X.Id,X.date(0)+1900,X.enf[0],X.enf[1],X.enf[2],X.enf[3],X.enf[4],X.enf[5],X.conjoint[0],X.matri[0],X.pere,X.mere); 
    for(int age : range(max(0,X.age(109)),X.ageMax))
      //if(age == 0 || X.date(age)==109 || X.conjoint[age] != X.conjoint[age-1] ||
        //    X.matri[age] != X.matri[age-1]) 
		if(X.est_present(X.date(age)))
			{
        if(X.date(age)==109 && X.pseudo_conjoint != 0) 
          df_fam.push_line(X.Id,X.date(age)+1900,X.enf[0],X.enf[1],X.enf[2],X.enf[3],X.enf[4],X.enf[5],X.pseudo_conjoint,X.matri[age],X.pere,X.mere);
        else
          df_fam.push_line(X.Id,X.date(age)+1900,X.enf[0],X.enf[1],X.enf[2],X.enf[3],X.enf[4],X.enf[5],X.conjoint[age],X.matri[age],X.pere,X.mere);
      }
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