#include "OutilsBase.h"
#include "OutilsRcpp.h"


deque<int> tirage(vector<int> champ, vector<double>& probas, int type_cible, double cible, string label)
{
  deque<int> resultat;
  double somme_probas = 0;
  
  
  if(type_cible != TIRAGE_SIMPLE) {
    // Tirage alignement par tri
    deque<Paire> tmp;
    auto odd = vector<double>(probas.size(),0.0);
    int proba_zero = 0;
    int proba_un = 0;
    int total_champ = 0;
    for(int i : indices(probas)) {
      if(!champ[i] || !(probas[i] >= 0)) continue;
      total_champ++;
      somme_probas += probas[i]; 
      if(probas[i]==1) {
        resultat.push_back(i);
        proba_un ++;
      }
      else if(probas[i] > 0) {
        double eps = alea();
        double odd0 = (probas[i]*(1-eps))/((1-probas[i])*eps);
        tmp.push_back({i,odd0});
      }
      else if(probas[i] == 0) {
        proba_zero++;
      }
    }    
    
    if(type_cible == TIRAGE_ESPERANCE) cible = somme_probas; // / total_champ;
    
    int nb_cible = min_max(
      cible*(type_cible == TIRAGE_TAUX ? total_champ : 1) + 
      alea() - resultat.size(),0,tmp.size());
    if(nb_cible > 0) {
      sort(begin(tmp),end(tmp),greatP);
      for(int i : range(nb_cible)) 
        resultat.push_back(tmp[i].id);
    }  
    
    static auto df0 = Rdout("tirageAvecChamp",
    {"type","cible","ciblep",
     "total_champ","proba0","proba1","probaP",
     "resultat"});
    df0.push_line(label,cible,somme_probas/(total_champ),
                  total_champ,
                  proba_zero, proba_un, total_champ-proba_un-proba_zero,
                  resultat.size());
  } else {
    // Tirage al?atoire simple
    for(int i : indices(probas))
      if(champ[i] > 0 && probas[i] >= 0 && probas[i] > alea())
        resultat.push_back(i); // resultat.push_back(champ[i]) ??
      
    static auto df1 = Rdout("tirageSimpleAvecChamp",{"label","nombre"});
    df1.push_line(label, resultat.size());
  }
  
  return resultat;  
}

deque<int> tirage(vector<double>& probas, int type_cible, double cible, string label)
{
  deque<int> resultat;
  double somme_probas = 0;
  
  
  if(type_cible != TIRAGE_SIMPLE) {
    // Tirage alignement par tri
    deque<Paire> tmp;
    auto odd = vector<double>(probas.size(),0.0);
    int proba_zero = 0;
    int proba_un = 0;
    int total_champ = 0;
    for(int i : indices(probas)) {
      if(!(probas[i] >= 0)) continue;
      total_champ++;
      somme_probas += probas[i]; 
      if(probas[i]==1) {
        resultat.push_back(i);
        proba_un ++;
      }
      else if(probas[i] > 0) {
        double eps = alea();
        double odd0 = (probas[i]*(1-eps))/((1-probas[i])*eps);
        tmp.push_back({i,odd0});
      }
      else if(probas[i] == 0) {
        proba_zero++;
      }
    }    
    
    if(type_cible == TIRAGE_ESPERANCE) cible = somme_probas; // total_champ;
    
    int nb_cible = min_max(
      cible*(type_cible == TIRAGE_TAUX ? total_champ : 1) + 
      alea() - resultat.size(),0,tmp.size());
    if(nb_cible > 0) {
      sort(begin(tmp),end(tmp),greatP);
      for(int i : range(nb_cible)) 
        resultat.push_back(tmp[i].id);
    }  
    
    static auto df0 = Rdout("tirageSansChamp",
    {"type","cible","ciblep",
     "total_champ","proba0","proba1","probaP",
     "resultat"});
    df0.push_line(label,cible,somme_probas/(total_champ),
                  total_champ,
                  proba_zero, proba_un, total_champ-proba_un-proba_zero,
                  resultat.size());
  } else {
    // Tirage al?atoire simple
    for(int i : indices(probas))
      if(probas[i] >= 0 && probas[i] > alea())
        resultat.push_back(i);
      
    static auto df1 = Rdout("tirageSimpleSansChamp",{"label","nombre"});
    df1.push_line(label, resultat.size());
  }
  
  return resultat;  
}

vector<bool> select(vector<int> champ, vector<double>& probas, int type_cible, double cible, string label)
{
  auto resultat = tirage(champ,probas,type_cible,cible,label);
  
  vector<bool> indic = vector<bool>(probas.size(),false);
  for(int k : resultat) {
    indic[k] = true;
  }
  
  return indic;  
}

vector<bool> select(vector<double>& probas, int type_cible, double cible, string label)
{
  auto resultat = tirage(probas,type_cible,cible,label);
  
  vector<bool> indic = vector<bool>(probas.size(),false);
  for(int k : resultat) {
    indic[k] = true;
  }
  
  return indic;  
}

/* Etant donnée une matrice A symétrique réelle définie positive, renvoie la matrice triangulaire inférieure L (qui est donc unique) à coefficients diagonaux positifs telle que A=LL' (décomposition de Cholesky). */
vector<vector<double> > cholesky(vector<vector<double> > A) {

int n = A.size();
double sum1 = 0.0;
double sum2 = 0.0;
double sum3 = 0.0;
vector<vector<double> > l(n, vector<double> (n));

// on calcule d'abord la première colonne de l
l[0][0] = sqrt(A[0][0]);
for (int j = 1; j <= n-1; j++)
	{l[j][0] = A[j][0]/l[0][0];}

// puis la deuxième jusqu'à l'avant-dernière
for (int i = 1; i <= (n-2); i++)
	{
	sum1=0.0;
	for (int k = 0; k <= (i-1); k++)
	sum1 += pow(l[i][k], 2);
	l[i][i]= sqrt(A[i][i]-sum1);
	for (int j = (i+1); j <= (n-1); j++)
	{
	sum2=0.0;
	for (int k = 0; k <= (i-1); k++)
	sum2 += l[j][k]*l[i][k];
	l[j][i]= (A[j][i]-sum2)/l[i][i];
	}
	}

// et enfin la derni?re	
for (int k = 0; k <= (n-2); k++)
sum3 += pow(l[n-1][k], 2);
l[n-1][n-1] = sqrt(max(A[n-1][n-1]-sum3,0.0));

return l;
}

/* Simule un vecteur gaussien de moyenne m et de matrice de covariance LL' */
vector<double> vecteur_gaussien(vector<double> m, vector<vector<double> > L) {
int n=m.size();
vector<double> vecteur_gauss_standard(0);
vector<double> resultat(n);
double norm1, norm2, alea1, alea2;

for(int i=0; i<int((n+1)/2); i++) {
	alea1=alea();
	alea2=alea();
	norm1=sqrt(-2 * log(alea1)) * cos(2*PI*alea2);
	norm2=sqrt(-2 * log(alea1)) * sin(2*PI*alea2);
	vecteur_gauss_standard.push_back(norm1);
	vecteur_gauss_standard.push_back(norm2);
	}


// on calcule le vecteur resultat = m + L*vecteur_gauss_standard
for(int i=0; i<n; i++) {
	resultat[i]=m[i];
	for(int k=0; k<(i+1); k++) {
		resultat[i] += L[i][k]*vecteur_gauss_standard[k];
		}
	}

return resultat;	
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