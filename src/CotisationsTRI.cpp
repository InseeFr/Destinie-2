#include "CotisationsTRI.h"
#include "Statistiques.h"
/*resolution*/

double imput_espVie(Indiv& X, int age, double alea)
{
  int age_deces = age;
  
  for(int a : range(age+1,121)) {
    int t = min(250,X.anaiss%1900+a);
    if(alea >= M->survie[X.sexe](t,a) / M->survie[X.sexe](t,age))  break;
    age_deces = a;
  }
  return age_deces;
} 


double VAN(vector<double>& v, double tx)
{
  double van = 0;
  for(auto i : indices(v)) van += v[i] / std::pow(1+tx,i);
  return van;
}

double calcule_tri(vector<double>& v) // précision 1/2**(22-1) ~ -1e6
{
  double tx[3] = {-1,1,30}; 
  while(tx[2]--) tx[ VAN(v, (tx[0] + tx[1]) / 2) < 0 ] = (tx[0] + tx[1]) / 2;
  return (tx[0]*tx[0]<0.999) ? tx[0] : INFINITY;
}



// [[Rcpp::export]]
void destinieSimTRI2(Environment envSim) {

  
    // importation des paramètres
    auto S = Simulation(envSim);
    auto cotisations = Rdin<Cotisations>("cotisations");
    auto options_tri = Rdin<OptionsTRI>("options_tri");
    // Création du champ
    vector<int> Statuts_hors_champ = {S_IND,S_FPAAE,S_FPAATH,S_FPAE,S_FPATH,S_FPSE,S_FPSTH};
    vector<int> Statuts_champ = {S_NC,S_NONTIT,S_CAD};
    vector<bool> pop_champ(pop.size(),false);
    auto ind = make_vector2(AN_NB,3,0.0);
    for(Indiv& X : pop) {
      bool ind_champ0 = true; // N'a jamais été indépendant ou fonctionnaire
      bool ind_champ1 = false; // A été au moins une fois salarié du privé
      
      for(int age : range(X.ageMax)) {
        if(in(X.statuts[age],Statuts_hors_champ))  ind_champ0 = false;
        if(in(X.statuts[age],Statuts_champ))  ind_champ1 = true;
      }
      if(X.ageMax>60 && ind_champ0 && ind_champ1 && X.anaiss>1948 && X.anaiss < 1990) {
        pop_champ[X.Id] = true; 
        int g(int((X.anaiss%1900+2)/5)*5);
        for(int s : {0,X.sexe}) ind[g][s]++;
      }
    }
    Rcout << "Effectif dans le champ : " << pop_champ.size() << endl; 
    Rcout << "Poids individuels : " << M->poids << endl;
    
    
    // Aléas gen pour décès après 2060
    auto gen = vector<int>(pop.size(),0);
    auto sexe = vector<int>(pop.size(),0);
    for(Indiv& X : pop)  if(!X.est_decede(AN_FIN)) {
      gen[X.Id] = X.anaiss%1900;
      sexe[X.Id] = X.sexe;
    }
    
    auto aleas = lisseAlea(sexe,gen,alea());

    // Prolongement après 2060
    for(auto& X : pop) {
        if(X.Id==0) continue;
        // boucle sur chaque age
        int new_ageMax = X.est_decede(AN_FIN) ? X.ageMax : imput_espVie(X,X.ageMax,aleas[X.Id]);
        for(int age : range(X.ageMax,new_ageMax)) {
          X.statuts[age] = (X.statuts[age] == 0 && age <= new_ageMax) ? S_INA : X.statuts[age];
        }
        X.ageMax = new_ageMax;
        static Rdout df5("espVieIndiv",{"Id","ageDeces","annee_Deces","sexe","indic"});
        df5.push_line(X.Id,new_ageMax,new_ageMax+X.anaiss,X.sexe,X.est_decede(AN_FIN));
    }
    
    // boucle sur les années
    auto flux = make_vector3(AN_NB,3,AGE_MAX,0.0);
    auto contrib= make_vector3(AN_NB,3,AGE_MAX,0.0);
    auto prestat= make_vector3(AN_NB,3,AGE_MAX,0.0);
    auto flux_rg = make_vector3(AN_NB,3,AGE_MAX,0.0);
    auto contrib_rg= make_vector3(AN_NB,3,AGE_MAX,0.0);
    auto prestat_rg= make_vector3(AN_NB,3,AGE_MAX,0.0);
    auto sal = make_vector3(AN_NB,3,AGE_MAX,0.0);
    auto ret = make_vector3(AN_NB,3,AGE_MAX,0.0);
    auto act = make_vector3(AN_NB,3,AGE_MAX,0.0);
    
    
    for(int t=50; t <= 90+120; t++) {
      // Boucle pour les droits directs
      for(Indiv & X: pop)  if(X.est_present(t)) {
          X.retr->revaloDir(t);
          if(X.age(t) >= 50 && !X.retr->totliq)  X.retr->SimDir(X.age(t));
      }
      
      // Boucle pour les droits dérivés
      for(Indiv & X: pop)  if(X.est_present(t)) {
          int age = X.age(t);
          
          Indiv& Y = pop[X.conjoint[age]];
          if(Y.est_present(t-1) && !Y.est_present(t)) {
            X.retr->SimDer(Y,t);
            
          } else  if(t==AN_BASE && X.pseudo_conjoint > 0) {
            Indiv &Y = pop[X.pseudo_conjoint] ; 
            X.retr->SimDer(Y,t);
          }
          X.retr->revaloDer(t);
      }
          
      // Boucle pour le minimum vieillesse
      // et calcul des quantiles de revenu
      vector<quantile> quantRev(3);
        
      for(Indiv& X: pop) if(X.est_present(t) && X.est_persRef(t)) {
          // Test minimum vieillesse
          // sur les 65 ans et plus
          // personnes de référence du ménage
          if(X.age(t) >=65)  X.retr->minvieil(t);
          
          int typAge = X.age(t) < 41 ? 0 : X.age(t)< 65 ? 1 : 2;
          double revenuMen = X.retr->revenu();
          int age = X.age(t);
          if(X.conjoint[age]>0 && pop[X.conjoint[age]].est_present(t)) {
            revenuMen += pop[X.conjoint[age]].retr->revenu();
          }
          revenuMen = revenuMen  / X.retr->nbUCCouple();
          quantRev[typAge].push(revenuMen);
      }
      
      vector2<double> TxConso = {
        { 126.8  , 115.2   , 112.4 },
        { 93.5   , 85.8    , 97.8  },
        { 91.8   , 84.4    , 90.5  },
        { 74.2   , 71.5    , 79.9  },
        { 59.3   , 61.5    , 68.3  }};
      
      // Boucle calcul des des contributions et prestations
      for(Indiv& X: pop) if(X.est_present(t)) {
          int age = X.age(t);
          int typAge = age < 41 ? 0 : age < 65 ? 1 : 2;

          double revenuMen = X.retr->revenu();
          if(X.conjoint[age]>0 && pop[X.conjoint[age]].est_present(t)) {
            revenuMen += pop[X.conjoint[age]].retr->revenu();
          }
          revenuMen = revenuMen  / X.retr->nbUCCouple();
          
          int typeConsoMen = 
                revenuMen < quantRev[typAge].resultat(0.20) ? 0 :
                revenuMen < quantRev[typAge].resultat(0.40) ? 1 :
                revenuMen < quantRev[typAge].resultat(0.60) ? 2 :
                revenuMen < quantRev[typAge].resultat(0.80) ? 3 : 4;
          
          double contribRetrConso = TxConso[typeConsoMen][typAge]/100 * cotisations.taux_part_de_conso[t] * X.retr->revenu() ;
          double contribRetrRevAct = 0;
          double contribRetrRevAct_rg = 0;
          double contrib_retr_actif = 0;
          double contrib_cot_retr = 0;
          double contrib_cot_agff = 0;
          double contrib_cot_alleg = 0;
          double contrib_csg = 0;
          double contrib_reven = 0;
          double contrib_trans_Unedic = 0;
          
          if(in(X.statuts[age], Statuts_occ) && X.retr->partavtliq(t) > 0) {
              CotisationsTRI contrib(X,age,*M,cotisations,options_tri);
              contribRetrRevAct =  contrib.cot_tot + (options_tri.NoCotisationsIndirectes ? 0 : contrib.contrib_retr_actif);
              contribRetrRevAct_rg = contrib.cot_rg + contrib.contrib_retr_actif  + contrib.csg_sal_fsv;
              contrib_retr_actif = contrib.contrib_retr_actif  + contrib.csg_sal_fsv;
              contrib_cot_retr = contrib.cot_tot;
              contrib_cot_alleg = contrib.cot_patr_Retr_alleg;
              contrib_trans_Unedic = contrib.transferts_Unedic_Agirc_Arrco;
              contrib_csg = contrib.csg_sal_fsv;
              contrib_cot_agff = contrib.cot_patr_AGFF + contrib.cot_patr_ASF + contrib.cot_sal_Retr_AGFF + contrib.cot_sal_Retr_ASF;
              contrib_reven = contrib.contrib_retr_actif - contrib_csg - contrib_cot_agff;
          }
          double prestaRetr = 0;
          double prestaRetr_rg = 0;
          double contribRetrRetr = 0;
          if ( X.retr->pension_tot > 0 ) {
              prestaRetr += X.retr->pension_tot;
              prestaRetr_rg += X.retr->pension_rg;
              contribRetrRetr += X.retr->retraite_tot * cotisations.taux_csg_retr_FSV[t];
              contrib_csg += X.retr->retraite_tot * cotisations.taux_csg_retr_FSV[t];
          }
          
          static Rdout df9("presta", {"Id", "age", "annee", "sexe", "prestaRetr", "contribRetrRetr", "contribRetrRevAct_rg", "contribRetrConso","contrib_retr_actif",
            "contrib_cot_retr","contrib_cot_alleg","contrib_csg","contrib_reven","contrib_cot_agff","contrib_trans_Unedic"});
          df9.push_line(X.Id, age, t, X.sexe, prestaRetr_rg, contribRetrRetr, contribRetrRevAct_rg, contribRetrConso, contrib_retr_actif,
            contrib_cot_retr,contrib_cot_alleg,contrib_csg,contrib_reven,contrib_cot_agff,contrib_trans_Unedic);
          
          
          if(X.anaiss > 1948 && X.anaiss < 1990 && pop_champ[X.Id]) { 
            for(int s : {0, X.sexe}) {
              int g(int((X.anaiss%1900+2)/5)*5), a(age);
              if(options_tri.NoCotisationsIndirectes) {
                  flux    [g][s][a] -= (contribRetrRevAct) / M->Prix[t];
              } else {
                  flux    [g][s][a] -= (contribRetrConso + contribRetrRetr + contribRetrRevAct) / M->Prix[t];
              }
              flux    [g][s][a] += (prestaRetr) /  M->Prix[t];
              
              flux_rg    [g][s][a] -= (contribRetrConso + contribRetrRetr + contribRetrRevAct_rg) / M->Prix[t];
              flux_rg    [g][s][a] += (prestaRetr_rg) /  M->Prix[t];
              
              contrib [g][s][a] += (contribRetrConso + contribRetrRetr + contribRetrRevAct) / M->SMPT[t];
              contrib_rg [g][s][a] += (contribRetrConso + contribRetrRevAct_rg + contribRetrRetr) / M->SMPT[t];
              prestat [g][s][a] += prestaRetr / M->SMPT[t];
              prestat_rg [g][s][a] += prestaRetr_rg / M->SMPT[t];
              sal     [g][s][a] += X.salaires[age] * X.retr->partavtliq(t) / M->SMPT[t];
              act     [g][s][a] += (X.salaires[age] >0 ) * X.retr->partavtliq(t);
              ret     [g][s][a] += (X.retr->pension_tot > 0) * (1-X.retr->partavtliq(t));
            }
          }
      }
    }
    
    
    static Rdout df0("TXRI",{"gen","sexe","TXRI","contrib","presta","salaires","re","ac",
                     "TXRI_RG","contrib_rg","presta_rg","nb_indiv"});
    for(unsigned int i : indices(flux))  for(unsigned int j : indices(flux[i])) {
        //tx_ri[i][j] = calcule_tri(flux[i][j]);
  
        double tx_ri(calcule_tri(flux[i][j]));
        double tx_ri_rg(calcule_tri(flux_rg[i][j]));
        double cot(0), pre(0), sala(0), re(0), ac(0), cot_rg(0), pre_rg(0);
        for(auto& c : contrib[i][j]) cot += c;
        for(auto& c : contrib_rg[i][j]) cot_rg += c;
        for(auto& p : prestat[i][j]) pre += p;
        for(auto& p : prestat_rg[i][j]) pre_rg += p;
        for(auto& s : sal[i][j]) sala += s;
        for(auto& r : ret[i][j]) re += r;
        for(auto& a : act[i][j]) ac += a;
        if(i > 48 && i < 190 && i%5==0)  
          df0.push_line(i+1900, j, tx_ri, cot, pre, 
           sala, re, ac, tx_ri_rg, cot_rg, pre_rg, ind[i][j]);
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