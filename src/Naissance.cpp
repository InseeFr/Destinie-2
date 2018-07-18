#include "Simulation.h"
#include "Indiv.h"
# include "Separations.h"
# include "OutilsBase.h"
using namespace std;


/** \struct ordre_findet
 * \brief Compare les individus en fonction de leur âge de fin d'étude
 */
struct ordre_findet {
  inline bool operator() (const int c1,const int c2) const
  { Indiv& X1=pop[c1];
    Indiv& X2=pop[c2];
    return X1.findet < X2.findet;
  };
};

void naissance (int t, bool option_tirageNonCale) {
  
  int parite_mere=0;
  int parite_pere=0;
  int duree;
  auto proba = vector<double>(pop.size(),0.0);
  for (Indiv& X : pop) {
    int age = X.age(t);
    int duretat= duree_etat(X,age);
    Indiv& W = pop[X.conjoint[age]];
    if ((X.est_present(t))&& (X.sexe == FEMME) && (age>=16) && (age<=45) && (age>X.findet)&&(X.matri[age]==2 )) 
      {
      parite_mere=X.nb_enf(age,999);
      parite_pere=W.nb_enf(W.age(t),999);
      if ((parite_mere<6) && (parite_pere<6)) {
        if (parite_mere == 0 && X.findet>=16 && X.findet<=20) {
          if (age-X.findet>=duretat) {
            duree=duretat;
          }
          else {
            duree=age-X.findet;
          }
          proba[X.Id]=(1/(1+exp(-(14.12-0.11*duree-1.72*(duree == 0)-0.62*(duree == 1)-0.22*(duree == 2)
                                  -0.13*(duree == 3)-1.72*age+6.6e-2*pow(age,2)- 8.3e-4*pow(age,3)-0.15))));
        }
        if (parite_mere == 0 && X.findet>=21 && X.findet<=30) { 
          if (age-X.findet>=duretat) {
            duree=duretat;
          }
          else {
            duree=age-X.findet;
          }
          proba[X.Id]= (1/(1+exp(-(-9.86-0.12*duree-2.20*(duree == 0)-1.28*(duree == 1)-0.68*(duree == 2)
                                  -0.30*(duree == 3)+0.62*age-1.02e-2*pow(age,2)+0.12*age-4.2))));
        }
        if (parite_mere == 1 && pop[X.enf[0]].pere == X.conjoint[age] && X.findet>=16 && X.findet<=20) {
          duree=pop[X.enf[0]].age(t);
          proba[X.Id]= (1/(1+exp(-(-3.70-0.24*duree-2.56*(duree == 1)-1.08*(duree == 2)-0.19*(duree == 3)
                                  +0.33*age-0.7e-2*pow(age,2)+0.01))));
        }
        if (parite_mere == 1 && pop[X.enf[0]].pere == X.conjoint[age] && X.findet>=21 && X.findet<=30) {
          duree=pop[X.enf[0]].age(t);
          proba[X.Id]= (1/(1+exp(-(-8.19-0.31*duree-3.14*(duree == 1)-1.13*(duree == 2)-0.28*(duree == 3)
                                  +0.61*age-1.05e-2*pow(age,2)+0.39))));
        }
        if (parite_mere == 2 && pop[X.enf[1]].pere == X.conjoint[age]) {
          duree=pop[X.enf[1]].age(t);
          proba[X.Id]= (1/(1+exp(-(3.44-0.16*duree-2.05*(duree == 1)-0.91*(duree == 2)
                                  -0.17*age+0.49*(X.findet>=21 && X.findet<=30)+0.37))));
        }
        if (parite_mere == 3 && pop[X.enf[2]].pere == X.conjoint[age]) {
          duree=pop[X.enf[2]].age(t);
          proba[X.Id]= (1/(1+exp(-(3.06-0.12*duree-1.54*(duree == 1)-0.36*(duree == 2)-0.17*age+0.20))));
        }
        if ((parite_mere == 4) &&  pop[X.enf[3]].pere == X.conjoint[age]) {
          duree=pop[X.enf[3]].age(t);
          proba[X.Id]= (1/(1+exp(-(3.22-0.13*duree-1.64*(duree == 1)-0.17*age+0.40))));
        }
        if (parite_mere == 5 && pop[X.enf[4]].pere == X.conjoint[age]) {
          duree=pop[X.enf[4]].age(t);
          proba[X.Id]= (1/(1+exp(-(0.75+1.15*(duree == 2)+1.05*(duree == 3)-0.12*age+0.35))));
        } 
        if (parite_mere == 1 && pop[X.enf[0]].pere != X.conjoint[age]) {
          duree=duretat;
          proba[X.Id]= (1/(1+exp(-(-3.79-0.16*duree-2.11*(duree == 0)-0.29*(duree == 1)+0.30*age-0.06*pow(age,2)+0.52))));
        }
        if (parite_mere==2 && pop[X.enf[1]].pere != X.conjoint[age]) {
          duree=duretat;
          proba[X.Id]= (1/(1+exp(-(+3.97-0.30*duree-2.05*(duree == 0)-0.67*(duree == 1)-0.14*age+0.42))));
        }
        if ((parite_mere == 3 && pop[X.enf[2]].pere != X.conjoint[age]) || (parite_mere == 4 && pop[X.enf[3]].pere != X.conjoint[age])
              || (parite_mere == 5 && pop[X.enf[4]].pere != X.conjoint[age])) {
          duree=duretat;
          proba[X.Id] = 1/(1+exp(-(+4.30-0.09*duree-0.68*(duree == 0)-0.17*age-0.35*(parite_mere == 4)+0.2*(parite_mere == 5))));
        }
		/// 10- Redressements divers :
        if  (parite_mere == 4 && age<=25) {
          proba[X.Id]=0;
        }
        if  (parite_mere == 5 && age<=28) {
          proba[X.Id]=0;
        }
        if  (parite_mere == 0 && age == 16) {
          proba[X.Id]=proba[X.Id]*0.2*2;
        }
        if  (parite_mere == 0 && age == 17) {
          proba[X.Id]=proba[X.Id]*0.2*1.5;
        }
        if  (parite_mere == 0 && (age == 18)) {
          proba[X.Id]=proba[X.Id]*0.2*1.2;
        }
        if  (parite_mere == 0 && age == 19) {
          proba[X.Id]=proba[X.Id]*0.17*1.25;
        }
        if  (parite_mere == 0 && age == 20) {
          proba[X.Id]=proba[X.Id]*0.17*1.25*(2.0/3.0)*(2.5/1.5);
        }
		if  (age==41) {proba[X.Id]=proba[X.Id]/1.1;}
		if  (age==42) {proba[X.Id]=proba[X.Id]/1.4;}
		if  (age==43) {proba[X.Id]=proba[X.Id]/1.1;}
		if  (age==44) {proba[X.Id]=proba[X.Id]/4.0*(3.0/2.0);}
		if  (age==45) {proba[X.Id]=proba[X.Id]/6.0*(3.0/2.0);}
      }
    }
  }
  int cible= (ciblesDemo->Naissances[t])/M->poids+alea();
  int option_tirage = option_tirageNonCale ? TIRAGE_SIMPLE : TIRAGE_NB;
  auto liste_nvellemere=tirage(proba, option_tirage, cible, "NAISSANCE");
  auto probas_garcons=vector<double>(liste_nvellemere.size(),(105.0/205.0));
  int cible_garcon=int((105.0/205.0)*liste_nvellemere.size()+alea());
  auto liste_nvellemere_garcon=select(probas_garcons ,option_tirage,cible_garcon, "tiragegarcon");
  double findetnn = 0;
  vector<int> liste_G;
  vector<int> liste_F;
  for (int i : indices(liste_nvellemere)) {
    int e = liste_nvellemere[i];
    double u1=alea();
    double u2=alea();
    double alea_adfe=sqrt(-2*log(u1))*cos(2*PI*u2);
    int sexenn =liste_nvellemere_garcon[i] ? HOMME : FEMME ;
	//(int Id, int sexe, int anaiss,  int findet, int age, int pere, int mere)
    pop.emplace_back(pop.size(),sexenn,t+1900,18,0,0,0);
    Indiv& nouv_mere = pop[e];
    Indiv& B = pop.back();
    Indiv& nouv_pere = pop[nouv_mere.conjoint[nouv_mere.age(t)]];
    B.pere = nouv_pere.Id;
    B.mere = e;
    parite_pere = nouv_pere.nb_enf(nouv_pere.age(t),999);
    parite_mere = nouv_mere.nb_enf(nouv_mere.age(t),999);
    nouv_pere.enf[parite_pere]= B.Id;
    nouv_mere.enf[parite_mere]= B.Id;
    nouv_pere.anaissEnf[parite_pere]= t+1900;
    nouv_mere.anaissEnf[parite_mere]=t+1900;
    B.anaiss = t+1900;
    B.matri[B.age(t)]=1;
    if (liste_nvellemere_garcon[i]) {
      findetnn=20.8-0.04825+0.22753*(nouv_pere.findet-20.8)+0.20332*(nouv_mere.findet-21.2)+2.68015*alea_adfe;
      liste_G.push_back(B.Id);
    }
    else {
      findetnn=21.2+ 0.06294+ 0.2122*(nouv_pere.findet-20.8)+0.21972*(nouv_mere.findet-21.2)+2.61275*alea_adfe;
      liste_F.push_back(B.Id);
    }
    B.findet=min_max(findetnn,16,30)+alea();
    B.neFrance=1;
    B.statuts[0]=S_SCO;
    B.ageMax=1;
	  B.moisnaiss=B.Id%12;
  }
  // niveau de diplôme des nouveaux nés
  std::sort(liste_G.begin(),liste_G.end(),ordre_findet());
  std::sort(liste_F.begin(),liste_F.end(),ordre_findet());
  int nb_g=liste_G.size();
  for (int e : range(nb_g)) {
    Indiv& Y = pop[liste_G[e]];
    if (e<(nb_g*0.1162+alea())) {Y.dipl=SSDIPL;}
    else if (e<(nb_g*0.1573+alea())) {Y.dipl=BREVET;}
    else if (e<(nb_g*0.4111+alea())) {Y.dipl=CAP;} 
    else if (e<(nb_g*0.6528+alea())) {Y.dipl=BAC;}
    else {Y.dipl=UNIV;}
  }
  
  int nb_f=liste_F.size();
  for (int e : range(nb_f)) {
    Indiv& Y = pop[liste_F[e]];
    if (e<(nb_f*0.086+alea())) {Y.dipl=SSDIPL;}
    else if (e<(nb_f*0.1238+alea())) {Y.dipl=BREVET;}
    else if (e<(nb_f*0.2921+alea())) {Y.dipl=CAP;} 
    else if (e<(nb_f*0.5233+alea())) {Y.dipl=BAC;}
    else {Y.dipl=UNIV;}
  }

       
  // Pour avoir le bon nombre de décès l'année de la naissance, on reporte le nombre de décès d'un sexe sur l'autre 
  //et pour éviter que ce ne soit toujours le même on "shuffle" le vecteur des sexes
  double reste=0.0;
  vector<int> vect_sexe;
  vect_sexe.push_back(HOMME);
  vect_sexe.push_back(FEMME);
  random_shuffle(vect_sexe.begin(),vect_sexe.end());
  //mort des nouveaux nés
  for(int i : indices(vect_sexe)) {
	int sexe=vect_sexe[i];
    double pp=M->q_mort[sexe](t,0);
    int nbre_nn= (sexe==HOMME) ? liste_G.size():liste_F.size();
    auto listebb= (sexe==HOMME) ? liste_G :liste_F;
    auto p=vector<double>(nbre_nn,pp);
	int cible= int(pp*nbre_nn+ alea()+reste);
	reste= pp*nbre_nn-cible;
    auto listebb_morts = tirage(p, option_tirage,cible, to_string("mortalite"));
    for (int e : listebb_morts) {
		Indiv& Y = pop[listebb[e]];
		Y.ageMax=0;
    }
  }

 
  // emigrations des nouveaux nes
  for(int sexe : {HOMME,FEMME}) {
    int cible=(sexe==HOMME)? ((ciblesDemo->emigrant_H_0[t])/M->poids+alea()) : ((ciblesDemo->emigrant_F_0[t])/M->poids+alea()) ;
    auto listebb= (sexe==HOMME) ? liste_G :liste_F;
    vector<int> bb_viv;
    for(int i : indices(listebb)) {
      Indiv&X= pop[listebb[i]];
      if (X.ageMax==1) bb_viv.push_back(X.Id);
    }
    auto p=vector<double>(bb_viv.size(),0.5);
    auto listebb_emig = tirage(p, option_tirage, cible, to_string("emig_bb"));
    for(int i : listebb_emig) {
      Indiv& X = pop[bb_viv[i]];
      X.ageMax=0;
      X.emigrant=1;
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



