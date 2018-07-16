#include "Simulation.h"
#include "Indiv.h"
#include "Sante.h"

using namespace std;

int age_arrive(int sexe, bool estEnfant, int t) {
  vector<double> migadu_age_hom = {0, 27, ciblesDemo->Soldemig_H_27_30[t] , (30+1),ciblesDemo->Soldemig_H_31_35[t] , (35+1), ciblesDemo->Soldemig_H_36_40[t], (40+1), 
				   ciblesDemo->Soldemig_H_41_45[t], (45+1),ciblesDemo->Soldemig_H_46_50[t], (50+1), ciblesDemo->Soldemig_H_51_55[t], (55+1), ciblesDemo->Soldemig_H_56_60[t], (60+1)};
  vector<double> migadu_age_fem = {0, 25, ciblesDemo->Soldemig_F_25_29[t], 30, ciblesDemo->Soldemig_F_30_34[t], 35, ciblesDemo->Soldemig_F_35_39[t], 40, 
				   ciblesDemo->Soldemig_F_40_44[t], 45, ciblesDemo->Soldemig_F_45_49[t], 50, ciblesDemo->Soldemig_F_50_54[t], 55, ciblesDemo->Soldemig_F_55_59[t], 60};
  vector<double> migenf_age_hom = {0, 1, ciblesDemo->Soldemig_G_1_5[t], 6,ciblesDemo->Soldemig_G_6_11[t], 12,ciblesDemo->Soldemig_G_12_17[t] , 18}; 
  vector<double> migenf_age_fem = {0, 1, ciblesDemo->Soldemig_F_1_5[t], 6,ciblesDemo->Soldemig_F_6_11[t], 12,ciblesDemo->Soldemig_F_12_17[t] , 18}; 
  
  
  if(sexe==HOMME && !estEnfant) {
    return affn(alea(),migadu_age_hom);
  }
  else if(sexe==FEMME && !estEnfant) {
    return affn(alea(),migadu_age_fem);
  }
  else if(sexe==HOMME && estEnfant) {
    return affn(alea(),migenf_age_hom);
  }
  else {
    return affn(alea(),migenf_age_fem);
  } 
}

/*
void statut_migrants (Indiv& X, int t) {
  int age = X.age(t);

  
  if(X.sexe == HOMME && X.findet <= 18) {
    X.statuts[age] = multinomiale(alea(),mig_occ1_hom);
  }
  if(X.sexe == HOMME && X.findet <= 21) {
    X.statuts[age] = multinomiale(alea(),mig_occ2_hom);
  }
  if(X.sexe == HOMME && X.findet > 21) {
    X.statuts[age] = multinomiale(alea(),mig_occ3_hom);
  } 
  if(X.sexe == FEMME && X.findet <= 18) {
    X.statuts[age] = multinomiale(alea(),mig_occ1_fem);
  }
  if(X.sexe == FEMME && X.findet <= 21) {
    X.statuts[age] = multinomiale(alea(),mig_occ2_fem);
  }
  if(X.sexe == FEMME && X.findet > 21) {
    X.statuts[age] = multinomiale(alea(),mig_occ3_fem);
  }
}

*/


int findet_migrant(Indiv& X) {
  int findet=0;
  int generation=X.anaiss;
  double u1;
  double u2;
  double x1;
  if (X.mere==0) {
    if (X.sexe==HOMME) {
      findet=finEtudeMoy->homme[generation%1900]+alea();
    }
    else {
      findet=finEtudeMoy->femme[generation%1900]+alea();
    }
  }
  else {
    u1=alea();
    u2=alea();
    x1=sqrt(-2*log(u1))*cos(2*PI*u2);
    
    if (X.sexe==HOMME) {
      findet=min_max(int(20.70811+x1*2.806263+alea()),16,30);
    }
    else {
      findet=min_max(int(21.21043 +x1*2.796902+alea()),16,30);
    }
  }
  return findet;
}




void migrant (int t) {
  // ajout des migrants adultes hommes à la table population
  // liste_immigrant_vieux_H et liste_immigrant_vieux_F stockent les identifiants des migrants de plus de 50 ans
  // pour leur imputer un état de santé si l'option est activé
  vector<int> liste_immigrant_vieux_H;
  vector<int> liste_immigrant_vieux_F;
  int cible= ciblesDemo->Soldemig_H[t]/M->poids + alea();
  double ecart = ciblesDemo->Soldemig_H[t]/M->poids -cible;
  for(int i : range(cible)) {
    int age = age_arrive(HOMME,false,t);
    pop.emplace_back(pop.size(),HOMME,t+1900-age,18,age,0,0);
    Indiv& X = pop.back();
    X.findet = findet_migrant(X);
    X.neFrance=0;
    X.statuts[age]=999;
	  X.matri[age] = CELIB;
	  X.moisnaiss=X.Id%12;
	  X.dipl=(X.findet<16) ? SSDIPL : (X.findet==16) ? BREVET : (X.findet<18) ? CAP : (X.findet<20) ? BAC : UNIV;
    if ((age>=50) & options->sante ) liste_immigrant_vieux_H.push_back(X.Id);
  }
  
  // ajout des migrantes adultes à la table population et création d'une liste contenant leurs identifiants
  vector<int> liste_meres; // contient les identifiants des mères
  cible= ciblesDemo->Soldemig_Fe[t]/M->poids +ecart + alea();
  ecart = ciblesDemo->Soldemig_Fe[t]/M->poids -cible;
  for(int i : range(cible)) {
    int age = age_arrive(FEMME,false,t);
    pop.emplace_back(pop.size(),FEMME,t+1900-age,18,age,0,0);
    Indiv& X = pop.back();
    X.findet = findet_migrant(X);
    X.neFrance=0;
    X.statuts[age]=999;
	  X.matri[age] = CELIB;
	  X.moisnaiss=X.Id%12;
	  X.dipl=(X.findet<16) ? SSDIPL : (X.findet==16) ? BREVET : (X.findet<18) ? CAP : (X.findet<20) ? BAC : UNIV;
    if(age >= 20) {  // condition nécessairement vérifiée car l'âge d'une migrante adulte >=20
      liste_meres.push_back(X.Id);
    }
    if ((age>=50) & options->sante ) liste_immigrant_vieux_F.push_back(X.Id);
  }
  
  if (options->sante) {
    premiere_sante(t,liste_immigrant_vieux_H,HOMME);
    premiere_sante(t,liste_immigrant_vieux_F,FEMME);   
  }

  // Création d'un tableau d'enfants
  vector<int> liste_enfants; // contient les identifiants des enfants
  cible= ciblesDemo->Soldemig_G[t]/M->poids +ecart + alea();
  ecart = ciblesDemo->Soldemig_G[t]/M->poids -cible;
  for(int i : range(cible))  {
    int age = age_arrive(HOMME,true,t);
    pop.emplace_back(pop.size(),HOMME,t+1900-age,18,age,0,0);
    Indiv& X = pop.back();
    X.neFrance=0;
	  X.moisnaiss=X.Id%12;
	  X.dipl=BAC;
    liste_enfants.push_back(X.Id);
  }
  cible = ciblesDemo->Soldemig_Fi[t]/M->poids +ecart + alea();
  ecart = ciblesDemo->Soldemig_Fi[t]/M->poids -cible;
  for(int i : range(cible))  {
    int age = age_arrive(FEMME,true,t);
    pop.emplace_back(pop.size(),FEMME,t+1900-age,18,age,0,0);  
    Indiv& X = pop.back();
    X.neFrance=0;
	  X.moisnaiss=X.Id%12;
	  X.dipl=BAC;
    liste_enfants.push_back(X.Id);
  }
  
  
  // Boucle premier enfant
  for(int e : liste_enfants) {
    Indiv&X = pop[e]; 
    double score = 999999;
    int id_mere = 0;
    for(int m : liste_meres) {
      double score_tmp = X.anaiss - (pop[m].anaiss + pop[m].findet); // différence entre l'année de naissance de l'enfant et l'année de fin d'études de la potentielle mère
      if(score_tmp >=-1 && score_tmp < score && pop[m].nb_enf(pop[m].age(t),999) == 0) { // on vérifie que année de naissance de l'enfant > année de fin d'études de la potentielle mère et que la mère n'a pas d'enfants
        score = score_tmp;
        id_mere = m;
      }
    }
	
    if(id_mere != 0 && score <= 10 && ((X.anaiss-pop[id_mere].anaiss) <= 45)) {
      X.mere = id_mere;
      pop[id_mere].enf[0] = e;
      pop[id_mere].anaissEnf[0]=X.anaiss;
      //pop[id_mere].NbEnf+=1;
      // Inutile car l'attribut NbEnf n'existe pas
    }
  }
  
  // Boucle deuxième enfant
  int id_mere = 0;
  for(int e : liste_enfants) {
	Indiv& X = pop[e];
    if(pop[e].mere == 0) {
		alea_shuffle(liste_meres);
		id_mere = 0;
		for(int m : liste_meres) {
			if(pop[m].nb_enf(pop[m].age(t),999) == 1) {
			id_mere = m;
			break;
			}
		}
		if(id_mere != 0 && (pop[pop[id_mere].enf[0]].anaiss < X.anaiss) && (X.anaiss-pop[id_mere].anaiss<50)&& (X.anaiss-pop[id_mere].anaiss>=pop[id_mere].findet) ) {
			pop[e].mere = id_mere;
			pop[id_mere].enf[1] = e;
			pop[id_mere].anaissEnf[1] = X.anaiss;
		}
    }
  }
  // Boucle troisième enfant
  for(int e : liste_enfants) 
  { Indiv& X = pop[e]; 
	if(pop[e].mere == 0) {
		alea_shuffle(liste_meres);
		id_mere = 0;
		int score=9999;
		for(int m : liste_meres) {
			if(( pop[m].nb_enf(pop[m].age(t),999) == 2 )&&(pop[m].findet<score)) {
				  id_mere = m;
				  score = pop[m].findet;
			}
		}
		if(id_mere != 0 &&(X.anaiss-pop[id_mere].anaiss<50)&& (X.anaiss-pop[id_mere].anaiss>=pop[id_mere].findet) && pop[pop[id_mere].enf[1]].anaiss < X.anaiss  ) {
			X.mere = id_mere;
			pop[id_mere].enf[2] = e;
			pop[id_mere].anaissEnf[2]=X.anaiss;
		}
    }
  }
  for(int e : liste_enfants) {
	Indiv&X = pop[e];
    X.findet = findet_migrant(X);
    X.statuts[X.age(t)]=(X.age(t)<=X.findet)? S_SCO:999;
	X.matri[X.age(t)] = CELIB;
  }
  
  
  
  
  
  /// Emigration entre 18 et 26 ans
  vector<int> vecteur_H; 
  vector<double> probas_H;
  vector<int> vecteur_F; 
  vector<double> probas_F;
  int compteur=0;
  for(Indiv& Y : pop) {
    if(Y.age(t)>=18&&Y.age(t)<=26&& Y.sexe==HOMME &&Y.est_present(t) )
      { compteur++;
	vecteur_H.push_back(Y.Id);
	probas_H.push_back(	Y.age(t)==18 ? ciblesDemo->proba_H_18[t] :
				Y.age(t)==19 ? ciblesDemo->proba_H_19[t] :
				Y.age(t)==20 ? ciblesDemo->proba_H_20[t] :
				Y.age(t)==21 ? ciblesDemo->proba_H_21[t] :
				Y.age(t)==22 ? ciblesDemo->proba_H_22[t] :
				Y.age(t)==23 ? ciblesDemo->proba_H_23[t] :
				Y.age(t)==24 ? ciblesDemo->proba_H_24[t] :
				Y.age(t)==25 ? ciblesDemo->proba_H_25[t] : ciblesDemo->proba_H_26[t]);
	
      }
    if(Y.age(t)>=18&&Y.age(t)<=25&& Y.sexe==FEMME &&Y.est_present(t) )
      { compteur++;
	vecteur_F.push_back(Y.Id);
	probas_F.push_back(	Y.age(t)==18 ? ciblesDemo->proba_F_18[t] :
				Y.age(t)==19 ? ciblesDemo->proba_F_19[t] :
				Y.age(t)==20 ? ciblesDemo->proba_F_20[t] :
				Y.age(t)==21 ? ciblesDemo->proba_F_21[t] :
				Y.age(t)==22 ? ciblesDemo->proba_F_22[t] :
				Y.age(t)==23 ? ciblesDemo->proba_F_23[t] :
				Y.age(t)==24 ? ciblesDemo->proba_F_24[t] : ciblesDemo->proba_F_25[t] );
	
      }
  }
  cible = ciblesDemo->nbre_emig_h[t]/M->poids -ecart + alea();
  ecart = ciblesDemo->nbre_emig_h[t]/M->poids -cible;
  auto liste_emigrant_H=tirage(vecteur_H,probas_H,TIRAGE_NB,cible, "Emigrants hommes");
  cible= ciblesDemo->nbre_emig_f[t]/M->poids +ecart + alea();
  ecart = ciblesDemo->nbre_emig_f[t]/M->poids -cible; // non utilisé par la suite
  auto liste_emigrant_F=tirage(vecteur_F,probas_F,TIRAGE_NB,cible, "Emigrants femmes");
  
  for (int e : range(liste_emigrant_H.size())) {
    Indiv& Y = pop[vecteur_H[liste_emigrant_H[e]]];
    Y.ageMax=Y.ageMax-1;
    Y.statuts[Y.age(t)] = 0;
    Y.matri[Y.age(t)] = 0;
    Y.conjoint[Y.age(t)] = 0;
	Indiv& X=pop[Y.conjoint[Y.age(t)-1]];
    if(Y.matri[Y.age(t)-1]==MARIE) {
      X.matri[X.age(t)] = SEPAR; } 
    Y.emigrant= 1 ;
  }
  
  for (int e : range(liste_emigrant_F.size())) {
    Indiv& Y = pop[vecteur_F[liste_emigrant_F[e]]];
    Y.ageMax=Y.ageMax-1;
    Y.statuts[Y.age(t)] = 0;
    Y.matri[Y.age(t)] = 0;
    Y.conjoint[Y.age(t)] = 0;
	Indiv& X=pop[Y.conjoint[Y.age(t)-1]];
    if(Y.matri[Y.age(t)-1]==MARIE) {
      X.matri[X.age(t)] = SEPAR; } 
    Y.emigrant= 1 ;
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