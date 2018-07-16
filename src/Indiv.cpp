#include "Indiv.h"
#include "DroitsRetr.h"
#include "Reversion.h"
#include "Retraite.h"
#include "Cotisations.h"


Indiv::Indiv(Ech& ech, Emp& emp, Fam& fam, int& i, int& i_e, int& i_f) 
{
  Id        = i+1       ;
  sexe      = ech.sexe[i]     ;
  anaiss    = ech.anaiss[i]   ;
  moisnaiss = ech.moisnaiss[i]   ;
  findet    = ech.findet[i]   ;
  taux_prim = ech.taux_prim[i];
  typeFP    = ech.typeFP[i]   ;
  neFrance  = ech.neFrance[i] ;
  emigrant  = ech.emigrant[i] ;
  k         = ech.k[i]        ;
  peudip 	  = ech.peudip[i]	;
  tresdip 	= ech.tresdip[i]	;
  dipl      = ech.dipl[i];
  
  int j_size = emp.Id.size();
  int k_size = fam.Id.size();
  
  while(i_e < j_size && emp.Id[i_e] == Id) {
    int age = emp.age[i_e];
    statuts[age] = emp.statut[i_e];
    salaires[age] = emp.salaire[i_e];
    if(statuts[age] != 0) ageMax = age+1;
    i_e++;
  }
  
  if(ageMax==0) {
  mere=abs(fam.mere[i_f+1]);
  pere=abs(fam.pere[i_f+1]);
  i_f++;
  }
  else{
	for(int age=0; age<ageMax; age++) {
    int an_max = max(AN_BASE+1900,anaiss+age);
    while(i_f+1 < k_size && (
          (Id > fam.Id[i_f]) || (Id == fam.Id[i_f+1] && an_max >= fam.annee[i_f+1])
            ))
      i_f++;
    if(i_f>=k_size || fam.Id[i_f] != Id)   
      break;
    
    pere = abs(fam.pere[i_f]);
    mere = abs(fam.mere[i_f]);
    matri[age] = fam.matri[i_f];
    enf[0] = abs(fam.enf1[i_f]); anaissEnf[0] = ech.anaiss[enf[0]-1];
    enf[1] = abs(fam.enf2[i_f]); anaissEnf[1] = ech.anaiss[enf[1]-1];
    enf[2] = abs(fam.enf3[i_f]); anaissEnf[2] = ech.anaiss[enf[2]-1];
    enf[3] = abs(fam.enf4[i_f]); anaissEnf[3] = ech.anaiss[enf[3]-1];
    enf[4] = abs(fam.enf5[i_f]); anaissEnf[4] = ech.anaiss[enf[4]-1];
    enf[5] = abs(fam.enf6[i_f]); anaissEnf[5] = ech.anaiss[enf[5]-1];
    conjoint[age] = abs(fam.conjoint[i_f]);
    /*if(fam.annee[i_f] == AN_BASE+1900 && fam.conjoint[i_f] < 0) {
      pseudo_conjoint = conjoint[age];
      conjoint[age] = 0; 
    } */
  }
  }
}  

Indiv::Indiv()  {}

Indiv::Indiv(const Indiv & indivACopier) :
   Id(indivACopier.Id), sexe(indivACopier.sexe), anaiss(indivACopier.anaiss), findet(indivACopier.findet),
   typeFP(indivACopier.typeFP), neFrance(indivACopier.neFrance),emigrant(indivACopier.emigrant),ageMax(indivACopier.ageMax),
   moisnaiss(indivACopier.moisnaiss), peudip(indivACopier.peudip),tresdip(indivACopier.tresdip),dipl(indivACopier.dipl),
   taux_prim(indivACopier.taux_prim),k(indivACopier.k),age_exo(indivACopier.age_exo),salaires(indivACopier.salaires),
   statuts(indivACopier.statuts),sante(indivACopier.sante),enfprestafam(indivACopier.enfprestafam),prestafam(indivACopier.prestafam),
   matri(indivACopier.matri),conjoint(indivACopier.conjoint),pere(indivACopier.pere),mere(indivACopier.mere),pseudo_conjoint(indivACopier.pseudo_conjoint),
   enf(indivACopier.enf),anaissEnf(indivACopier.anaissEnf)

{ 
	retr =make_shared<Retraite>(*(indivACopier.retr));

}

bool Indiv::est_decede(int t) {
  return (t  >= anaiss%1900 + ageMax + 1 && statuts[ageMax+1]==0);
}

// Fonction : est_persRef
bool Indiv::est_persRef(int t) {
  return (sexe==HOMME || !pop[conjoint[age(t)]].est_present(t));
}


// Méthode : ACharge
bool Indiv::ACharge(int t){
    int a = age(t);
    if((a > 21)&(t>=100))
      return false;

	if((a > 20)&(t<100))
      return false;
    if(in(statuts[a], Statuts_occ)&&salaires[a]>(0.55*M->SMIC[t]))
      return false;
	//if (enfprestafam[a]==1) 
      //return false;    TODO qu'ai je voulu dire ici
    if(present(pere,t))
      return true;
    if(present(mere,t))
      return true;      
    return false;
}


Indiv& Indiv::PersRef(int t){
    if(ACharge(t)) {
      if(present(pere,t))
        return pop[pere];
      else
        return pop[mere];
    }
    else if(sexe == FEMME && present(conjoint[age(t)],t))
      return pop[conjoint[age(t)]];
      
    return *this;
}

// Méthode SalMoyBrut
double Indiv::SalMoyBrut(double agefin, int nb) const {
  int age = int_mois(agefin, moisnaiss + 1);
  int nb_sal = 0;
  double sum_sal = 0;
  for(int a = age; a >= 14 && nb_sal < nb; a--) {
      if(in(statuts[a], Statuts_occ)) {
          nb_sal++;
          sum_sal+= salaires[a] * M->SMPT[anaiss%1900+age] / M->SMPT[anaiss%1900+a];
      }
  }
  if(nb_sal > 0)
      return sum_sal / nb_sal;
  return 0;
}

// Méthode NbEnfC
int Indiv::NbEnfC(int t,int ageEnfMax) 
{	int annee=t+1900;
    int nb_enf_a_charge = 0;
    for(int e=0; e < 6; e++) {
      Indiv& X = pop[enf[e]];
      if(X.est_present(t) && X.ACharge(t)&& ((annee - X.anaiss) <= ageEnfMax))
        nb_enf_a_charge++;
    }
    return nb_enf_a_charge;
}
 
 
// Méthode TailMen
int Indiv::TailMen(int t)
{
    if(pop[conjoint[age(t)]].est_present(t))
      return NbEnfC(t) + 2;
    else
      return NbEnfC(t) + 1;
}


/**
 * Renvoie le salaire net d'un individu
 */
double Indiv::SalNet(int t)
{
    int age = t-anaiss%1900;
    return salaires[age] - CotRet(*this,age)-CotAut(*this,age)-CSGSal(*this,age);
  
}

bool present(int id, int t) 
{
    if(id<=0 || id >= int(pop.size()))
      return false;
      
    t %= 1900;
    Indiv & X = pop[id];
    
    return X.est_present(t) ;
}

/** Probabilité de survie à l'âge age1 sachant survie à l'âge age0 */
double Indiv::survie(int age1, int age0) {
  return M->survie[sexe](min(date(age1),160), min(age1,120)) /
      M->survie[sexe](min(date(age0),160), min(age0,120));
}

/** Espérance de vie à l'âge age0 */
double Indiv::esp_vie(int age0) {
  double esp_vie = 0;
  for(int age = age0+1; age <= 120; age++) {
    esp_vie += M->survie[sexe](min(date(age),160), min(age,120));
  }
  return esp_vie;
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