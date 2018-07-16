#include "Simulation.h"
#include "Indiv.h"
#include "Sante.h"
#include "OutilsBase.h"
#include "Constantes.h"

void imputsante(int t){
	if (t%2==(AN_BASE+1)%2){ evolsante(t);}
	/*on ne fait les transitions que sur les ann?es paires (attention la variance )*/
	auto indiv_sexe = make_vector(3,vector<int>());
	for(Indiv& X : pop) {
		if(X.est_present(t)&&X.age(t)==50) {
		  indiv_sexe[X.sexe].push_back(X.Id);
		}
	}
	premiere_sante(t,indiv_sexe[HOMME],HOMME);
	premiere_sante(t,indiv_sexe[FEMME],FEMME);   
}

void evolsante(int t){
	auto indiv_par_sante_sexe_cage = make_vector3(3,3,9,vector<int>());
	/* on construit les 9*2*2 groupes sur lequels on applique les transitions de sante  classe d'age et hommes/femmes et malades/sains*/
	for(Indiv& X : pop) {
		if(X.est_present(t)&&X.age(t)>=51) {
			int cage= (X.age(t)<=54) ? 0 : 
					(X.age(t)<=59) ? 1 : 
					(X.age(t)<=64) ? 2 : 
					(X.age(t)<=69) ? 3 : 
					(X.age(t)<=74) ? 4 :
					(X.age(t)<=79) ? 5 : 
					(X.age(t)<=84) ? 6 : 
					(X.age(t)<=89) ? 7 : 8 ;
			indiv_par_sante_sexe_cage[X.sante[X.age(t)]][X.sexe][cage].push_back(X.Id);
		}
	}
	
	for(int sexe : {HOMME,FEMME}) {
		for(int sante : range(0,2)) {
			for (int cage : range(0,9)) {
				auto& pop_sante = indiv_par_sante_sexe_cage[sante][sexe][cage];
				int incid=(sante==0) ? (sexe == HOMME ? INCID_0_H : INCID_0_F ) : 
				(sexe == HOMME ? INCID_1_H : INCID_1_F) ;
				auto& eq = eq_sante[incid];    
				auto probas = vector<double>(pop_sante.size(),0.0); 
				for(int i : indices(pop_sante)) {
					Indiv& X = pop[pop_sante[i]];
					int age=X.age(t);
					double age_corr=(max(X.age(t)-2.0-options->m_incapacite*(M->espvie[sexe](max(t,110),age)-M->espvie[sexe](AN_BASE+1,age)),50.0)-50.0);
					double reg=-eq.Intercept
					-eq.couple*(X.matri[age]==2)
					-eq.enf4plus*(X.nb_enf(age)>=4)
					-eq.enf01*(X.nb_enf(age)<=1)
					-eq.tresdip*(X.tresdip)
					-eq.peudip*(X.peudip)
					-eq.agebis*age_corr
					-eq.agebis2*pow(age_corr,2)
					-eq.agebis3*pow(age_corr,3);
					 probas[i]=1/(1+exp(reg));
				}
				auto malades=select(probas, TIRAGE_ESPERANCE,0, to_string(" evol_sante ",t," ",sante, " ",sexe));
				for(int i : indices(pop_sante)) {
					Indiv& X = pop[pop_sante[i]];
					X.sante[X.age(t)]=(malades[i])?1 :0;
				}
			}
		}
	}	  
}

void premiere_sante(int t, vector<int>& champ,int sexe){
	auto proba = vector<double>(champ.size(),0.0);
	int preval=(sexe == HOMME ? PREVAL_H : PREVAL_F )  ;
	auto& eq = eq_sante[preval];
	for(int i : indices(champ)) {
		Indiv& X = pop[champ[i]];
		int age=X.age(t);
		double age_corr=(max(X.age(t)-options->m_incapacite*(M->espvie[sexe](max(t,110),age)-M->espvie[sexe](AN_BASE+1,age)),50.0)-50.0);
		double reg=-eq.Intercept
			-eq.couple*(X.matri[age]==2)
			-eq.enf4plus*(X.nb_enf(age)>=4)
			-eq.enf01*(X.nb_enf(age)<=1)
			-eq.tresdip*(X.tresdip)
			-eq.peudip*(X.peudip)
			-eq.agebis*age_corr
			-eq.agebis2*pow(age_corr,2)
			-eq.agebis3*pow(age_corr,3);
		proba[i]=1/(1+exp(reg));
	}
	auto malades = vector<bool>(champ.size(),0);
	malades=select(proba, TIRAGE_ESPERANCE,0, to_string(" preval_sante ",t," ", "sexe", sexe));
	for(int i : indices(champ)) {
		Indiv& X = pop[champ[i]];
		X.sante[X.age(t)]=(malades[i]) ? 1 :0;
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