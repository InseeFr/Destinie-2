#include "Simulation.h"
#include "Indiv.h"
#include "OutilsBase.h"


int duree_etat(Indiv& X, int age) {
  int conjoint = X.conjoint[age];
  int matri = X.matri[age];
  int duree = 0;
  int compteurAge = age-1;
  while (compteurAge >=0 && X.conjoint[compteurAge] == conjoint && X.matri[compteurAge]==matri) {
	  duree++;
	  compteurAge--;
  }
  return duree;
}
 

int NEnfunion(Indiv& X, int age) {
  int compteur_en_cours=0;
  for (int e : X.enf) {
    if(e > 0) {
      if (X.sexe==HOMME && (pop[e].mere==X.conjoint[age])) {
        compteur_en_cours++;
      }
      else {
        if (X.sexe==FEMME  && (pop[e].pere==X.conjoint[age])) {
          compteur_en_cours++;
        }
      }
    }
  }
  return compteur_en_cours;
} 

double sigmoid(double x) {
  return 1/(1+exp(-x));
}

void separation (int t) {
  auto probas = vector<double>(pop.size(),0.0);
  for (Indiv& X : pop) {
    if ((X.est_present(t)) && (X.sexe == FEMME) && (X.matri[X.age(t)] == MARIE)) {
      int age = X.age(t);
      int nEnfunion = NEnfunion(X,age);
      probas[X.Id]= sigmoid(
                           -2.47 
                           -0.06*duree_etat(X,age)
                           -0.04*(age-duree_etat(X,age))
                           +0.58*(nEnfunion==0)
                           +0.21*(nEnfunion==1)
                           +0.13*(nEnfunion>=4)
                           +0.41*(X.nb_enf(age,999)-nEnfunion>=1)
                           +0.09*(X.findet>=16 && X.findet<=20)
                           -0.45);
    }
  }
  // tirage des identifiants de femmes se séparant en t
  auto liste_femmes=tirage(probas, TIRAGE_SIMPLE, 0, "Separation");
  
  for (int i : range(liste_femmes.size()))  {
    int e = liste_femmes[i];
    Indiv& X=pop[e];
    int age = X.age(t);
    Indiv& Y=pop[X.conjoint[age]];
    X.matri[age]=4;
    X.conjoint[age]=0 ;
    Y.matri[Y.age(t)]=4;
    Y.conjoint[Y.age(t)]=0;
  }
}


void mise_en_couple (int t) {
  double agetrend;
  double probas;
  vector<double> probas_F;
  vector<double> probas_H;
  vector<int> liste_H;
  vector<int> liste_F;

  
  // [Début création] On crée deux tableaux contenant les identifiants des personnes non mariées, l'un pour les hommes, l'autre pour les femmes, ainsi que deux tableaux contenant leurs probabilités estimées respectives de connaître une union.
  for (Indiv& X : pop) {
    int age = X.age(t);
	// On crée deux tableaux contenant les célibataires hommes et femmes, et deux autres tableaux contenant leurs probabilités estimées respectives de première mise en couple.
    if (X.est_present(t) && (X.matri[age] == 1) && (age>15) && (age>=min(X.findet-4,21))) {
	// Début du calcul de l'âge corrigé servant à estimer la proba de première mise en couple
      if (X.findet<=25) {
        if (X.findet >= 16) {
          agetrend=age-(X.findet-16)*0.5;
        }
        else {
          agetrend=age;
        }
      }
      if (X.findet>25) {
        agetrend=age-4.5;
      } // Fin

      if (X.sexe == FEMME) {
		// proba de première union pour une femme
        probas= 1/(1+exp(-(-68.49 + 9.06*agetrend - 4.41e-1*pow(agetrend,2)
                             + 9.14e-3*pow(agetrend,3)-6.91e-5*pow(agetrend,4)-0.06*(X.findet>=21 && X.findet<=30) +0.133)));
        probas_F.push_back(probas);
        liste_F.push_back(X.Id);
        //champs_F[X.Id-1]=1;
      }
      else {
		// proba de première union pour un homme
        probas=1/(1+exp(-(-73.10 + 8.96*agetrend - 4.08e-1*pow(agetrend,2) + 7.96e-3*pow(agetrend,3) 
                            -5.72e-5*pow(agetrend,4)-0.13*(X.findet>=16 && X.findet<=20) 
                            +0.04*(X.findet>=21 && X.findet<=30) +0.264)));
        probas_H.push_back(probas);
        liste_H.push_back(X.Id);
      }
    }
	
	// On ajoute aux deux tableaux précédents de célibataires les personnes veuves ou séparées, et aux deux tableaux de probabilités leurs probabilités estimées respectives de remise en couple.
    if (X.est_present(t) && (X.matri[age] > 2)) {
      if (X.sexe == FEMME) {
		// proba de remise en couple
        probas= (1/(1+exp(-(0.33 - 0.10*duree_etat(X,age) +0.14 *(duree_etat(X,age)==0) -0.08*(age-duree_etat(X,age))
                              +0.41*(X.nb_enf(age,999)==0) -0.57*(X.matri[age]==3) +0.01))));
        probas_F.push_back(probas);
        liste_F.push_back(X.Id);
      }
      else {
		// proba de remise en couple
        probas= (1/(1+exp(-(-0.25 - 0.09*duree_etat(X,age) +0.19 *(duree_etat(X,age)==0) -0.05*(age-duree_etat(X,age))
                              +0.41*(X.nb_enf(age,999)==0) -0.19*(X.findet>=16 && X.findet<=20) -0.2))));
        probas_H.push_back(probas);
        liste_H.push_back(X.Id);
      }
    }
  } // [Fin création]
  
  
  // On sélectionne les personnes qui connaissent effectivement une union. D'un côté les femmes, de l'autre les hommes.
  auto liste_femmes=tirage(liste_F,probas_F,TIRAGE_SIMPLE,0, "Mise en couple pour femmes");
  
  vector<int> liste_F_amarier;
  vector<int> liste_H_amarier;
  
  // A ce stade, liste_femmes contient les indices (et non les identifiants) des femmes connaissant une union. On remplace donc les indices par les identifiants. Idem pour les hommes.
  for (int i : liste_femmes) {
    liste_F_amarier.push_back(liste_F[i]);
  }

  auto liste_hommes=tirage(liste_H,probas_H,TIRAGE_NB,liste_femmes.size(), "Mise en couple pour hommes");
  for (int i : liste_hommes) {
    liste_H_amarier.push_back(liste_H[i]);
  }
  // A présent, liste_F_amarier et liste_H_amarier contiennent les identifiants des personnes entrant en union.
  
  // On forme les couples
  std::random_shuffle (liste_H_amarier.begin(), liste_H_amarier.end());
  auto score = vector<double>(liste_hommes.size(),0.0);
  int nb_mari=0;
  
  // On trouve un mari à chaque femme.
  
  for (int e : indices(liste_F_amarier)) {
    Indiv& X=pop[liste_F_amarier[e]];
    int indiceMari=0;
    double score_tmp = 0;
    double score=999999;
    
      /*for (int i : range(liste_hommes.size())) {
        Indiv& Y=pop[vecteur_H[liste_hommes[i]]];
        score_tmp=Y.age(t)-X.age(t)- (0.25*(Y.age(t)-20)*(Y.age(t)>=20 && Y.age(t)<=40)+5*pow((Y.age(t)>=41 && Y.age(t)<=103),2) +pow((Y.findet-X.findet),2));
        if(score_tmp > 0 && score_tmp >= score) {
          score = score_tmp;
          id_mari = vecteur_H[liste_hommes[i]];
        }
      }*/
	  
      for (int i: indices(liste_H_amarier)) {
        Indiv& Y=pop[liste_H_amarier[i]];
        score_tmp=pow(Y.age(t)-X.age(t)- 0.25*min_max(Y.age(t)-20,0,20),2) +pow((Y.findet-X.findet),2);
        if(score_tmp <= score) {
          score = score_tmp;
          indiceMari=i;
        }
      }
	  int id_mari = (liste_H_amarier.size()!=0)?liste_H_amarier[indiceMari] : 0 ;
	  Indiv& MARI = pop[id_mari];
      if (abs(MARI.age(t)- X.age(t))<21) {
        X.matri[X.age(t)] = 2;
        MARI.matri[MARI.age(t)] = 2;
        MARI.conjoint[MARI.age(t)] = X.Id;
        X.conjoint[X.age(t)] = id_mari;
        nb_mari++;
		liste_H_amarier.erase(liste_H_amarier.begin() + indiceMari);
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