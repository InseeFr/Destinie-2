/**
 * \file Salaires.h
 * \brief Fichier contenant les outils permettant de simuler les salaires, à la fois en rétrospectif et en projection.
 */
 
 #pragma once
#include "Simulation.h"
#include "Indiv.h"

/**
 * \struct EqSalaires
 * \brief La structure EqSalaires contient les coefficients des variables explicatives des équations de salaires, 
 * pour un sexe et un secteur donnés.
 * 
 * Pour la signification des libellés des coefficients, voir \cite aubert2012disparites , pp 30-35.
 */
struct EqSalaires {
  double   _(Intercept),         _(E_1er),             _(E_0),               _(E),                 _(E2),                _(FR_E),            
           _(FR_E2),             _(FR_E_0),            _(FR_D2),             _(D),                 _(D2),                _(FR_D),            
           _(D_0),               _(FR_D_0),            _(FR), _(log_fp)	,               _(Sig2pente_indiv),              
           _(corr_indiv),       _(Sig2_indiv),    		
		   _(Sig2_res);    
 };
  

/**
* \struct Options_salaires
* \brief La structure Options_salaires contient un attribut booléen, valant 1 pour une simulation des salaires sans aléa, et 0 sinon
*/
struct Options_salaires {
  bool _(noAlea);
  };

/**
 * \struct CStructSexeAge
 * \brief La structure CStructSexeAge contient la série temporelle des coefficients correcteurs utilisés lors de la simulation des salaires, pour les 
 * femmes et les hommes.
 * 
 *  Voir \cite aubert2012disparites , page 34, pour plus de détails.
 */
struct CStructSexeAge {
    NumericVector 
  /**
   * \brief Coefficient correctif appliqué au revenu d'activité des hommes 
   */
  _(CORREC_HOMMES), 
  /**
   * \brief Coefficient correctif appliqué au revenu d'activité des femmes 
   */
  _(CORREC_FEMMES);
};

/**
 * \struct Salaire
 * \brief La structure Salaire contient les équations et les fonctions permettant l'imputation des revenus d'activité.
 * 
 * La structure Salaire contient :
 * <ul>
 * <li> en attributs: 
 *    <ul>
 *              <li> les coefficients des variables explicatives de l'équation de salaire, et ce, pour chaque sexe et chaque secteur
 *              <li> la chronique annuelle du coefficient correcteur utilisé lors de la simulation des salaires, pour les femmes et les hommes
 *              <li> l'option sur la simulation des salaires (présence d'aléa ou non)
 *    </ul>    
 * <li> en méthodes :  
 *    <ul> 
 *        <li> une fonction imputant le statut de cadre au cours d'une année
 *        <li> une fonction imputant le statut d'AVPF au cours d'une année
 *        <li> une fonction imputant le taux de primes dans la fonction publique en fin de carrière
 *        <li> une fonction qui recode le statut, uniquement dans le but de faciliter la simulation
 *        <li> une fonction qui calcule la durée passée dans le statut courant, sans interruption
 *        <li> une fonction qui calcule la durée passée en emploi 
 *    </ul>
 *</ul>               
 */
struct Salaire {    
  Environment& env;
  vector<EqSalaires> eqs = Rdin<EqSalaires>("EqSalaires","NomVar",levels(FPE_F,FPE_H,FPHT_F,FPHT_H,IND_F,IND_H,PRI_F_deb,PRI_F_fin,PRI_H_deb,PRI_H_fin));
  CStructSexeAge cs = Rdin<CStructSexeAge>("CStructSexeAge");
  Options_salaires options_sal = Rdin<Options_salaires>("options_salaires");
  
  Salaire(Environment& env) : env(env){}
 
 /**
  * \fn imput_statut_cadre(int t)
  * \brief Impute le statut cadre à une date t. Après la phase de simulation des transitions, tous les salariés du privé ont par défaut
  *  le statut non-cadre. L’imputation du statut de cadre dépend uniquement du niveau de revenu, en respectant une cible p. Les 
  *  cadres correspondent alors aux salariés du privé faisant partie des p% plus hauts salaires. Cela revient à calculer le quantile d’ordre
  *  1-p de la distribution des salaires dans le privé et à sélectionner les salaires supérieurs à ce quantile. p est
  *  une fonction affine par morceaux de t, déterminée par les valeurs suivantes : p(1930)=5%, p(2005)=20%, p(2015)=23%, p(2025)=24%
  *  et p(2050)=25%. Le statut de cadre étant supposé relativement absorbant, les salaires des individus ayant été cadres en t-1 sont multipliés
  *  par 2, de sorte à accroître leur rang dans l’échelle des salaires et à augmenter leur chance de rester cadres.
  */
  void imput_statut_cadre(int t);
  
  /**
   * \fn imput_sal(Indiv& X, int age, double ei1, double ei2, double e1, double e2)
   * \brief Renvoie le revenu d'activité de l'individu X à l'âge age, en fonction du sexe et du statut.
   * 
   *Les paramètres ei1 et ei2 (resp. e1 et e2) sont les aléas utilisés pour la simulation des effets permanents (resp. du résidu annuel).
   */
  double imput_sal(Indiv& X, int age,  
                 double ei1, double ei2, 
                 double e1, double e2);

  void imput_salaires();
  
  /**
   * \fn imput_avpf(int t)
   * \brief Impute le statut d'AVPF en l'année t. 
   * 
   * L'affiliation à l'AVPF étant soumise à la perception de certaines prestations et à une
   * condition de ressources, cette fonction est appelée après imput_sal. Dans Destinie, seuls les inactifs ayant des enfants à charge
   * sont susceptibles d’être affiliés à l’AVPF. Cette affiliation permet d’acquérir des droits à la retraite dans le régime général
   * sans versement de cotisations : les droits sont calculés sur la base d’une rémunération fictive égale au SMIC. Le bénéfice de l’AVPF
   * est testé sur chaque individu. La méthode prestafam détermine le type de prestation familiale perçue (allocation de base,
   * complément familial, complément de libre choix d’activité) en t, ce qui permet à la méthode test_ avpf de tester 
   * l’éligibilité à l’AVPF en fonction des ressources et de la prestation perçue. L’imputation de l’AVPF devient alors effective en
   * modifiant le statut des inactifs concernés : celui-ci passe de S_ INA à S_ AVPF.
   */
  void imput_avpf(int t);
  
  /**
   * \fn input_taux_prim()
   * \brief Impute le taux de primes pour les titulaires de la fonction publique.
   * 
   *  Le taux de prime des fonctionnaires est simulé selon une
   *  distribution discrète dépendant du niveau de salaire et du sexe. Le salaire pris en compte est celui de la dernière année en
   *  qualité de fonctionnaire (hors invalidité), et les distributions du taux de prime sont déclinées suivant 9 niveaux de salaire définis
   *  par pas de SMPT/4. Ce taux ne dépend pas du nombre d’années dans la fonction publique, ni du type de fonction publique (FPE ou FPTH).
   *  Une fois imputé, il reste constant tout au long de la carrière de l’individu.
   */
  void input_taux_prim();
private:
  
  /**
   * \fn recode_statut(int st)
   * \brief Recode le statut st en une des quatre modalités suivantes : 0, 1, 2, -99
   */
  double recode_statut(int st);
  
  /**
   * \fn duree_statut(Indiv& X, int age)
   * \brief Calcule la durée passée dans le statut occupé à l'âge age, sans interruption
   */
  double duree_statut(Indiv& X, int age);
  
  /**
   * \fn duree_emp(Indiv& X, int age);
   * \brief Calcule la durée passée en emploi à l'âge age
   */
  double duree_emp(Indiv& X, int age);
  const  vector<double> corr = 
    {0       ,1952,	0.191   ,1953,	0.189   ,1954,	0.118   ,
     1955,	0.263   ,1956,	0.295   ,1957,	0.492   ,1958,	0.500   ,
     1959,	0.758   ,1960,	0.554   ,1961,	0.974   ,1962,	1.080   ,
     1963,	1.351   ,1964,	1.058   ,1965,	0.770   ,1966,	0.762   ,
     1967,	0.528   ,1968,	0.665   ,1969,	0.885   ,1970,	0.545   ,
     1971,	0.519   ,1972,	0.340   ,1973,	0.648   ,1974,	-0.288  ,
     1975,	-0.234  ,1976,	0.256   ,1977,	0       } ;   
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