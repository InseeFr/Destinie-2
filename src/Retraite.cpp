#include "Retraite.h"
#include "Indiv.h"
#include "OutilsComp.h"

Retraite::Retraite(Indiv& X, int t) : X(X),   age(X.age(t))                         
{
  revaloDir(t);
  revaloDer(t);
}


void Retraite::minvieil(int t) {
  int age = X.age(t);
  // Minimum vieillesse du ménage
  if (age >=65 && X.est_persRef(t)) {
    double revenu_men = 0;
    revenu_men += revenu();
    
    if(X.matri[age]==MARIE && present(X.conjoint[age],t)) {
      // couple
      Indiv&  Y = pop[X.conjoint[age]];
      revenu_men +=  Y.retr->revenu();
      min_vieil=max(0.0, M->MinVieil2[t]-revenu_men);
      
    }
    else {
      // personne seule
      min_vieil=max(0.0, M->MinVieil1[t]-revenu_men); 
    }  
  }
  
  if(min_vieil>0 && t<=X.dateMinFlux) { X.dateMinFlux=t;}
  
  retraite_tot = pension_tot + rev_tot;
  
}



// Fonction SimDir :
void Retraite::SimDir(int age) {
  Leg l = Leg(X, age, min(X.anaiss+age,options->anLeg));
  
  double pas = (age < 55)            ? options->pas2 : 
    (age > l.AgeAnnDecRG) ? options->pas2 : 
    options->pas1 ;
  
  double ageLegTest = (age < 55)            ? l.AgeMinFP    : 
    (age > l.AgeAnnDecRG) ? l.AgeAnnDecRG : 
    l.AgeMinRG    ;
  
  int moistest = arr(12*ageLegTest + X.moisnaiss + 1) % arr(12*pas);
  
  double agetest  = arr_mois(age, moistest - X.moisnaiss - 1);
  
  if(age > 70)    
    return;
  
  double ageMax;
  
  // boucle infra-annuel
  while(moistest < 12 && !totliq) {
    
    // on teste la première, et éventuellement la seconde liquidation
    ptr<DroitsRetr> dr = make_shared<DroitsRetr>(X,l,agetest);
    
    ageMax = dr->AgeMax();
    if(!primoliq && TestLiq(X,*dr, l, agetest)) {
      primoliq = dr;
      liq = dr;
      totliq   = (dr->liq) ? dr : NULL;
      if(options->ecrit_dr_test) ecriture_droitsRetr(X,*dr);
    }
    else if(primoliq && TestSecondLiq(X, *dr, l, agetest)) {
      totliq = dr;
      liq = dr;
      if(options->ecrit_dr_test) ecriture_droitsRetr(X,*dr);
    }
    else {
      if(options->ecrit_dr_test) ecriture_droitsRetr(X,*dr);
    }
    
    moistest += 
      (arr_mois(l.AgeMinFP    - agetest) > 0)  ? arr(12*min(pas, l.AgeMinFP   - agetest)) :
      (arr_mois(l.AgeMinRG    - agetest) > 0)  ? arr(12*min(pas, l.AgeMinRG   - agetest)) :
      (arr_mois(l.AgeAnnDecRG - agetest) > 0)  ? arr(12*min(pas, l.AgeAnnDecRG- agetest)) :
      (arr_mois(ageMax        - agetest) > 0)  ? arr(12*min(pas, ageMax     - agetest))   :
      arr(12*pas);
    
    agetest  = arr_mois(age, moistest- X.moisnaiss - 1);
  }
  
  revaloDir(X.date(age)); // Permet entre autres choses de mettre à jour les différents attributs de pension de l'objet Retraite.
}


void Retraite::tx_cotis_sur_les_retraites(int t)
{ 	int age=X.age(t);
  if(X.matri[age] != MARIE) {
    if(retraite_tot<= M->SeuilExoCSG[t]) 
      txCSG=0;
    else if(retraite_tot <= M->SeuilTxReduitCSG[t]) 
      txCSG=M->TauxCSGRetMin[t];
    else {txCSG=M->TauxCSGRetFort[t];
      txMal=M->TauxMalComp[t];}
  }
  else if(pop[X.conjoint[age]].retr->primoliq) { // Pour le calcul du revenu du foyer fiscal, le conjoint pris en compte est celui au 31/12 de l'année de déclaration. Les couples sont supposés remplir une déclaration commune.
    double pensionConjoint = pop[X.conjoint[age]].retr->pension_age(pop[X.conjoint[age]].age(t), REG_TOT);
    if((retraite_tot+ pensionConjoint) <= M->SeuilExoCSG2[t]) 
      txCSG=0;
    else if((retraite_tot + pensionConjoint) <= M->SeuilTxReduitCSG2[t]) 
      txCSG=M->TauxCSGRetMin[t];
    else {txCSG=M->TauxCSGRetFort[t];
      txMal=M->TauxMalComp[t];}} 
  else if(retraite_tot + pop[X.conjoint[age]].salaires[pop[X.conjoint[age]].age(t)] <= M->SeuilExoCSG2[t]) {
    txCSG=0;
  }	
  else if(retraite_tot + pop[X.conjoint[age]].salaires[pop[X.conjoint[age]].age(t)] <= M->SeuilTxReduitCSG2[t]) {
    txCSG=M->TauxCSGRetMin[t];
  }
  else {txCSG=M->TauxCSGRetFort[t];
    txMal=M->TauxMalComp[t];}
}

void Retraite::retraite_nette(int t)
{	age = X.age(t);
  if((liq && age >= liq->ageprimoliq)||(totliq && age >= totliq->ageliq)) tx_cotis_sur_les_retraites(t);
  retraite_net= retraite_tot-(retraite_tot*txCSG+(pension_ar+pension_ag+pension_ag_ar+rev_ar+rev_ag+rev_ag_ar)*txMal);
}

void Retraite::revaloDir(int t)
{
  age = X.age(t);
  pension_tot=0;
  pension_rg=0;
  pension_fp=0;
  pension_in=0;
  pension_ar=0;
  pension_ag=0;
  min_vieil=0;
  pension_ag_ar=0;
  
  if(liq && age >= liq->ageprimoliq) {
    int t_liq = X.date(liq->ageprimoliq); // Année de la liquidation
    
    pension_fp = liq->pension_fp  * M->RevaloCumFP[t] / M->RevaloCumFP[t_liq];
  }
  
  if(totliq && age >= totliq->ageliq) {
    int t_liq = X.date(liq->ageliq); // Année de la liquidation totale
    double invCoeffTmp = 1.0;
    int anneeAnnCoeffTemp = int_mois(X.anaiss + liq->ageAnnulCoeffTemp,X.moisnaiss+1); // année de l'annulation du coefficient temporaire
    /*if(!options->NoCoeffTemp &&  arr_mois(age - liq->ageAnnulCoeffTemp, 11 - X.moisnaiss) < 0) {
     coeffTmp = liq->coeffTemp;
    }*/
    int an_leg = totliq->l.an_leg;
    if(
      (!options->NoCoeffTemp) &&
        (t >= 119) && (X.anaiss >= 1957) &&
        ((an_leg >= 2015 && an_leg <= 2022) || (an_leg >= 2023 && t <= 123))
    ) {
      if((1900 + t) < anneeAnnCoeffTemp) {
        invCoeffTmp = 1.0;
      } else if((1900+t) == anneeAnnCoeffTemp) {
        double partAvtAnnulCoeffTemp = arr_mois(X.anaiss + liq->ageAnnulCoeffTemp,X.moisnaiss+1) - max(arr_mois(X.anaiss + liq->agefin_totliq,X.moisnaiss+1),double(anneeAnnCoeffTemp)); // part de l'année avant annulation du coeff temp
        double partavtliq= partavttotliq(t);
        if(partavtliq<1) {partAvtAnnulCoeffTemp = partAvtAnnulCoeffTemp/(1-partavtliq);}
        invCoeffTmp = (partAvtAnnulCoeffTemp*liq->coeffTemp + 1-partAvtAnnulCoeffTemp)/liq->coeffTemp;
      } else {
        invCoeffTmp=1/liq->coeffTemp;
      }
    }
    
    pension_rg = liq->pension_rg * M->RevaloCumRG[t] / M->RevaloCumRG[t_liq];
    pension_in = liq->pension_in * M->RevaloCumRG[t] / M->RevaloCumRG[t_liq];
    pension_ar = liq->pension_ar * M->ValPtARRCO[t]  / M->ValPtARRCO[t_liq] * invCoeffTmp;
    pension_ag = liq->pension_ag * M->ValPtAGIRC[t]  / M->ValPtAGIRC[t_liq] * invCoeffTmp; 
    pension_ag_ar = liq->pension_ag_ar * M->ValPtAGIRC_ARRCO[t]  / M->ValPtAGIRC_ARRCO[t_liq] * invCoeffTmp;
  }
  
  pension_tot = pension_fp + pension_rg + pension_in + pension_ar + pension_ag+ pension_ag_ar;
  
  
  retraite_tot = pension_tot + rev_tot;
}

void Retraite::SimDer(Indiv& Y, int t) {
  liqrevs.emplace_back(make_shared<Reversion>(X, Y, t, options->anLeg));
  //revalo(t);
}

void Retraite::revaloDer(int t)
{
  age = X.age(t);
  retraite_tot = 0.0;
  rev_tot = 0.0;
  rev_rg = 0.0;
  rev_fp = 0.0;
  rev_in = 0.0;
  rev_ar = 0.0;
  rev_ag = 0.0;
  rev_ag_ar = 0.0;
  min_vieil = 0.0;
  if (age < 0) return;
  
  // Somme des réversions par régime (hors majoration et ecrêtement)
  double rev_rg_hors_modif = 0.0;
  double rev_in_hors_modif = 0.0;

  int matri = X.matri[age];
  int max_ageliq_rev_rg_in = -1; // age de liquidation totale des droits de réversion
  for (auto& liqrev : liqrevs) {
    if (age < liqrev->ageliq_rev) continue;
    int t_liq = X.date(liqrev->ageliq_rev); // Année de la liquidation
    
    double liq_rev_rg = liqrev->rev_rg  * M->RevaloCumRG[t] / M->RevaloCumRG[t_liq];
    double liq_rev_fp = liqrev->rev_fp  * M->RevaloCumFP[t] / M->RevaloCumFP[t_liq];
    double liq_rev_in = liqrev->rev_in  * M->RevaloCumRG[t] / M->RevaloCumRG[t_liq];
    double liq_rev_ar = liqrev->rev_ar * M->ValPtARRCO[t]  / M->ValPtARRCO[t_liq];
    double liq_rev_ag = liqrev->rev_ag  * M->ValPtAGIRC[t]  / M->ValPtAGIRC[t_liq];
    double liq_rev_ag_ar = liqrev->rev_ag_ar   * M->ValPtAGIRC_ARRCO[t]  / M->ValPtAGIRC_ARRCO[t_liq];
    
    //Condition d'âge  pour l'AGIRC on n'implemente pas la decote si la pension de reversion est percue avant 60ans sans toucher rev_rg
    if (age < 55 && !in(X.statuts[age],{S_INVAL, S_INVALRG, S_INVALFP, S_INVALIND}) && liqrev->nbEnfCharge <= 1){
      liq_rev_rg = 0.0;
      liq_rev_in = 0.0;
      liq_rev_ar = 0.0;
      liq_rev_ag_ar = 0.0;
    }
    
    if ( (age < 55 || (age < 60 && (t_liq < 119 || options->anLeg < 2017)))  && !in(X.statuts[age], {S_INVAL, S_INVALRG, S_INVALFP, S_INVALIND}) && liqrev->nbEnfCharge <= 1){
      liq_rev_ag = 0.0;
    }
    
    // Introduction de la condition de statut marital 14/10/2015 pour les complémentaires et fonctionnaires
    if (matri == MARIE){
      liqrev->rev_ar = 0.0;
      liqrev->rev_ag = 0.0;
      liqrev->rev_ag_ar = 0.0;
      liq_rev_fp = 0.0;
      liq_rev_ar = 0.0;
      liq_rev_ag = 0.0;
      liq_rev_ag_ar = 0.0;
    }
    
    int idConj = liqrev->idConj;
    r_assert(idConj != 0);
    
    if (t == AN_BASE && X.matri[age] == VEUF) {
      X.pseudo_conjoint = idConj;
    }
    
    // Suppression du droit à réversion en cas de remariage avant le décès de l'ex-conjoint
    if ((liqrev->rev_ar > 0.0 || liqrev->rev_ag > 0.0 || liqrev->rev_ag_ar > 0.0) && X.pseudo_conjoint != idConj) {
      for (int t2 = t_liq; t2 > X.anaiss - 1900 + 1; t2--) {
        int age2 = X.age(t2);
        int matri2 = X.matri[age2];
        if (matri2 != MARIE) continue;
        
        if (X.conjoint[age2] != idConj) {
          liqrev->rev_ar = 0.0;
          liqrev->rev_ag = 0.0;
          liqrev->rev_ag_ar = 0.0;
          liq_rev_ar = 0.0;
          liq_rev_ag = 0.0;
          liq_rev_ag_ar = 0.0;
        }

        break;
      }
    }
    
    if (liqrev->rev_fp > 0.0) {
      // Doit avoir acquis des droits dérivés avec le conjoint en question
      if (age == liqrev->ageliq_rev && !a_acquis_droits_derives_fp(idConj)) {
        liqrev->rev_fp = 0.0;
        liq_rev_fp = 0.0;
      }
      // Pas de cumul de droits dérivés pour les fonctionnaires
      if (rev_fp > 0.0) {
        liqrev->rev_fp = 0.0;
        liq_rev_fp = 0.0;
      }
      // Ne doit pas avoir acquis de droits avec un autre conjoint
      // On teste seulement sur le conjoint actuel
      if (X.conjoint[age] != 0 && X.conjoint[age] != idConj) {
        int idConj_actuel = X.conjoint[age];
        if(a_acquis_droits_derives_fp(idConj_actuel)) {
          liqrev->rev_fp = 0.0;
          liq_rev_fp = 0.0;
        }
      }
      
    }

    if (liq_rev_rg > 0.0 || liq_rev_in > 0.0) {
      max_ageliq_rev_rg_in = max(max_ageliq_rev_rg_in, liqrev->ageliq_rev);
    }

    // Calcul de la proratisation à la durée du mariage
    Indiv& Y = pop[idConj];
    double part_duree_union_vivant = 1.0;
    double part_duree_union = 1.0;

    // On ne proratise pas la réversion issue d'un pseudo-conjoint
    if (X.pseudo_conjoint != idConj) {
      int duree_union_tot = 0;
      int duree_union_tot_vivant = 0;
      int duree_union = 0;
      for (int a : range(Y.ageMax)) {
        if (Y.conjoint[a] == 0) continue;
        if (Y.matri[a] == MARIE) {
          duree_union_tot++;
          duree_union_tot_vivant += (pop[Y.conjoint[a]].est_present(t));
          duree_union += (Y.conjoint[a] == X.Id);
        }
      }
      if(duree_union_tot_vivant != 0) {
        part_duree_union_vivant = duree_union / static_cast<double>(duree_union_tot_vivant);
      }
      if(duree_union_tot != 0) {
        part_duree_union = duree_union / static_cast<double>(duree_union_tot);
      }
    }
    
    // Calcul de la proratisation à la durée
    rev_rg += liq_rev_rg * part_duree_union_vivant;
    rev_rg_hors_modif += liq_rev_rg * part_duree_union_vivant;
    rev_in += liq_rev_in * part_duree_union_vivant;
    rev_in_hors_modif += liq_rev_in * part_duree_union_vivant;
    rev_fp += liq_rev_fp * part_duree_union_vivant;
    rev_ar += liq_rev_ar * part_duree_union;
    rev_ag += liq_rev_ag * part_duree_union;
    rev_ag_ar += liq_rev_ag_ar * part_duree_union;
  }
  
  
  double tauxmajo_rg = 0.0;
  double tauxmajo_in = 0.0;
  if(X.nb_enf(X.age(t)) >= 3) {
    tauxmajo_rg = 0.1;
    tauxmajo_in = 0.1;
  }

 // Recalcul du plafond pour la pension au rg et in jusqu'au depart en retraite definitif
 if (!totliq || (totliq && ((age == totliq->ageliq) || (age == max_ageliq_rev_rg_in)))) {
   // Amputation des reversions RG et ind. si ressources dépassent plafond
   double excedent = 0;
   if ((rev_rg + rev_in) > 0) {
     if (matri == MARIE) {
       excedent = X.retr->revenuReversionCouple() + rev_in / (1 + tauxmajo_in) + rev_rg / (1 + tauxmajo_rg) - 1.6 * M->PlafRevRG[t];
     } else {
       excedent = X.retr->revenuReversion() + rev_in / (1 + tauxmajo_in) + rev_rg / (1 + tauxmajo_rg) - M->PlafRevRG[t];
     }  // on vérifie la condition de ressources sans l'ajout éventuel de la majoration pour enfants dans Reversion.cpp
     
     if (excedent > 0) {
       double ratio_rg = rev_rg / (rev_rg + rev_in);
       rev_rg = (1 + tauxmajo_rg) * max(0.0, rev_rg / (1 + tauxmajo_rg) - excedent * ratio_rg);
       rev_in = (1 + tauxmajo_in) * max(0.0, rev_in / (1 + tauxmajo_in) - excedent * (1 - ratio_rg));
     }
   }
 }
  
  if (totliq) {
    // Majoration pour atteinte de l'AAD
    // Avec un plafond de ressources spécifique
    // L353-6 CSS, D353-4 CSS
    double ressources = X.retr->revenuReversion() + rev_in / (1 + tauxmajo_in) + rev_rg / (1 + tauxmajo_rg) + rev_ag + rev_ar;
    if((age >= totliq->l.AgeAnnDecRG) && (ressources < M->PlafMajoRevRG[t]) && (t >= 110) && (options->anLeg >= 2010)) {
      double taux_majo_age = 0.111;
      rev_rg *= 1 + taux_majo_age;
      rev_in *= 1 + taux_majo_age;
      if ((rev_rg + rev_in) > 0) {
        double excedent = X.retr->revenuReversion() + rev_in / (1 + tauxmajo_in) + rev_rg / (1 + tauxmajo_rg)
        + rev_ag + rev_ar - M->PlafMajoRevRG[t];
        if (excedent > 0) {
          double ratio_rg = rev_rg / (rev_rg + rev_in);
          rev_rg = (1 + tauxmajo_rg) * max(0.0, rev_rg / (1 + tauxmajo_rg) - excedent * ratio_rg);
          rev_in = (1 + tauxmajo_in) * max(0.0, rev_in / (1 + tauxmajo_in) - excedent * (1 - ratio_rg));
        }
      }
    }

    // Mise en cohérence des pensions de réversion dans liq_rev après les modifications de reversion
    if ((age == totliq->ageliq) || (age == max_ageliq_rev_rg_in)) {
      for (auto& liqrev : liqrevs) {
        int t_liq = X.date(liqrev->ageliq_rev);
        liqrev->rev_rg = rev_rg_hors_modif > 0 ? rev_rg / rev_rg_hors_modif * liqrev->rev_rg * M->RevaloCumRG[t_liq] / M->RevaloCumRG[t] : 0;
        liqrev->rev_in = rev_in_hors_modif > 0 ? rev_in / rev_in_hors_modif * liqrev->rev_in * M->RevaloCumRG[t_liq] / M->RevaloCumRG[t] : 0;
      }
    }
  }

  rev_tot = rev_fp + rev_rg + rev_in + rev_ar + rev_ag + rev_ag_ar;
  retraite_tot = pension_tot + rev_tot;
}


double Retraite::pension_age(int age, int regime) {
  int annee = X.anaiss%1900 + age;
  double resultat = 0.0;
  
  if(liq && age >= liq->ageprimoliq && regime==REG_FP) {
    int t_liq = X.date(liq->ageprimoliq);
    
    resultat = liq->pension_fp  * M->RevaloCumFP[annee] / M->RevaloCumFP[t_liq];
  }
  
  if(totliq && age >= totliq->ageliq) {
    int t_liq = X.date(liq->ageliq);
    int ageAnnCoeffTempEnt = int_mois(liq->ageAnnulCoeffTemp,X.moisnaiss+1);
    double invCoeffTemp = 1.0;
    int an_leg = liq->l.an_leg;
    if(
      (!options->NoCoeffTemp) &&
        (annee >= 119) && (X.anaiss >= 1957) &&
        ((an_leg >= 2015 && an_leg <= 2022) || (an_leg >= 2023 && t_liq <= 123))
    ) {
      invCoeffTemp = (age<ageAnnCoeffTempEnt) ? 1.0 : 1/liq->coeffTemp;
    }
    
    if(regime==REG_RG) {resultat = liq->pension_rg * M->RevaloCumRG[annee] / M->RevaloCumRG[t_liq];}
    if(regime==REG_IN) {resultat = liq->pension_in * M->RevaloCumRG[annee] / M->RevaloCumRG[t_liq];}
    if(regime==REG_AR) {resultat = liq->pension_ar * invCoeffTemp * M->ValPtARRCO[annee]  / M->ValPtARRCO[t_liq];}
    if(regime==REG_AG) {resultat = liq->pension_ag * invCoeffTemp * M->ValPtAGIRC[annee]  / M->ValPtAGIRC[t_liq];}
    if(regime==REG_AGIRC_ARRCO) {resultat = liq->pension_ag_ar * invCoeffTemp * M->ValPtAGIRC_ARRCO[annee]  / M->ValPtAGIRC_ARRCO[t_liq];}
    if(regime==REG_TOT) {resultat = liq->pension_fp  * M->RevaloCumFP[annee] / M->RevaloCumFP[X.date(liq->ageprimoliq)] 
      + liq->pension_rg * M->RevaloCumRG[annee] / M->RevaloCumRG[t_liq]
      + liq->pension_in * M->RevaloCumRG[annee] / M->RevaloCumRG[t_liq]
      + liq->pension_ar* invCoeffTemp * M->ValPtARRCO[annee]  / M->ValPtARRCO[t_liq]
      + liq->pension_ag * invCoeffTemp * M->ValPtAGIRC[annee]  / M->ValPtAGIRC[t_liq]
      + liq->pension_ag_ar  * invCoeffTemp * M->ValPtAGIRC_ARRCO[annee]  / M->ValPtAGIRC_ARRCO[t_liq];}
  }
  return resultat;
}

double Retraite::pension_prorat(double pension, double daterevalo, double RevaloReg, double partavtliq) {
  return pension * (1/RevaloReg*
                    (max(daterevalo, partavtliq)-partavtliq)
                      +(1-max(daterevalo, partavtliq)));
}

double Retraite::pension_prorat(int age, int regime) {
  int t = X.date(age);
  double pens(0);
  if((regime==REG_TOT || regime==REG_RG) && pension_rg>0) pens += pension_prorat(pension_rg, M->daterevalobase[t],  M->RevaloRG[t], partavttotliq(t));
  if((regime==REG_TOT || regime==REG_IN) && pension_in>0) pens += pension_prorat(pension_in, M->daterevalobase[t],  M->RevaloRG[t], partavttotliq(t));
  if((regime==REG_TOT || regime==REG_FP) && pension_fp>0) pens +=  pension_prorat(pension_fp, M->daterevalobase[t],  M->RevaloFP[t], partavtliq(t));
  if((regime==REG_TOT || regime==REG_AR) && pension_ar>0) pens +=  pension_prorat(pension_ar, M->daterevalocomp[t],   M->ValPtARRCO[t-1]/ M->ValPtARRCO[t], partavttotliq(t));
  if((regime==REG_TOT || regime==REG_AG) && pension_ag>0) pens +=  pension_prorat(pension_ag, M->daterevalocomp[t],   M->ValPtAGIRC[t-1]/ M->ValPtAGIRC[t], partavttotliq(t));
  if((regime==REG_TOT || regime==REG_AGIRC_ARRCO) && pension_ag_ar>0) pens +=  pension_prorat(pension_ag_ar, M->daterevalocomp[t],   M->ValPtAGIRC_ARRCO[t-1]/M->ValPtAGIRC_ARRCO[t], partavttotliq(t));
  return pens;    
}

//Tom : ajout de méthode(initialement implémentée pour l'exercice "demande CSR 2022")  -> calcule la pension nette à la liquidation
double Retraite::pensionliq_tot_nette_horsMal() {
  double resultat= 0;
  if(totliq && liq->agefin_totliq > 0) {
    tx_cotis_sur_les_retraites(X.date(liq->ageliq));
    resultat = liq->pension -(liq->pension*txCSG/*+(pension_ar+pension_ag+pension_ag_ar+rev_ar+rev_ag+rev_ag_ar)*txMal*/);
  }
  return resultat;
}

//Tom : ajout de méthode(initialement implémentée pour l'exercice "demande CSR 2022") -> calcule le taux de remplacement net à la liquidation : on autorise jusqu'à 10 ans d'inactivité pré-liquidation en continue
double Retraite::tauxRempl_net_horsMal_10() {
  double resultat=0;
  if(totliq && liq->agefin_totliq > 0 && X.SalNet(X.date(liq->ageliq))>0) {
    resultat= pensionliq_tot_nette_horsMal()/ X.SalNet(X.date(liq->ageliq));
  }
  else if(totliq && liq->agefin_totliq > 0 && X.SalNet(X.date(liq->ageliq))==0) 
  {
    int t=0;
    while(X.SalNet(X.date(floor(liq->ageliq)-t))==0&&t<=10)//Tom : on se limite aux cas où la période d'inactivité pré-retraite ne dure pas plus de 10 ans
    {
      t++;
    }
    if(X.SalNet(X.date(floor(liq->ageliq)-t))>0)
    {
      resultat= pensionliq_tot_nette_horsMal()/ (X.SalNet(X.date(floor(liq->ageliq)-t))* (M->SMPT[X.date(liq->ageliq)]/M->SMPT[X.date(floor(liq->ageliq)-t)]));
    }
  }
  return resultat;
}


bool Retraite::a_acquis_droits_derives_fp(int idConj) {
  const Indiv& conjoint = pop[idConj];
  int duree_union = 0;
  int ageliq_conjoint = 999;
  bool possede_droits_a_reversion = false;
  if (conjoint.retr && conjoint.retr->primoliq) {
    ageliq_conjoint = conjoint.retr->primoliq->ageliq;
  }
  for (int a : range(min(X.ageMax, age))) {
    if (X.conjoint[a] != idConj) continue;
    // Mariage au moins deux ans avant l'age de liquidation
    if (a < ageliq_conjoint - 2) {
      possede_droits_a_reversion = true;
      break;
    }
    // Mariage d'au moins 4 ans
    duree_union++;
    if (duree_union >= 4) {
      possede_droits_a_reversion = true;
      break;
    }
  }
  // Au moins un enfant issu de l'union
  for (int anaiss_e : X.anaissEnf) {
    if (anaiss_e == 0) continue;
    if((X.conjoint[X.age(anaiss_e - 1900)] == idConj) && (anaiss_e - 1900 < age)) {
      possede_droits_a_reversion = true;
      break;
    }
  }
  
  bool conjoint_a_ete_fp = false;
  for (int a : range(min(conjoint.ageMax, age))) {
    if (in(conjoint.statuts[a], Statuts_FP)) {
      conjoint_a_ete_fp = true;
      break;
    }
  }
  
  return conjoint_a_ete_fp && possede_droits_a_reversion;
}



double Retraite::tauxRempl_net_horsMal_10_rg() {
  if (!totliq || liq->agefin_totliq <= 0) {
    // Pas encore liquidé totalement
    return 0.0;
  }
  
  tx_cotis_sur_les_retraites(X.date(liq->ageliq));
  double pensionliq_nette_rg = liq->pension_rg * (1 - txCSG);
  
  int t = 0;
  int an_liq = X.date(liq->ageliq);
  double salaire_net = X.SalNet(an_liq);
  while (salaire_net == 0 && t <= 10) {
    t++;
    salaire_net = X.SalNet(an_liq - t);
  }
  if (salaire_net > 0) {
    return pensionliq_nette_rg / (salaire_net * (M->SMPT[an_liq] / M->SMPT[an_liq - t]));
  }
  return 0.0;
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