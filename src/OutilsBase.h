/**
 * \file OutilsBase.h
 * \brief Contient une bibliothèque de fonctions utilitaires
 * 
 * Bibliothèque de fonctions utilitaires, en particulier
 * pour la gestion des dates au mois près.
 * 
 * Les dates et les âges dans Destinie sont exprimés pour la liquidation
 * des droits directs au mois près, et sous format de nombres à
 * virgules flotantes (doubles) avec pour unité le nombre d'années.
 * Pour gérer les problèmes d'arrondis, et effectuer des tests d'égalité
 * on utilise en particulier les fonctions \ref arr_mois et \ref int_mois.
 */

#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <array>
#include <deque>
#include <cmath>
#include <Rcpp.h>
#include <ctime>
//#include <random>
#include <algorithm>
#include <memory>
#include <utility>
#include "range.h" 
#include <sstream>
using namespace util::lang;


// Importations de la STL
// (librarie standard de C++)
using std::vector; // Tableau
using std::deque;  // Tableaux optimisés pour être redimensionnés
using std::array;
using std::string; // Chaines de caractères
using std::clock;  // Heure système
template<typename T>
using ptr = std::shared_ptr<T>;
using std::make_shared;
using std::pair;

// Fonctions mathématiques
using std::sqrt;   // racine carrée sqrt(25) == 5
using std::log;    // logarithme log(2.7183) ~= 1
using std::exp;    // exponentielle exp(log(2)) == 2
using std::cos;    // cosinus cos(3.1416) ~= 1
using std::sin;    // sinus : sin(3.1416) ~=  0
using std::pow;    // puissance : pow(2,8) == 256
using std::min;
using std::max;
using std::endl;
using std::nth_element;
using std::sort;

using namespace Rcpp;



/**
 * Macro testant une assertion et arretant le programme 
 * si l'assertion est invalide
 */

#define r_assert(e) {((e) ? (void)0 : Rf_error("assert false %s ligne %d : %s \n", __FILE__, __LINE__, #e));}


/**
 * \brief Calcul la valeur d'une fonction affine par morceaux.
 * 
 * Calcule la valeur en un point donné d’une fonction affine par morceaux.
 * On donne le point où on souhaite l'évaluer et un nombre arbitraire de paires xi,yi 
 * (avec x croissant) correspondant aux points anguleux. 
 * La fonction est supposée constante avant et aprés le dernier point fournis
 * 
 * Exemple :
 * ` double y = affn(x, {2010,0,2020,10});`
 * y vaut 0 pour x <= 2010, 3 pour x = 2013, 10 pour x >= 2020 
 */

inline double affn(double v, const vector<double> & u) {
    int u_size = u.size();
    if(v >= u[u_size-2])
        return u[u_size-1];
    for(int j=u_size-4; j >= 0; j-=2 )
        if(u[j] < v)
            return u[j+1]+(u[j+3]-u[j+1])*(v-u[j])/(u[j+2]-u[j]) ;
    return u[1];
}


/**
 * \brief Retourne la valeur d'une fonction constante par morceaux.
 * 
 * Calcule valeur en un point donné d’une fonction constante par morceaux.
 * On donne le point où on souhaite l'évaluer et un nombre arbitraire de paires xi,yi 
 * (avec x croissant) correspondant aux points de discontinuités. 
 * La fonction est supposée constante avant et aprés le dernier point fournis
 * 
 * Exemple :
 * ` double y = esc(x, {0, 2010, 5,2020,10});`
 * y vaut 0 pour x < 2010, 5 pour 2010 <= x < 2020, 10 pour x >= 2020 
 */
inline double esc(double v, const vector<double> & u) {
  int u_size = u.size();
	if (v < u[1])
		return u[0];
	for (int j = 0; j < u_size-1; j += 2)
		if (v < u[j+1])
			return u[j];
	return u[u_size-1];
}

/**
 * \brief Arrondit un nombre réel à un nombre entier
 */
inline int arr(double x)
{
  return x + (x>0 ? 0.5 : - 0.5);
}

/**
 * \brief Arrondit une date au mois près stockée sous forme d'un double
 * 
 * Arrondit une date précise passée sous forme d'un double
 * de façon à pouvoir effectuer des comparaisons de dates au mois près.
 * En effet la manière dont sont calculés les nombres à virgule flotante 
 * peuvent entrainer des phénomènes d'arrondis de calcul  tels que 
 * par exemple 20+1/12 != 10+121/12.
 * arr_mois assure que `arr_mois(20+1/12) == arr_mois(20+121/12)` renverra
 * le bon résultat.
 */
inline double arr_mois(double annee)
{
    int a = arr(annee * 12);
    return a/12 + (a % 12) / 12.0;
}



/**
 * \brief Arrondit une date au mois près stockée sous forme d'un double
 * 
 * Idem première fonction arr_mois, avec comme paramètre supplémentaire
 * le nombre de mois sous forme d'entier.
 * 
 * `arr_mois(60+4/12.0) == arr_mois(60,4) == arr_mois(50+124/12.0)`
 * 
 */
inline double arr_mois(double annee, double mois)
{
    int a = arr(annee * 12 + mois);
    return a/12 + (a % 12) / 12.0;
}




/**
 * \brief Renvoie un nombre d'année sous forme d'entier
 * 
 * Renvoie un nombre d'année sous forme d'entier :
 * `int_mois(60+10/12.0) == 60`
 */
inline int int_mois(double annee) {
  return int(annee + 1/24.0);
}

/**
 * \brief Renvoie un nombre d'année sous forme d'entier
 * 
 * Renvoie un nombre d'année sous forme d'entier :
 * `int_mois(60+10/12.0) == 60`
 * 
 * Remarque :
 * Un individu atteindra l'âge de 60 ans et 6 mois
 * au cours de l'année :
 *  `int_mois(X.anaiss+60, 6 + X.moisnaiss)`
 */
inline int int_mois(double annee, double mois) {
  return int(annee + mois/12.0 + 1/24.0);
}



/**
 * \brief test appartenance à un ensemble de taille quelconque
 * 
 * Test appartenance à un ensemble de taille quelconque
 * \return true si x est présent dans le vecteur ens, false sinon
 * \param x entier dont l'appartenance est testé
 * \param ens vecteur de l'ensemble des valeurs auquel on teste l'appartenance
 */
inline bool in(int x, const vector<int> & ens) {
  for(const int& y: ens) if(y==x)  return true;
  return false;
}


/**
 * \brief Borne majore et minore un réel
 * 
 * \return x si y\f$\leq\f$x\f$\leq\f$z, y si x<y et z si x>z
 * \param x valeur initiale
 * \param y borne inférieure
 * \param z borne supérieure
 */


inline double min_max(double x, double y, double z){
  return (x < y) ? y : (x > z) ? z : x;
}





inline int trimInf(double an, double mois) {
  return (an*12+mois > 0) ? int(floor(an*4+mois/3.0 + 0.25))  : 0;
}

/*** \brief Arrondi une valeur positive au trimestre supérieur
***/
inline double ArrTrimSup(double x) {
    return (x > 0)  ?  (1+floor(4*x))/4.0  :  0.0 ;
}

/*** \brief Arrondi une valeur positive au trimestre inférieur
***/
inline double ArrTrimInf(double x) {
    return (x > 0)  ?  floor(4*x)/4.0  :  0.0 ;
}


/** 
\brief fonction LogLogist

Tirage d'une VA selon une loi LogLogistique de fonction de répartition
z**a/(b+z**a) paramétrée par deux quelconques de ses quantiles, par
exemple

 `x=LogLogist(.25,2,.5,3,S.alea())`

tirera une VA à valeurs positives dont le premier quartile est à 2 et la
médiane est à 3.

Si l'on ajoute un argument supplémentaire, cet argument est pris comme valeur de l'aléa par défaut.
Sinon, un aléa est tiré selon une loi uniforme.   (modif le 21/12/2011)

**/
inline double LogLogist(double q1, double v1, double q2, double v2, double alea) {
    double a = ( log(q1/(1-q1)) - log(q2/(1-q2)) ) / ( log(v1/v2) );
    double b = pow(v1,a) * (1-q1) / q1;
    return pow(b*alea/(1-alea), 1/a);
}




/**
 * \brief Calcule la part d'un nombre comprise entre deux seuils
 * 
 * Calcule la part d'un nombre x comprise entre deux seuils s1 et s2, 
 * c’est-à-dire 0 si x<s1, x-s1 si x entre s1 et s2 et s2-s1 si x>s2.
 */

inline double part(double base, double seuil1, double seuil2)
{
    if (base < seuil1)
        return 0;
    if (base < seuil2)
        return (base - seuil1);
    
    return (seuil2-seuil1);
}

/**
* \brief Calcule la part d'un nombre comprise entre deux seuils
* 
* Calcule la part d'un nombre x comprise entre deux seuils s1 et s2, 
* c’est-à-dire 0 si x<s1, (x-s1)/(s2-s1) si x entre s1 et s2 et 1 si x>s2.
*/

inline double partTx(double base, double seuil1, double seuil2)
{
  if (base < seuil1)
    return 0;
  if (base < seuil2)
    return (base - seuil1)/(seuil2-seuil1);
  
  return 1;
}



/**
 * \brief Fonction réciproque de la fonction de répartition logistisque.
 * 
 * Fonction réciproque de la fonction de répartition logistisque.
 * 
 * Exemple: tirage d'une v.a. y suivant une Pareto:
 * `double y = Logist(1, 1/3.0, S.alea())`
 */

inline double Logist(double mu, double se, double x) {
  return mu+(se*pow(3,0.5)/3.14159) * log(x/(1-x));
}

/**
 * \brief Fonction réciproque de la fonction de répartition de Pareto.
 * 
 * Fonction réciproque de la fonction de répartition de Pareto.
 * 
 * Exemple: tirage d'une v.a. y suivant une Pareto:
 * 
 * `double y = Pareto(1, 1/3.0, S.alea())`
 */
inline double Pareto(double seuil, double param, double x) {
  return seuil*(pow(1-x,-1/param));
}


/**
 * \brief Fonction réciproque de la fonction de répartition d'une loi multinomiale.
 * 
 * Fonction réciproque de la fonction de répartition d'une loi multinomiale.
 * 
 * Exemple: tirage d'une v.a. y suivant une loi multinomiale
 * 
 * `double y = Multinom(S.alea(), {1/3.0, 1/4.0})`
 * renvoie 1 avec une proba 1/3.
 * renvoie 2 avec une proba 1/4.
 * renvoie 3 avec une proba 5/12.
 */
inline double Multinom(double x, const vector<double>& v) {
  double pcum = v[0];
  unsigned int i = 0;
  while(x > pcum && (i+1) < v.size()) pcum += v[++i];
  return i;
}

/**
 * \brief Renvoie un tableau d'aléas lissés
 */
inline vector<double> lisseAlea(vector<int>& strate1, vector<int>& strate2, double alea)
{
    r_assert(strate1.size() == strate2.size());
    auto result = vector<double>(strate1.size(),0.0); 
    
    int i_max=0, j_max=0;
    
    for(auto i : strate1) {i_max=i_max < i ? i : i_max;} // Rmq : i_max devient égal au max de 0 et des éléments de strate1
    for(auto i : strate2) {j_max=j_max < i ? i : j_max;}
    
    auto listes = make_vector2(i_max+1,j_max+1,deque<int>());
    for(auto i : range(strate1.size())) 
      listes[strate1[i]][strate2[i]].push_back(i); // Rmq : dans la pratique (cf. Salaires.cpp), strate1= vecteur de sexe des individus, strate2=vecteur de générations des individus.
												   // listes contient donc les identifiants des individus par sexe et génération.
    std::mt19937 g(INT_MAX*alea);
    
    for(auto& s1 : listes)
      for(auto& s2 : s1)
        if(s2.size()) {
          shuffle(begin(s2),end(s2), g);
          double alea2 = alea;
          int inc = 0;
          for(auto i : s2) result[i] = (alea2 + inc++) / s2.size(); // Rmq : result[i] suit bien une loi uniforme sur [0,1] grâce au shuffle.
        }
    
    return result;
}

template<typename T>
inline typename T::value_type sum(const T& vect)
{
	typename T::value_type tot = 0;
	for (auto& v : vect) tot += v;
	return tot;
}

/**
 * \struct Paire 
 * \brief Paire contient les identifiants et leurs probabilités ainsi que la structure d'ordre associée.
 */
struct Paire { int id; double proba; };
inline bool greatP(Paire& x, Paire& y) {
  return x.proba > y.proba;
}  

template<typename T1, typename T2>
void order(T1& val, T2& val2, bool reverse=false)
{
	using Pair = pair<typename T1::value_type, typename T2::value_type>;
	vector<Pair> vect;
	vect.reserve(val.size());
	for (unsigned int i = 0; i < val.size(); i++)
		vect.emplace_back(val[i], val2[i]);

	if (reverse)
		sort(begin(vect), end(vect), [](Pair& lhs, Pair& rhs) {return lhs.second > rhs.second; });
	else 
		sort(begin(vect), end(vect), [](Pair& lhs, Pair& rhs) {return lhs.second < rhs.second; });
	for (unsigned int i = 0; i < val.size(); i++)
		val[i] = vect[i].first;
}

enum {TIRAGE_SIMPLE,TIRAGE_ESPERANCE,TIRAGE_TAUX,TIRAGE_NB};
deque<int> tirage(vector<int> champ, vector<double>& probas, int type_cible, double cible, string label="");  
deque<int> tirage(vector<double>& probas, int type_cible, double cible, string label="");  

vector<bool> select(vector<int> champ, vector<double>& probas, int type_cible, double cible, string label="");  
vector<bool> select(vector<double>& probas, int type_cible, double cible, string label="");  



enum { SANS_REMISE, AVEC_REMISE};

template<int N>
vector<array<int,2>> appariement(const deque<array<double,N>>& candidats1, const deque<array<double,N>>& candidats2, vector<double> poids, int option)
{
	vector<array<int, 2>> id_candidats2(candidats1.size());
	vector<bool> select_candidats2(candidats2.size(),false);

	for (unsigned int k = 0; k < candidats1.size(); k++) {
		auto& c = candidats1[k];
		double min_score = 1;
		double min_id = 0;
		for (unsigned int j = 0; j < candidats2.size();j++) {
			auto& c2 = candidats2[j];
			double nouv_score = 0;
			if (option == AVEC_REMISE || !select_candidats2[j]) {
				for (unsigned int i = 1; i < c2.size(); i++) {
					nouv_score += poids[i-1] * pow(c[i] - c2[i], 2);
				}
				if (nouv_score < min_score) {
					min_score = nouv_score;
					min_id = j;
				}
			}
		}
		id_candidats2[k] = {int(candidats1[k][0]), int((min_score < 1) ? candidats2[min_id][0] : -1) };
		if (min_score < 1) select_candidats2[min_id] = true;
	}
	return id_candidats2;
}

/* Etant donnée une matrice A symétrique réelle définie positive, renvoie la matrice triangulaire inférieure L (qui est donc unique) à coefficients diagonaux positifs telle que A=LL' (décomposition de Cholesky). */
vector<vector<double> > cholesky(vector<vector<double> > A);

/* Simule un vecteur gaussien de moyenne m et de matrice de covariance LL' */
vector<double> vecteur_gaussien(vector<double> m, vector<vector<double> > L);


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