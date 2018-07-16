#include "Simulation.h"
#include "Indiv.h"
#include "Migration.h"


void mortalite(int t, bool option_mort_diff, bool option_mort_tirageCale,bool option_sante=false, bool option_mort_diff_dip=false) {

  auto indiv_par_age = make_vector2(121,3,deque<int>());
  // Création d'un tableau contenant les identifiants des individus présents en t-1 par âge et sexe
  for(Indiv& X : pop) {
    if(X.est_present(t-1)) {
      indiv_par_age[X.age(t)][X.sexe].push_back(X.Id);
    }
  }
  int w;
  double ecart =0.0;
  double u;		
  // - Création d'un tableau contenant les probabilités de survie des individus du tableau précédent.
  // - Puis tirage des individus survivants.
  for(int sexe : {HOMME,FEMME}) {
    for(int age : range(1,121)) {
      auto& pop_age = indiv_par_age[age][sexe];
      auto probas = vector<double>(pop.size(),0.0); 
      
      // probabilité de survie
      //double p = 1-M->q_mort[sexe](t,age) ;
	  double p = M->q_mort[sexe](t,age) ;
	  double p_origine = M->q_mort[sexe](110,age) ;
      if(age==120) p = 1;
      
      // Calcul probabilités de décès (ou de survie ?)
      for(int i : pop_age) {
        Indiv& X = pop[i];
        if(option_mort_diff && age > 30 && age < 100) {
          double p_diff = 1;
          if (X.sexe==HOMME){
            if      ((X.findet-finEtudeMoy->homme[X.anaiss-1900])<-2) p_diff = mortalite_diff->findetH1[X.age(t)] / 100;
            else if ((X.findet-finEtudeMoy->homme[X.anaiss-1900])<-1) p_diff = mortalite_diff->findetH2[X.age(t)] / 100;
            else if ((X.findet-finEtudeMoy->homme[X.anaiss-1900])<1)  p_diff = mortalite_diff->findetH3[X.age(t)] / 100;
            else if ((X.findet-finEtudeMoy->homme[X.anaiss-1900])>=1) p_diff = mortalite_diff->findetH4[X.age(t)] / 100;
          }
          if (X.sexe==FEMME){
            if      ((X.findet-finEtudeMoy->femme[X.anaiss-1900])<-2) p_diff = mortalite_diff->findetF1[X.age(t)] / 100;
            else if ((X.findet-finEtudeMoy->femme[X.anaiss-1900])<-1) p_diff = mortalite_diff->findetF2[X.age(t)] / 100;
            else if ((X.findet-finEtudeMoy->femme[X.anaiss-1900])<1)  p_diff = mortalite_diff->findetF3[X.age(t)] / 100;
            else if ((X.findet-finEtudeMoy->femme[X.anaiss-1900])>=1) p_diff = mortalite_diff->findetF4[X.age(t)] / 100;
          }
          probas[i] = p_diff;
        }
		else if(option_mort_diff_dip && age > 30 && age < 100) {
          double p_diff = 1;
          if (X.sexe==HOMME){
            if      ((X.dipl==1)) {p_diff =  mortadiff_dip_H->sansdip[X.age(t)]*p/p_origine;}
            else if ((X.dipl==2)) {p_diff =  mortadiff_dip_H->Brevet[X.age(t)]*p/p_origine;}
            else if ((X.dipl==3)) {p_diff =  mortadiff_dip_H->CAPBEP[X.age(t)]*p/p_origine;}
            else if ((X.dipl==4)) {p_diff =  mortadiff_dip_H->BAC[X.age(t)]*p/p_origine ;}
            else if ((X.dipl==5)) {p_diff =  mortadiff_dip_H->sup[X.age(t)]*p/p_origine; }
          }
          if (X.sexe==FEMME){
            if      ((X.dipl==1)) {p_diff =  mortadiff_dip_F->sansdip[X.age(t)]*p/p_origine;}
            else if ((X.dipl==2)) {p_diff =  mortadiff_dip_F->Brevet[X.age(t)]*p/p_origine;}
            else if ((X.dipl==3)) {p_diff =  mortadiff_dip_F->CAPBEP[X.age(t)]*p/p_origine;}
            else if ((X.dipl==4)) {p_diff =  mortadiff_dip_F->BAC[X.age(t)]*p/p_origine;}
            else if ((X.dipl==5)) {p_diff =  mortadiff_dip_F->sup[X.age(t)]*p/p_origine; }
          }
          probas[i] = p_diff;
        }
        else
          probas[i] = p;
	if (option_sante && age>=51 && age<=119) {
	  int choix_eq=(sexe == HOMME ? MORT_H : MORT_F) ;
	  auto& eq = eq_sante[choix_eq];
	  double age_corr=(max(age-options->m_incapacite*(M->espvie[sexe](t,age)-M->espvie[sexe](AN_BASE+1,age)),50.0)-50);
	  double reg=-eq.Intercept
	    -eq.couple*(X.matri[age-1]==2)
	    -eq.enf4plus*(X.nb_enf(age-1)>=4)
	    -eq.enf01*(X.nb_enf(age-1)<=1)
	    -eq.tresdip*(X.tresdip)
	    -eq.peudip*(X.peudip)
	    -eq.agebis*age_corr
	    -eq.agebis2*pow(age_corr,2)
	    -eq.agebis3*pow(age_corr,3)
	    -eq.sante*X.sante[age-1];
	  probas[i]=1/(1+exp(reg));
	} 
      }
      
      int option_tirage = option_mort_tirageCale ? TIRAGE_NB : TIRAGE_SIMPLE;
      // tirage liste des vivants
      if (option_mort_diff == 1 && option_tirage == 0) {
	//std::RCout <<"Pour la mortalité différentielle, il faut un tirage calé"<<endl;
        break;
      }
      u=p*pop_age.size() - ecart; // u = nombre de morts
      w = int(u +alea());
	  auto ind_morts = select(probas, option_tirage,w, to_string("mortalite ",t," ",age, " ",sexe));
      ecart=w-u;
      
      // changements des statuts des individus

	for(int i : pop_age) {
        
        if(ind_morts[i]) {
               pop[i].ageMax = age;
          if(pop[i].matri[age-1]==MARIE) {
			  pop[pop[i].conjoint[age-1]].matri[pop[pop[i].conjoint[age-1]].age(t)] = VEUF;}
		}
        else {pop[i].ageMax = age +1;
          pop[i].statuts[age] = pop[i].statuts[age-1];
          if (pop[i].matri[age] != VEUF) pop[i].matri[age] = pop[i].matri[age-1];
          pop[i].conjoint[age] = pop[i].conjoint[age-1];
		  pop[i].sante[age] = pop[i].sante[age-1];
		}
	}
	}
  }	
}


/*

// Avec une autre formule pour calculer la p_diff: P_diff(2050,30,H,1)=P(2050,30,H)*P_diff(2009,30,H,1)/P(2009,30,H) pour l'année 2050.
p = 1-M->q_mort[sexe](t,age);
if (X.sexe==HOMME){
  if      ((X.findet-finEtudeMoy->homme[X.anaiss-1900])<-2) p_diff = (M->q_mort[HOMME](t,age))*(mortalite_diff->findetH1[X.age(t)]/100)/(M->q_mort[HOMME](109,age));
  else if ((X.findet-finEtudeMoy->homme[X.anaiss-1900])<-1) p_diff = (M->q_mort[HOMME](t,age))*(mortalite_diff->findetH2[X.age(t)]/100)/(M->q_mort[HOMME](109,age));
  else if ((X.findet-finEtudeMoy->homme[X.anaiss-1900])<1)  p_diff = (M->q_mort[HOMME](t,age))*(mortalite_diff->findetH3[X.age(t)]/100)/(M->q_mort[HOMME](109,age));
  else if ((X.findet-finEtudeMoy->homme[X.anaiss-1900])>=1) p_diff = (M->q_mort[HOMME](t,age))*(mortalite_diff->findetH4[X.age(t)]/100)/(M->q_mort[HOMME](109,age));
}
if (X.sexe==FEMME){
  if    ((X.findet-finEtudeMoy->femme[X.anaiss-1900])<-2)   p_diff = (M->q_mort[FEMME](t,age))*(mortalite_diff->findetF1[X.age(t)]/100)/(M->q_mort[FEMME](109,age));
  else if ((X.findet-finEtudeMoy->femme[X.anaiss-1900])<-1) p_diff = (M->q_mort[FEMME](t,age))*(mortalite_diff->findetF2[X.age(t)]/100)/(M->q_mort[FEMME](109,age));
  else if ((X.findet-finEtudeMoy->femme[X.anaiss-1900])<1)  p_diff = (M->q_mort[FEMME](t,age))*(mortalite_diff->findetF3[X.age(t)]/100)/(M->q_mort[FEMME](109,age));
  else if ((X.findet-finEtudeMoy->femme[X.anaiss-1900])>=1) p_diff = (M->q_mort[FEMME](t,age))*(mortalite_diff->findetF4[X.age(t)]/100)/(M->q_mort[FEMME](109,age));
}
*/


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