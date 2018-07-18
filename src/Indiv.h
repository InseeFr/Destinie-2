#pragma once
#include "Simulation.h"
/** \file Indiv.h
 * \brief Contient la définition de la classe \ref Indiv.
 */
class DroitsRetr;
class Reversion;
class Retraite;

/**
* \class Indiv
* \brief Classe contenant l'ensemble de l'information sur l'individu.
* 
* Cette classe contient l'ensemble de l'information sur l'individu
* lue dans les tables ech, emp, et fam.
* Elle contient de plus un pointeur sur un objet Retraite relatif
* à sa situation courante. 
*/
class Indiv {
public:

  // Caracteristiques individuelles générales (ech)
  int Id        = 0;              ///< Identifiant de l'individu
  int sexe      = 0;              ///< Sexe 
  int anaiss    = 0;              ///< Année de naissance 
  int findet    = 0;              ///< Âge de fin d'étude 
  int typeFP    = NONFP;          ///< Type de fonctionnaire (fonction publique d'état ou fonction publique territoriale ou hospitalière) 
  int neFrance  = 0;              ///< Indicatrice né en france 
  int emigrant	= 0;			        ///< Indicatrice  emigrant
  int ageMax    = 0;              ///< Âge maximal de statut renseigné (min âge décès, âge en fin de projection) 
  int moisnaiss = 0;              ///< Mois de naissance (0=janvier, 11=décembre)
  int dateMinFlux=999999;         ///< Date d'entrée dans le minimum vieillesse. Variable créée uniquement pour l'exercice de projection du Cor. A supprimer
  
  int peudip = 0;				          ///<Indicatrice d'être peu diplomé par rapport à sa génération et son sexe
  int tresdip= 0;				          ///<Indicatrice d'être très diplomé par rapport à sa génération et son sexe
  int dipl   = 0;                 ///<Niveau de diplôme de l'individu en classes
  
  double  taux_prim = 0;          ///< Taux de prime pour les fonctionnaires 
  double  k         = 0;          ///< Préférence pour le loisir 
  double  age_exo   = 0;          ///< Âge de liquidation exogène
  
 vector<double> salaires   		= vector<double>(AGE_MAX, 0.0) ;  ///< tableau des salaires indicés par l'âge 
 vector<int>    statuts    		= vector<int>(AGE_MAX, 0)      ;  ///< tableau des statuts indicés par l'âge 
 vector<int>    sante      		= vector<int>(AGE_MAX, 0)      ;  ///< tableau des états de santé indicés par l'âge 
 vector<int>	enfprestafam  	= vector<int>(AGE_MAX, 0) ;       ///< tableau indiquant si l'enfant a donné droit à des prestas fam pour un de ses parents (évite les doubles comptes des enfants)
 vector<int>	prestafam   	  = vector<int>(AGE_MAX, 0) ;       ///< tableau des types de prestation familiale indexé par l'age 
 vector<double> cotis_rg   		= vector<double>(AGE_MAX, 0.0) ;  ///< tableau des cotisations retraite au régime général par l'âge (part patronale et salariale)
 vector<double> cotis_fp   		= vector<double>(AGE_MAX, 0.0) ;  ///< tableau des cotisations retraite au régime de la fonction publique par l'âge 
 vector<double> cotis_in   		= vector<double>(AGE_MAX, 0.0) ;  ///< tableau des cotisations retraite au régime des indépendants par l'âge 
  
  // Caractéristiques familiales (fam)
  vector<int> matri     = vector<int>(AGE_MAX, 0);    ///< tableau des situation matrimoniales par âge  
  vector<int> conjoint  = vector<int>(AGE_MAX, 0);    ///< tableau des identifiants de conjoint par âge 

  // Identifiants des membres de la famille
  int pere = 0;                                   ///< identifiant du pere  
  int mere = 0;                                   ///< identifiant de la mère 
  int pseudo_conjoint = 0;                        ///< identifiant du pseudo-conjoint 
  vector<int> enf = vector<int>(6, 0);            ///< tableau des identifiants des enfants 
  vector<int> anaissEnf = vector<int>(6, 0);      ///< tableau des années de naissances des enfants            
  
  ptr<Retraite> retr =nullptr;   ///< Pointeur vers un objet de la classe Retraite  
  
  
  /** \brief Indique si la personne est vivante à la date t et est présente en France 
   * @return Renvoie vrai si l'individu est présent à une date t et faux si l'individu n'est pas vivant ou pas encore présent sur le territoire.
  */
  bool est_present(const int t) const
  {
      return (Id!=0) && 
        (t%1900 >= anaiss%1900) &&
        (t%1900 < anaiss%1900 + ageMax) &&
        (statuts[age(t%1900)] > 0);
  }
  
  /** \brief Indique si la personne décède l'année t 
   * \return `true`  si année du décès, `false` sinon
  */
  bool decede(int t);

  /** \brief Indique si la personne est à charge de l'un de ses parents
   *  \return `true` si la personne est à charge `false` sinon
  */
  bool ACharge(int t);
  
  /** \brief Renvoie la personne de référence du ménage de l'individu
  * \return Identifiant de la personne de référence du ménage
  */
  Indiv& PersRef(int t);
  
  /** \brief Renvoie le nombre d'enfants à charge une année donnée*/
  int NbEnfC(int t,int ageEnfMax = AGE_MAX) ;
  
  /** \brief Renvoie le nombre d'individu du ménage */ 
  int TailMen(int t);

  /** \brief Salaire net de l'année t */
  double SalNet(int t);

  /** \brief Revenu net de l'année t */
  double RevNet(int t);
  
  /** \brief Salaire moyen brut 
   *  \return Renvoie le salaire moyen de l'individu
   *  pour un âge donné et un nombre de salaires donné
  */
  double SalMoyBrut(double agefin, int nb) const;
  
  
  /** \brief Renvoie l'âge de l'individu à la date t */
  int age(int t) const {
    return t - anaiss%1900;
  }
  
  /** \brief Renvoie la date précise à un âge fin */
  double datefm(double age, double mois) {
    int a = int(anaiss*12+moisnaiss+age*12+mois+1/24.0);
    return (a/12+(a%12)/12.0);
  }

  /** \brief Renvoie la date (modulo 1900) pour un âge donné */
  int date(int age) const {
    return anaiss%1900 + age;
  }

  /** \brief Indique si la personne est personne de référence de son ménage l'année t */
  bool est_persRef(int t);
  
  /** \brief Indique si la personne est décédée l'année */
  bool est_decede(int t);
  
  /** \brief Probabilité de survie à l'âge age1 sachant la survie à l'âge age0 */
  double survie(int age1, int age0);

  /** \brief Espérance de vie à l'âge age0 */
  double esp_vie(int age0);

  /** \brief Nombre d'enfants l'âge \ref age*/
  inline int nb_enf(int age) const {
    int annee = date(age) + 1900;
    int nbEnf = 0;
    for(int e : range(6)) {
      if (enf[e] && annee - anaissEnf[e] > 0)
        nbEnf++;
    }
    return nbEnf;
  }
  
  /** \brief Nombre d'enfants sur l'ensemble de la vie*/
  inline int nb_enf() const {
    int nbEnf = 0;
    for(int e : range(6)) {
      if (enf[e] > 0)
        nbEnf++;
    }
    return nbEnf;
  }
  
  /** \brief Nombre d'enfants à un âge donné, avec un âge maximal pour les enfants*/
  inline int nb_enf(int age, int ageEnfMax) const {
    int annee = date(age) + 1900;
    int nbEnf = 0;
    for(int e : range(6)) {
      if (annee - anaissEnf[e] > 0 && annee - anaissEnf[e] <= ageEnfMax)
        nbEnf++;
    }
    return nbEnf;
  }
  
  
  /** \brief Constructeur */
  Indiv();
  /** \brief Constructeur à partir des tables \ref Ech, \ref Fam et \ref Emp*/
  Indiv(Ech& ech, Emp& emp, Fam& fam, int& i, int& i_e, int& i_f);
  /** \brief Constructeur en précisant certains paramètres*/
  Indiv(int Id, int sexe, int anaiss,  int findet, int age, int pere, int mere) : 
    Id(Id), sexe(sexe), anaiss(anaiss), findet(findet), ageMax(age+1), pere(pere), mere(mere) {
    if(age <= findet) statuts[age] = S_SCO;
    else statuts[age] = 999;
  }
  /**
   * \brief Constructeur d'un individu en copiant un individu
   */
   Indiv( const Indiv& indivACopier); 
  /** Destructeur */
   ~Indiv() {
     salaires.clear();
     statuts.clear(); 
     sante.clear();
     enfprestafam.clear(); 
     prestafam.clear();
     matri.clear();
     conjoint.clear();
     retr.reset();
     enf.clear();
     anaissEnf.clear();
   };
};

/** indique si une personne est présente à la date t à partir de son identifiant */
bool present(int id, int t) ;




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