#pragma once
#include "DroitsRetr.h"
#include "Indiv.h"
#include "Reversion.h"
#include "Simulation.h"
/** \file Retraite.h
 * \brief Contient la définition de la classe \ref Retraite et l'implémentation
 * de quelques fonctions du module retraite.
 */

/**
 * \class Retraite
 * \brief Pensions de droits directs et dérivés ainsi que le minimum vieillesse
 * à l'âge courant
 *
 * Pensions de droits directs et dérivés ainsi que le minimum vieillesse à l'âge
 * courant. La classe Retraite est le coeur de la simulation.
 *
 * Exemple d'utilisation :
 * =======================
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Indiv& X = pop[12];
 *
 * // crée un objet retraite
 * Retraite retraite(X, t);
 * // calcule les droits directs
 * retraite.SimDir(65);
 * if(retraite.primoliq && !retraite.totliq) {
 *    Rcout << "L'individu a effectué un primo-liquidation" << endl;
 *    Rcout << "Sa pension est : " << retraite.pension_tot << endl;
 * } else if (retraite.totliq) {
 *    Rcout << "L'individu a liquidé tous ses droits directs" << endl;
 *    Rcout << "Sa pension est : " << retraite.pension_tot << endl; *
 * } else {
 *    Rcout << l'individu n'a pas liquidé << endl;
 * }
 * retraite.SimDer(Y, t) // Calcul des droits dérivés
 * retraite.revalo(t+1) // calcul des pensions en t+1
 * retraite.minvieil(t+1) //calcul ASPA
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 */

class Retraite {
public:
  Indiv &X; ///< Individu X
  int age;  ///< Âge de l'individu

  ///\{ \name Pensions de droits directs
  double pension_tot = 0, ///< Pension de droit direct total
      pension_rg = 0,     ///< Pension au Régime Général
      pension_fp = 0,     ///< Pension à la FP
      pension_in = 0,     ///< Pension au RSI
      pension_ar = 0,     ///< Pension à l'ARRCO
      pension_ag = 0,     ///< Pension à l'AGIRC
      pension_ag_ar = 0;  ///< Pension au regime complementaire unifié

  ///\}\{ \name Pensions de droits dérivés
  double rev_tot = 0, ///< Réversion totale tous régimes
      rev_rg = 0,     ///< Réversion au Régime Général
      rev_fp = 0,     ///< Réversion à la FP
      rev_in = 0,     ///< Réversion au RSI
      rev_ar = 0,     ///< Réversion à l'ARRCO
      rev_ag = 0,     ///< Réversion à l'AGIRC
      rev_ag_ar =
          0; ///< Réversion au régime unifié des complémentaires du privé
  ///\}

  double min_vieil =
      0; ///< montant perçu pour l'ASPA (anciennement : minimum vieillesse).
         ///< N'est renseigné que pour la personne de référence du ménage

  double txCSG =
      0; ///< taux de CSG à appliquer aux retraites de base et complémentaires
  double txMal =
      0; ///< taux de maladie à aplliquer aux retraites complémentaires

  double retraite_tot = 0; ///< Total des pensions de droits directs et dérivés
  double retraite_net =
      0; ///< Total des pensions de droits directs et dérivés nets de la CSG de
         ///< la CRDS et des cotisations maladies pour les complémentaires

  ptr<DroitsRetr> primoliq = nullptr; ///< Pointeur vers la première liquidation
  ptr<DroitsRetr> totliq = nullptr;   ///< Pointeur vers la liquidation totale
  ptr<DroitsRetr> liq = nullptr; ///< Pointeur vers la dernière liquidation
  ptr<Reversion> liqrev =
      nullptr; ///< Pointeur vers la dernière liquidation de droits dérivé

  Retraite(Indiv &X, int t);
  /**
   * \fn SimDir(int age)
   * \brief La fonction \ref SimDir teste la liquidation (et la
   primo-liquidation le cas échéant) pour un individu X pour une année donnée,
   ainsi
   * que les droits directs.
   *
   * Teste la liquidation (et de primo-liquidation le cas échéant) pour un
   individu X pour une année donnée, ainsi que les droits directs.
   *
   * Cette fonction est une alternative à SimDirIndiv qui teste et calcule les
   droits pour toute la durée de vie de l'individu.
   * Les arguments sont  :
   * - X : L'individu
   * - age : Âge de l'individu.
   *
   * La fonction stocke les variables de liquidation dans le tableau deque_liq
   pour chaque liquidation.
   *
   * Les `pas` de test de la liquidation des droits peuvent prendre les valeurs
   (1= annuel, 0.25 = trimestriel, 1/12=mensuel).
   * Deux pas sont spécifiés : le premier est utilisé entre l'âge d'ouverture
   des droits et l'âge d'annulation de la décote
   * au RG, le second est utilisé en dehors de cette fenêtre d'âge. L'idée est
   qu'on veut souvent tester la liquidation
   * avec un pas temporel plus fin dans le premier cas que dans le second (par
   exemple, pas1 = 0.25 et pas2 = 1).
   *
   * \param age âge en fin d'année de l'individu
  */
  void SimDir(int age);

  /**
   * \fn SimDer(Indiv& Y, int t)
   * \brief La fonction SimDer initialise ou projette des droits dérivés.
   *
   * \param Y individu
   * \param t année de projection
   */
  void SimDer(Indiv &Y, int t);

  /**
   * \fn minvieil(int t)
   * \brief La fonction minvieil calcule le montant du minimum vieillesse auquel
   * a droit le cas échéant le ménage. \param t année de projection
   */
  void minvieil(int t);

  /**
   * \fn tx_cotis_sur_les_retraites(int t)
   * \brief Calcule le taux de CSG/Crds à appliquer à la pension
   * \param t année de projection
   */
  void tx_cotis_sur_les_retraites(int t);

  /**
   * \fn retraite_nette(int t)
   * \brief Calcul du montant de la retraite nette
   * \param t année de projection
   */
  void retraite_nette(int t);

  /**
   * \fn revaloDir(int t)
   * \brief Revalorisation des pensions de droits directs
   * \param t année de projection
   */
  void revaloDir(int t);

  /**
   * \fn revaloDer(int t)
   * \brief Revalorisation des pensions de droits derivés
   * \param t année de projection
   */
  void revaloDer(int t);

  /**
   *  \fn partavtliq(int t)
   *  \brief Renvoie la part de l'année avant la première liquidation
   *
   *  partavtliq renvoie la part de l'année avant la première liquidation et
   * permet le calcul de la proratisation des pensions. \param t année de la
   * simulation \return 1 si pas encore liquidé, 0 si en retraite toute l'année
   */

  double partavtliq(int t) {
    int age = X.age(t);
    if (liq)
      return liq->agefin_primoliq > 0
                 ? min_max(
                       arr_mois(liq->agefin_primoliq - age, X.moisnaiss + 1), 0,
                       1)
                 : 1;
    else
      return 1;
  }

  /**
   *  \fn partavttotliq(int t)
   *  \brief Renvoie la part de l'année avant la liquidation totale
   *
   *  La fonction renvoie la part de l'année avant la liquidation totale et
   * permet le calcul de la proratisation des pensions. \param t année de la
   * simulation \return 1 si pas encore liquidé, 0 si en retraite toute l'année
   */

  double partavttotliq(int t) {
    int age = X.age(t);
    if (totliq)
      return liq->agefin_totliq > 0
                 ? min_max(arr_mois(liq->agefin_totliq - age, X.moisnaiss + 1),
                           0, 1)
                 : 1;
    else
      return 1;
  }

  /**
   * \fn reversion(int regime)
   *
   * \brief Renvoie la valeur annuelle au 31/12 des pensions de réversion
   * touchées. \param regime régime d'affiliation
   */

  double reversion(int regime) {
    if (regime == REG_TOT)
      return rev_tot;
    if (regime == REG_RG)
      return rev_rg;
    if (regime == REG_IN)
      return rev_in;
    if (regime == REG_FP)
      return rev_fp;
    if (regime == REG_AR)
      return rev_ar;
    if (regime == REG_AG)
      return rev_ag;
    if (regime == REG_AGIRC_ARRCO)
      return rev_ag_ar;
    return 0;
  }

  /**
   * \fn pension(int regime)
   * \brief Renvoie la valeur annuelle au 31/12 des pensions de droits directs
   *touchées. \param regime régime d'affiliation
   **/
  double pension(int regime) {
    if (regime == REG_TOT)
      return pension_tot;
    if (regime == REG_RG)
      return pension_rg;
    if (regime == REG_IN)
      return pension_in;
    if (regime == REG_FP)
      return pension_fp;
    if (regime == REG_AR)
      return pension_ar;
    if (regime == REG_AG)
      return pension_ag;
    if (regime == REG_AGIRC_ARRCO)
      return pension_ag_ar;
    return 0;
  }

  /**
   * \fn pension_prorat(int regime)
   * \brief Renvoie la pension proratisée
   * \param regime régime d'affiliation
   **/
  double pension_prorat(int regime) {
    if (regime == REG_TOT)
      return pension_tot;
    if (regime == REG_RG)
      return pension_rg;
    if (regime == REG_IN)
      return pension_in;
    if (regime == REG_FP)
      return pension_fp;
    if (regime == REG_AR)
      return pension_ar;
    if (regime == REG_AG)
      return pension_ag;
    if (regime == REG_AGIRC_ARRCO)
      return pension_ag_ar;
    return 0;
  }

  /**
   * \fn revenu
   * \brief Renvoie la valeur annualisée au 31/12 des revenus touchés par un
   *individu
   **/
  double revenu() {
    if (liq)
      return retraite_tot;
    else
      return X.salaires[age] + rev_tot;
  }

  /**
   * \fn revenuCoupleBrut
   * \brief Renvoie la valeur annualisée au 31/12 des revenus touchés par un
   *ménage
   **/
  double revenuCoupleBrut() {
    double rev = revenu() + min_vieil;
    int conjoint = X.conjoint[age];
    if (conjoint && pop[conjoint].est_present(X.date(age))) {
      rev += pop[conjoint].retr->revenu();
    }
    return rev;
  }

  /**
   * \fn revenuReversion
   * \brief Calcule une approximation des revenus pris en compte dans la
   *condition de revenus pour la réversion des régimes alignés
   **/
  double revenuReversion() {
    if (liq)
      return pension_tot + rev_fp;
    else
      return X.salaires[age] * 0.7 + rev_fp +
             pension_tot; // decote de 30% des revenus salariaux Article
                          // R353-1du code de la SS
  }

  /**
   * \fn revenuReversionCouple()
   * \brief Calcule une approximation des revenus pris en compte dans la
   *condition de revenus pour la réversion des régimes alignés au niveau du
   *ménage
   **/
  double revenuReversionCouple() {
    double rev = revenuReversion();
    int conjoint = X.conjoint[age];
    if (conjoint && pop[conjoint].est_present(X.date(age))) {
      rev += pop[conjoint].retr->revenu();
    }
    return rev;
  }

  /**
   * \fn nbUCCouple()
   * \brief Calcule le nombre d'unités de consommation (UC) du ménage (sans
   *tenir compte d'enfants à charge)
   **/
  double nbUCCouple() {
    double nbUC = 1;
    int conjoint = X.conjoint[age];
    if (conjoint && pop[conjoint].est_present(X.date(age))) {
      nbUC += 0.5;
    }
    return nbUC;
  }

  /**
   * \fn pension_age(int age, int regime)
   * \brief Renvoie la pension perçue à un âge donné, à un régime donné (tout
   * régime par défaut) \param age âge \param regime régime servant la pension
   */
  double pension_age(int age, int regime);

  /**
   * \fn pension_prorat(double pension, double daterevalo, double RevaloReg,
   * double partavtliq=0) \brief Renvoie la pension percue au cours de l'année
   * en tenant compte de la proratisation et de la revalorisation \param pension
   * montant de la pension au 31/12 \param daterevalo date de la relavoration
   * des pensions \param RevaloReg valeur de la revalorisation \param partavtliq
   * période de l'année avant la liquidation des droits retraites
   */
  double pension_prorat(double pension, double daterevalo, double RevaloReg,
                        double partavtliq = 0);

  /**
   * \fn pension_prorat(int age, int regime)
   * \brief Renvoie la pension percue au cours de l'année en tenant compte de la
   * proratisation et de la revalorisation \param age Âge de l'individu \param
   * regime régime servant la pension
   */
  double pension_prorat(int age, int regime);
};

// Destinie 2
// Copyright © 2005-2018, Institut national de la statistique et des études
// économiques This program is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
