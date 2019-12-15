#include "Retraite.h"
#include "Indiv.h"
#include "OutilsComp.h"

Retraite::Retraite(Indiv &X, int t) : X(X), age(X.age(t)) {
  revaloDir(t);
  revaloDer(t);
}

void Retraite::minvieil(int t) {
  int age = X.age(t);
  // Minimum vieillesse du ménage
  if (age >= 65 && X.est_persRef(t)) {
    double revenu_men = 0;
    revenu_men += revenu();

    if (X.matri[age] == MARIE && present(X.conjoint[age], t)) {
      // couple
      Indiv &Y = pop[X.conjoint[age]];
      revenu_men += Y.retr->revenu();
      min_vieil = max(0.0, M->MinVieil2[t] - revenu_men);

    } else {
      // personne seule
      min_vieil = max(0.0, M->MinVieil1[t] - revenu_men);
    }
  }

  if (min_vieil > 0 && t <= X.dateMinFlux) {
    X.dateMinFlux = t;
  }

  retraite_tot = pension_tot + rev_tot;
}

// Fonction SimDir :
void Retraite::SimDir(int age) {
  Leg l = Leg(X, age, min(X.anaiss + age, options->anLeg));

  double pas = (age < 55)
                   ? options->pas2
                   : (age > l.AgeAnnDecRG) ? options->pas2 : options->pas1;

  double ageLegTest = (age < 55)
                          ? l.AgeMinFP
                          : (age > l.AgeAnnDecRG) ? l.AgeAnnDecRG : l.AgeMinRG;

  int moistest = arr(12 * ageLegTest + X.moisnaiss + 1) % arr(12 * pas);

  double agetest = arr_mois(age, moistest - X.moisnaiss - 1);

  if (age > 70)
    return;

  double ageMax;

  // boucle infra-annuel
  while (moistest < 12 && !totliq) {

    // on teste la première, et éventuellement la seconde liquidation
    ptr<DroitsRetr> dr = make_shared<DroitsRetr>(X, l, agetest);

    ageMax = dr->AgeMax();
    if (!primoliq && TestLiq(X, *dr, l, agetest)) {
      primoliq = dr;
      liq = dr;
      totliq = (dr->liq) ? dr : NULL;
      if (options->ecrit_dr_test)
        ecriture_droitsRetr(X, *dr);
    } else if (primoliq && TestSecondLiq(X, *dr, l, agetest)) {
      totliq = dr;
      liq = dr;
      if (options->ecrit_dr_test)
        ecriture_droitsRetr(X, *dr);
    } else {
      if (options->ecrit_dr_test)
        ecriture_droitsRetr(X, *dr);
    }

    moistest += (arr_mois(l.AgeMinFP - agetest) > 0)
                    ? arr(12 * min(pas, l.AgeMinFP - agetest))
                    : (arr_mois(l.AgeMinRG - agetest) > 0)
                          ? arr(12 * min(pas, l.AgeMinRG - agetest))
                          : (arr_mois(l.AgeAnnDecRG - agetest) > 0)
                                ? arr(12 * min(pas, l.AgeAnnDecRG - agetest))
                                : (arr_mois(ageMax - agetest) > 0)
                                      ? arr(12 * min(pas, ageMax - agetest))
                                      : arr(12 * pas);

    agetest = arr_mois(age, moistest - X.moisnaiss - 1);
  }

  revaloDir(
      X.date(age)); // Permet entre autres choses de mettre à jour les
                    // différents attributs de pension de l'objet Retraite.
}

void Retraite::tx_cotis_sur_les_retraites(int t) {
  int age = X.age(t);
  if (X.matri[age] != MARIE) {
    if (retraite_tot <= M->SeuilExoCSG[t])
      txCSG = 0;
    else if (retraite_tot <= M->SeuilTxReduitCSG[t])
      txCSG = M->TauxCSGRetMin[t];
    else {
      txCSG = M->TauxCSGRetFort[t];
      txMal = M->TauxMalComp[t];
    }
  } else if (pop[X.conjoint[age]]
                 .retr
                 ->primoliq) { // Pour le calcul du revenu du foyer fiscal, le
                               // conjoint pris en compte est celui au 31/12 de
                               // l'année de déclaration. Les couples sont
                               // supposés remplir une déclaration commune.
    double pensionConjoint = pop[X.conjoint[age]].retr->pension_age(
        pop[X.conjoint[age]].age(t), REG_TOT);
    if ((retraite_tot + pensionConjoint) <= M->SeuilExoCSG2[t])
      txCSG = 0;
    else if ((retraite_tot + pensionConjoint) <= M->SeuilTxReduitCSG[t])
      txCSG = M->TauxCSGRetMin[t];
    else {
      txCSG = M->TauxCSGRetFort[t];
      txMal = M->TauxMalComp[t];
    }
  } else if (retraite_tot +
                 pop[X.conjoint[age]].salaires[pop[X.conjoint[age]].age(t)] <=
             M->SeuilExoCSG2[t]) {
    txCSG = 0;
  } else if (retraite_tot +
                 pop[X.conjoint[age]].salaires[pop[X.conjoint[age]].age(t)] <=
             M->SeuilTxReduitCSG2[t]) {
    txCSG = M->TauxCSGRetMin[t];
  } else {
    txCSG = M->TauxCSGRetFort[t];
    txMal = M->TauxMalComp[t];
  }
}

void Retraite::retraite_nette(int t) {
  age = X.age(t);
  if ((liq && age >= liq->ageprimoliq) || (totliq && age >= totliq->ageliq))
    tx_cotis_sur_les_retraites(t);
  retraite_net =
      retraite_tot -
      (retraite_tot * txCSG +
       (pension_ar + pension_ag + pension_ag_ar + rev_ar + rev_ag + rev_ag_ar) *
           txMal);
}

void Retraite::revaloDir(int t) {
  age = X.age(t);
  pension_tot = 0;
  pension_rg = 0;
  pension_fp = 0;
  pension_in = 0;
  pension_ar = 0;
  pension_ag = 0;
  min_vieil = 0;
  pension_ag_ar = 0;

  if (liq && age >= liq->ageprimoliq) {
    int t_liq = X.date(liq->ageprimoliq); // Année de la liquidation

    pension_fp = liq->pension_fp * M->RevaloCumFP[t] / M->RevaloCumFP[t_liq];
  }

  if (totliq && age >= totliq->ageliq) {
    int t_liq = X.date(liq->ageliq); // Année de la liquidation totale
    double invCoeffTmp = 1.0;
    int anneeAnnCoeffTemp = int_mois(
        X.anaiss + liq->ageAnnulCoeffTemp,
        X.moisnaiss + 1); // année de l'annulation du coefficient temporaire
    /*if(!options->NoCoeffTemp &&  arr_mois(age - liq->ageAnnulCoeffTemp, 11 -
    X.moisnaiss) < 0) { coeffTmp = liq->coeffTemp;
    }*/

    if ((!options->NoCoeffTemp) && (!options->NoAccordAgircArrco) &&
        (t >= 119) && (X.anaiss >= 1957)) {
      if ((1900 + t) < anneeAnnCoeffTemp) {
        invCoeffTmp = 1.0;
      } else if ((1900 + t) == anneeAnnCoeffTemp) {
        double partAvtAnnulCoeffTemp =
            arr_mois(X.anaiss + liq->ageAnnulCoeffTemp, X.moisnaiss + 1) -
            max(arr_mois(X.anaiss + liq->agefin_totliq, X.moisnaiss + 1),
                double(anneeAnnCoeffTemp)); // part de l'année avant annulation
                                            // du coeff temp
        double partavtliq = partavttotliq(t);
        if (partavtliq < 1) {
          partAvtAnnulCoeffTemp = partAvtAnnulCoeffTemp / (1 - partavtliq);
        }
        invCoeffTmp = (partAvtAnnulCoeffTemp * liq->coeffTemp + 1 -
                       partAvtAnnulCoeffTemp) /
                      liq->coeffTemp;
      } else {
        invCoeffTmp = 1 / liq->coeffTemp;
      }
    }

    pension_rg = liq->pension_rg * M->RevaloCumRG[t] / M->RevaloCumRG[t_liq];
    pension_in = liq->pension_in * M->RevaloCumRG[t] / M->RevaloCumRG[t_liq];
    pension_ar =
        liq->pension_ar * M->ValPtARRCO[t] / M->ValPtARRCO[t_liq] * invCoeffTmp;
    pension_ag =
        liq->pension_ag * M->ValPtAGIRC[t] / M->ValPtAGIRC[t_liq] * invCoeffTmp;
    pension_ag_ar = liq->pension_ag_ar * M->ValPtAGIRC_ARRCO[t] /
                    M->ValPtAGIRC_ARRCO[t_liq] * invCoeffTmp;
  }

  pension_tot = pension_fp + pension_rg + pension_in + pension_ar + pension_ag +
                pension_ag_ar;

  retraite_tot = pension_tot + rev_tot;
}

void Retraite::SimDer(Indiv &Y, int t) {
  liqrev = make_shared<Reversion>(X, Y, t, options->anLeg);
  // revalo(t);
}

void Retraite::revaloDer(int t) {
  age = X.age(t);

  rev_tot = 0;
  rev_rg = 0;
  rev_fp = 0;
  rev_in = 0;
  rev_ar = 0;
  rev_ag = 0;
  rev_ag_ar = 0;

  min_vieil = 0;

  if (liqrev && age >= liqrev->ageliq_rev) {
    int t_liq = X.date(liqrev->ageliq_rev); // Année de la liquidation

    rev_rg = liqrev->rev_rg * M->RevaloCumRG[t] / M->RevaloCumRG[t_liq];
    rev_fp = liqrev->rev_fp * M->RevaloCumFP[t] / M->RevaloCumFP[t_liq];
    rev_in = liqrev->rev_in * M->RevaloCumRG[t] / M->RevaloCumRG[t_liq];
    rev_ar = liqrev->rev_ar * M->ValPtARRCO[t] / M->ValPtARRCO[t_liq];
    rev_ag_ar =
        liqrev->rev_ag_ar * M->ValPtAGIRC_ARRCO[t] / M->ValPtAGIRC_ARRCO[t_liq];
    rev_ag = liqrev->rev_ag * M->ValPtAGIRC[t] / M->ValPtAGIRC[t_liq];

    // Condition d'âge  pour l'AGIRC on n'implemente pas la decote si la pension
    // de reversion est percue avant 60ans sans toucher rev_rg
    if (age < 55 &&
        !in(X.statuts[age], {S_INVAL, S_INVALRG, S_INVALFP, S_INVALIND}) &&
        liqrev->nbEnfCharge <= 1) {
      rev_rg = 0.0;
      rev_in = 0.0;
      rev_ar = 0.0;
      rev_ag_ar = 0.0;
    }

    if (age < 60 &&
        !in(X.statuts[age], {S_INVAL, S_INVALRG, S_INVALFP, S_INVALIND}) &&
        liqrev->nbEnfCharge <= 1) {
      rev_ag = 0.0;
    }
    // Introduction de la condition de statut marital 14/10/2015 pour les
    // complémentaires et fonctionnaires
    int matri = X.matri[age];

    if (matri == MARIE) {
      liqrev->rev_fp = 0.0;
      liqrev->rev_ar = 0.0;
      liqrev->rev_ag = 0.0;
      liqrev->rev_ag_ar = 0.0;
      rev_fp = 0.0;
      rev_ar = 0.0;
      rev_ag = 0.0;
      rev_ag_ar = 0.0;
    }

    double tauxmajo_rg = 0;
    double tauxmajo_in = 0;
    if (X.nb_enf(X.age(t)) >= 3) {
      tauxmajo_rg = 0.1;
      tauxmajo_in = 0.1;
    }

    // recalcul du plafond pour la pension au rg et in jusqu'au depart en
    // retraite definitif
    if (!totliq) {
      // Amputation des reversions RG et ind. si ressources dépassent
      // plafond
      double excedent = 0;
      double ratio_rg = 0;
      if ((rev_rg + rev_in) > 0) {
        if (matri == MARIE) {
          excedent = X.retr->revenuReversionCouple() +
                     rev_in / (1 + tauxmajo_in) + rev_rg / (1 + tauxmajo_rg) -
                     1.6 * M->PlafRevRG[t];
        } else {
          excedent = X.retr->revenuReversion() + rev_in / (1 + tauxmajo_in) +
                     rev_rg / (1 + tauxmajo_rg) - M->PlafRevRG[t];
        } // on vérifie la condition de ressources sans l'ajout éventuel de la
          // majoration pour enfants dans Reversion.cpp
        ratio_rg = rev_rg / (rev_rg + rev_in);
        if (excedent > 0) {
          rev_rg = (1 + tauxmajo_rg) *
                   max(0.0, rev_rg / (1 + tauxmajo_rg) - excedent * ratio_rg);
          rev_in = (1 + tauxmajo_in) * max(0.0, rev_in / (1 + tauxmajo_in) -
                                                    excedent * (1 - ratio_rg));
        }
      }
    }
    // si la liquidation totale a lieu ou a deja eu lieu on plafonne
    if ((totliq && age == totliq->ageliq) ||
        (totliq && age == liqrev->ageliq_rev)) {
      // Amputation des reversions RG et ind. si ressources dépassent
      // plafond
      double excedent = 0;
      double ratio_rg = 0;
      if ((rev_rg + rev_in) > 0) {
        if (matri == MARIE) {
          excedent = X.retr->revenuReversionCouple() +
                     rev_in / (1 + tauxmajo_in) + rev_rg / (1 + tauxmajo_rg) -
                     1.6 * M->PlafRevRG[t];
        } else {
          excedent = X.retr->revenuReversion() + rev_in / (1 + tauxmajo_in) +
                     rev_rg / (1 + tauxmajo_rg) - M->PlafRevRG[t];
        }
        ratio_rg = rev_rg / (rev_rg + rev_in);
        if (excedent > 0) {
          rev_rg = (1 + tauxmajo_rg) *
                   max(0.0, rev_rg / (1 + tauxmajo_rg) - excedent * ratio_rg);
          rev_in = (1 + tauxmajo_in) * max(0.0, rev_in / (1 + tauxmajo_in) -
                                                    excedent * (1 - ratio_rg));
          liqrev->rev_rg = rev_rg * M->RevaloCumRG[t_liq] / M->RevaloCumRG[t];
          liqrev->rev_in = rev_in * M->RevaloCumRG[t_liq] / M->RevaloCumRG[t];
        }
      }
    }

    /*if(totliq && (age>=l.AgeAnnDecRG) && matri!=MARIE) { // il reste à
    rajouter la condition sur le plafond rev_rg*=(1+11.1/100.0);
          rev_in*=(1+11.1/100.0);
    } */
  }
  rev_tot = rev_fp + rev_rg + rev_in + rev_ar + rev_ag + rev_ag_ar;
  retraite_tot = pension_tot + rev_tot;
}

double Retraite::pension_age(int age, int regime) {
  int annee = X.anaiss % 1900 + age;
  double resultat = 0;

  if (liq && age >= liq->ageprimoliq && regime == REG_FP) {
    int t_liq = X.date(liq->ageprimoliq);

    resultat = liq->pension_fp * M->RevaloCumFP[annee] / M->RevaloCumFP[t_liq];
  }

  if (totliq && age >= totliq->ageliq) {
    int t_liq = X.date(liq->ageliq);
    int ageAnnCoeffTempEnt = int_mois(liq->ageAnnulCoeffTemp, X.moisnaiss + 1);
    double invCoeffTemp = 1.0;
    if ((!options->NoCoeffTemp) && (!options->NoAccordAgircArrco) &&
        (annee >= 119) && (X.anaiss >= 1957)) {
      invCoeffTemp = (age < ageAnnCoeffTempEnt) ? 1.0 : 1 / liq->coeffTemp;
    }

    if (regime == REG_RG) {
      resultat =
          liq->pension_rg * M->RevaloCumRG[annee] / M->RevaloCumRG[t_liq];
    }
    if (regime == REG_IN) {
      resultat =
          liq->pension_in * M->RevaloCumRG[annee] / M->RevaloCumRG[t_liq];
    }
    if (regime == REG_AR) {
      resultat = liq->pension_ar * invCoeffTemp * M->ValPtARRCO[annee] /
                 M->ValPtARRCO[t_liq];
    }
    if (regime == REG_AG) {
      resultat = liq->pension_ag * invCoeffTemp * M->ValPtAGIRC[annee] /
                 M->ValPtAGIRC[t_liq];
    }
    if (regime == REG_AGIRC_ARRCO) {
      resultat = liq->pension_ag_ar * invCoeffTemp *
                 M->ValPtAGIRC_ARRCO[annee] / M->ValPtAGIRC_ARRCO[t_liq];
    }
    if (regime == REG_TOT) {
      resultat =
          liq->pension_fp * M->RevaloCumFP[annee] /
              M->RevaloCumFP[X.date(liq->ageprimoliq)] +
          liq->pension_rg * M->RevaloCumRG[annee] / M->RevaloCumRG[t_liq] +
          liq->pension_in * M->RevaloCumRG[annee] / M->RevaloCumRG[t_liq] +
          liq->pension_ar * invCoeffTemp * M->ValPtARRCO[annee] /
              M->ValPtARRCO[t_liq] +
          liq->pension_ag * invCoeffTemp * M->ValPtAGIRC[annee] /
              M->ValPtAGIRC[t_liq] +
          liq->pension_ag_ar * invCoeffTemp * M->ValPtAGIRC_ARRCO[annee] /
              M->ValPtAGIRC_ARRCO[t_liq];
    }
  }
  return resultat;
}

double Retraite::pension_prorat(double pension, double daterevalo,
                                double RevaloReg, double partavtliq) {
  return pension * (1 / RevaloReg * (max(daterevalo, partavtliq) - partavtliq) +
                    (1 - max(daterevalo, partavtliq)));
}

double Retraite::pension_prorat(int age, int regime) {
  int t = X.date(age);
  double pens(0);
  if ((regime == REG_TOT || regime == REG_RG) && pension_rg > 0)
    pens += pension_prorat(pension_rg, M->daterevalobase[t], M->RevaloRG[t],
                           partavttotliq(t));
  if ((regime == REG_TOT || regime == REG_IN) && pension_in > 0)
    pens += pension_prorat(pension_in, M->daterevalobase[t], M->RevaloRG[t],
                           partavttotliq(t));
  if ((regime == REG_TOT || regime == REG_FP) && pension_fp > 0)
    pens += pension_prorat(pension_fp, M->daterevalobase[t], M->RevaloFP[t],
                           partavtliq(t));
  if ((regime == REG_TOT || regime == REG_AR) && pension_ar > 0)
    pens += pension_prorat(pension_ar, M->daterevalocomp[t],
                           M->ValPtARRCO[t - 1] / M->ValPtARRCO[t],
                           partavttotliq(t));
  if ((regime == REG_TOT || regime == REG_AG) && pension_ag > 0)
    pens += pension_prorat(pension_ag, M->daterevalocomp[t],
                           M->ValPtAGIRC[t - 1] / M->ValPtAGIRC[t],
                           partavttotliq(t));
  if ((regime == REG_TOT || regime == REG_AGIRC_ARRCO) && pension_ag_ar > 0)
    pens += pension_prorat(pension_ag_ar, M->daterevalocomp[t],
                           M->ValPtAGIRC_ARRCO[t - 1] / M->ValPtAGIRC_ARRCO[t],
                           partavttotliq(t));
  return pens;
}

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
