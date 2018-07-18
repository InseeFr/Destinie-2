/**
 * \file Statistiques.h
 * \brief Statistiques contient des structures qui facilitent la manipulation des indicateurs suivants : moyenne, somme, taux, ratio, quantile. Ces structures sont notamment utilisées dans \ref Indicateurs_annee_COR.h
 */
 
 #pragma once

#include <vector>
#include <algorithm>
using namespace std;

/**
 * \struct Indicateur
 * \brief Structure à l'origine des structures Moyenne, Ratio, Somme et Taux.
 */
 
struct Indicateur {
  public:
    virtual double resultat() = 0; // must provide a definition for virtual functions or declare = 0
    virtual operator double() {
      return (this->resultat());
    }; 
};

/**
 * \struct Moyenne
 * \brief Structure permettant de manipuler des moyennes. 
 */
 
struct Moyenne : public Indicateur {
  double somme = 0;
  double compteur = 0;
  public:
  inline void push(bool cond, double val, double poids) {
      somme += cond ? val * poids : 0;
      compteur += cond ? poids : 0;
  }
  inline void push(double val) {
      somme += val;
      compteur += 1;
  }
  double resultat() {
      return compteur > 0 ? somme / compteur : 0;
  }  
};

/**
 * \struct Somme
 * \brief Structure permettant de manipuler des sommes. 
 */
struct Somme : public Indicateur {
  double somme = 0;
  public:
  inline void push(bool cond, double val, double poids) {
      somme += cond ? val * poids : 0;
  }
  inline void push(double val) {
      somme += val;
  }
  double resultat() {
      return somme;
  }
};

/**
 * \struct Taux
 * \brief Structure permettant de manipuler des taux. 
 */
struct Taux : public Indicateur {
  double somme = 0;
  double compteur = 0;
  public:
  inline void push(bool cond, bool val, double poids) {
      somme += val ? poids : 0;
      compteur += cond ? poids : 0;
  }
  double resultat() {
      return compteur > 0 ? somme / compteur : 0;
  }  
};

/**
 * \struct Ratio
 * \brief Structure permettant de manipuler des ratios. 
 */
struct Ratio : public Indicateur {
  double somme_den = 0;
  double somme_num = 0;
  public:
  inline void push(bool cond, double val_num, double val_den) {
      somme_den += cond ? val_den : 0;
      somme_num += cond ? val_num : 0;
  }
  
  double resultat() {
      return somme_den > 0 ? somme_num / somme_den : 0;
  }
};

/**
 * \struct quantile
 * \brief Structure permettant de manipuler des quantiles. 
 */
struct quantile {
  deque<double> liste;
  bool sorted = false;

  inline void push(double val) {
    sorted=false;
    liste.push_back(val);
  }
  
  double resultat(double q) {
    if (liste.size() <= 0)
        return 0;
    if(!sorted)
        sort(begin(liste), end(liste));
    sorted=true;
    return liste[round(q*liste.size())];
  }
};


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