// ligne suivante indispensable. Grâce à elle : 
// - Rcpp::compilerAttributes() génère une ligne en plus dans RcppExports.R
// (#' @useDynLib destinie)
// - ensuite devtools::document() ajoute la ligne useDynLib(destinie) dans 
// le NAMESPACE du package
//' @useDynLib destinie


#include "Simulation.h"
#include "Indiv.h"
#include "DroitsRetr.h"
#include "Reversion.h"
#include "Retraite.h"
#include "Cotisations.h"
#include "range.h"
#include "Rcpp.h"
using namespace util::lang;

ptr<Options> options = nullptr;
ptr<Macro> M = nullptr;
ptr<CiblesDemo> ciblesDemo = nullptr;
ptr<Mortalite_diff>  mortalite_diff    = nullptr;
ptr<Mortadiff_dip_F>   mortadiff_dip_F   = nullptr;
ptr<Mortadiff_dip_H>   mortadiff_dip_H   = nullptr;
ptr<FinEtudeMoy>      finEtudeMoy      = nullptr;   
int AN_FIN = 160;       ///< Année de fin de la simulation
int AN_NB = 161;

vector<Indiv> pop; ///< tableau de l'ensemble des individus (cf. classe Indiv)
vector<EqSante> eq_sante ; ///< coefficient des équations d'imputation des états de sante
double Simulation::heure_debut = 0;

Simulation::Simulation(Environment env) : context(env)
{
  envSim = env;
  heure_debut = clock();
  options = make_shared<Options>(Rdin<Options>("options"));
 
  AN_FIN = options->AN_MAX%1900;
  AN_NB = options->AN_MAX%1900 + 1 ;
  const double POP_BASE=(options->FM) ? 62765235 : 64612939;
  M = make_shared<Macro>(Rdin<Macro>("macro"));
  ciblesDemo = make_shared<CiblesDemo>(Rdin<CiblesDemo>("CiblesDemo"));

  mortalite_diff   = make_shared<Mortalite_diff>(Rdin<Mortalite_diff>("mortalite_diff"));
  mortadiff_dip_F  = make_shared<Mortadiff_dip_F>(Rdin<Mortadiff_dip_F>("mortadiff_dip_F"));
  mortadiff_dip_H  = make_shared<Mortadiff_dip_H>(Rdin<Mortadiff_dip_H>("mortadiff_dip_H"));
  finEtudeMoy      = make_shared<FinEtudeMoy>(Rdin<FinEtudeMoy>("FinEtudeMoy") ) ; 
  seed = options->seedAlea;
  if(options->umixt) options->comp = comp_umixt;
  M->survie.push_back(NumericMatrix() );
  M->survie.push_back(envSim["Survie_H"]);
  M->survie.push_back(envSim["Survie_F"]);
  
  M->espvie.push_back(NumericMatrix() );
  M->espvie.push_back(envSim["espvie_H"]);
  M->espvie.push_back(envSim["espvie_F"]);
   
  M->q_mort.push_back(NumericMatrix() );
  M->q_mort.push_back(envSim["Qmort_H"]);
  M->q_mort.push_back(envSim["Qmort_F"]);
  
  M->daterevalobase = NumericVector(M->Prix.size());
  M->daterevalocomp = NumericVector(M->Prix.size());
  
  for(int t = 0; t < int(M->Prix.size()); t++) {
    M->daterevalobase[t] = (t < 109) ? 0 : (t < 113) ? 0.25 : (t < 119) ? 0.75 : 0;
    M->daterevalocomp[t] = (t<99) ? 0 : (t<116) ? 3/12 : 10/12;
  }
  
  M->RevaloCumFP = NumericVector(M->Prix.size());
  M->RevaloCumRG = NumericVector(M->Prix.size());
  
  M->RevaloCumFP[0] = 1;
  M->RevaloCumRG[0] = 1;
  for(auto t : indices(M->Prix)) {
    M->RevaloCumFP[t] = M->RevaloCumFP[t-1] * (M->RevaloFP[t] > 0 ? M->RevaloFP[t] : 1);
    M->RevaloCumRG[t] = M->RevaloCumRG[t-1] * (M->RevaloRG[t] > 0 ? M->RevaloRG[t] : 1);
  }
  // Import échantillon et créer le vector pop
  auto ech = Rdin<Ech>("ech");
  auto fam = Rdin<Fam>("fam");
  auto emp = Rdin<Emp>("emp");
  NumericVector ageExo(ech.Id.size(),0.0);
  pop.clear();
  pop.reserve(200000);
  pop.emplace_back(); // ajoute l'individu fictif d'identifant 0
  int j=0, k=0;
  for(int i : range(ech.Id.size())) {
      pop.emplace_back(ech,emp,fam,i,j,k);
  }
  for(auto& X : pop) {
    X.retr = make_shared<Retraite>(X,50);
  }

  // Calcul des poids individuels
  double pop_base = 0;
  for(auto& X : pop) pop_base += (X.est_present(AN_BASE) && X.statuts[X.age(AN_BASE)]);
  M->poids = POP_BASE / pop_base;
  // importation des équations de sante
  eq_sante =  Rdin<EqSante>("EqSante","NomVar",levels(INCID_0_F,INCID_0_H,INCID_1_F,INCID_1_H,MORT_F,MORT_H,PREVAL_F,PREVAL_H));
  
  }


Simulation::~Simulation() {
  pointage("fin de simulation");
  // Détruit tous les objets
  eq_sante.clear();
  pop.clear();
  M.reset();
  ciblesDemo.reset();
  mortalite_diff.reset();
  mortadiff_dip_F.reset();
  mortadiff_dip_H.reset();
  finEtudeMoy.reset(); 
  options.reset();
}


void ecriture_retraites(int t) {
  static Rdout retraites("retraites", {
    "pension", "pension_rg", "pension_fp", "pension_in", "pension_ar", "pension_ag", "pension_ag_ar", /*"coeffTemp", "ageAnnulCoeffTemp",*/
    "rev", "rev_rg", "rev_fp", "rev_in", "rev_ar", "rev_ag",
    "min_vieil","Id","age", "annee","retraite_nette"
    });

  
  for(auto& X : pop) {
    if(X.est_present(t) && (X.retr->pension_tot > 0 || X.retr->rev_tot >0 || X.retr->min_vieil>0)) {
      Retraite& r = *(X.retr);
      retraites.push_line(
        r.pension_tot, r.pension_rg, r.pension_fp, r.pension_in, r.pension_ar, r.pension_ag, r.pension_ag_ar,
        r.rev_tot, r.rev_rg, r.rev_fp, r.rev_in, r.rev_ar, r.rev_ag,
        r.min_vieil, X.Id, X.age(t), X.anaiss + X.age(t),r.retraite_net);
    }
  }
}


void ecriture_droitsRetr(Indiv& X, DroitsRetr& r) {
  static Rdout liquidations("liquidations", {
   "Id","annee","sexe","anaiss","findet",
   "duree_cho", "duree_PR", "duree_inv", "duree_snat", 
   "duree_avpf", "duree_emprg", "duree_fp", "duree_fpa", 
   "duree_fps", "duree_ag", "duree_ag_ar", "duree_emp", "duree_in", 
   "duree_rg", "duree_tot", "dureecotmin_tot", 
   "dureecotdra_tot", "duree_rg_maj", "duree_fp_maj", 
   "duree_in_maj", "duree_tot_maj", "durdecote_fp", 
   "dursurcote_fp", "durdecote_rg", "dursurcote_rg", 
   "tauxliq_fp", "tauxliq_rg", "tauxliq_ar", 
   "majo_min_rg", "majo_min_in", "majo_min_fp", 
   "majo_3enf_rg", "majo_3enf_ar", "majo_3enf_ag", 
   "majo_3enf_in", "majo_3enf_fp", "min_cont", 
   "min_cont_in", "min_garanti", "sr_fp", "sam_rg", 
   "sam_in", "sam_rgin", "sam_uni", "points_arrco", 
   "points_agirc", "points_agirc_arrco", "coeffTemp", "ageAnnulCoeffTemp", "ntp_FP", "VFU_rg", "VFU_ar", 
   "VFU_ag", "pliq", "pension_fp", "pension_rg", 
   "pension_ar", "pension_ag", "pension_ag_ar", "pension_in", "pension", 
   "taux_prorat_rg", "taux_prorat_in", "taux_prorat_fp", 
   "tp", "agetest", "ageliq", "primoliq", "liq", 
   "agefin_totliq", "agefin_primoliq", "ageprimoliq",
   "indic_mc", "indic_mg", "indic_mc_in", "dar", "t","type_liq", 
   "ageFinEmp", "ageFinAct", "partavtprimo", "partavtliq"/*,"derSalNet"*/
  }); /* ,"type_liq",type_liq_labels */
  
  liquidations.push_line( 
     X.Id,r.t+1900,X.sexe,X.anaiss,X.findet,
     r.duree_cho, r.duree_PR, r.duree_inv, r.duree_snat, 
     r.duree_avpf, r.duree_emprg, r.duree_fp, r.duree_fpa, 
     r.duree_fps, r.duree_ag, r.duree_ag_ar, r.duree_emp, r.duree_in, 
     r.duree_rg, r.duree_tot, r.dureecotmin_tot, 
     r.dureecotdra_tot, r.duree_rg_maj, r.duree_fp_maj, 
     r.duree_in_maj, r.duree_tot_maj, r.durdecote_fp, 
     r.dursurcote_fp, r.durdecote_rg, r.dursurcote_rg, 
     r.tauxliq_fp, r.tauxliq_rg, r.tauxliq_ar, 
     r.majo_min_rg, r.majo_min_in, r.majo_min_fp, 
     r.majo_3enf_rg, r.majo_3enf_ar, r.majo_3enf_ag, 
     r.majo_3enf_in, r.majo_3enf_fp, r.min_cont, 
     r.min_cont_in, r.min_garanti, r.sr_fp, r.sam_rg, 
     r.sam_in, r.sam_rgin, r.sam_uni, r.points_arrco, 
     r.points_agirc, r.points_agirc_arrco, r.coeffTemp, r.ageAnnulCoeffTemp, r.ntp_FP, r.VFU_rg, r.VFU_ar, 
     r.VFU_ag, r.pliq, r.pension_fp, r.pension_rg, 
     r.pension_ar, r.pension_ag, r.pension_ag_ar, r.pension_in, r.pension, 
     r.taux_prorat_rg, r.taux_prorat_in, r.taux_prorat_fp, 
     r.tp, r.agetest, r.ageliq, r.primoliq, r.liq, 
     r.agefin_totliq, r.agefin_primoliq, r.ageprimoliq,
     r.indic_mc, r.indic_mg, r.indic_mc_in, r.dar, r.t,r.type_liq, 
     r.ageFinEmp(), r.ageFinAct(),
     r.partavtprimo(r.ageprimoliq), r.partavtliq(r.ageliq)
     /*, r.derSalNet()*/);   
}

void ecriture_liquidations() {
  for(auto& X : pop) {
    if(X.retr->liq) {
      DroitsRetr& r = *(X.retr->liq);
      ecriture_droitsRetr(X,r);
    }
  }
}


void ecriture_aide() {

  Rcpp::Rcout<<"ecriture_aide"<<endl;
  static Rdout aide("aide", {
    "yolo"
    });

  for(auto& X : pop) {
    aide.push_line(0.0);
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