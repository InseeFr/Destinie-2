#include "Simulation.h"
#include "Transitions.h"
#include "Retraite.h"
using NV = Rcpp::NumericVector;

struct CiblesTrans2 {NumericVector  _(H50),   _(F50);};

//Fonctions de répartition des statuts pour l'année d'arrivée des migrants
const vector<double> mig_occ1_hom={INA,	0.0489,	TIT, 0.1734,	PRI, 0.7337,	IND, 0.7886, CHO,	0.9502, RET, 1};
const vector<double> mig_occ2_hom={INA,	0.0687, TIT, 0.2148,	PRI, 0.8202,	IND, 0.8364, CHO,	0.9888, RET, 1};
const vector<double> mig_occ3_hom={INA,	0.0277, TIT, 0.2464,	PRI, 0.8620,	IND, 0.8916, CHO,	0.9939, RET, 1};
const vector<double> mig_occ1_fem={INA,	0.3609, TIT, 0.4246,	PRI, 0.7446,	IND, 0.7633, CHO,	0.9741, RET, 1};
const vector<double> mig_occ2_fem={INA,	0.1916, TIT, 0.3749,	PRI, 0.7779,	IND, 0.7849, CHO,	0.9867, RET, 1};
const vector<double> mig_occ3_fem={INA,	0.1559, TIT, 0.3992,	PRI, 0.8383,	IND, 0.8626, CHO,	0.9980, RET, 1};


//' Fonction destinieTransMdt
//' @description Simule les transitions sur le marché du travail
//' entre statuts d'activité
//' @return Retourne le meme environnement,
//' mais avec des status d'activité du début de la vie active
//' jusqu'au décès.
//' @param envSim Environment 
//' @export
// [[Rcpp::export]]
void destinieTransMdt(Environment envSim) {
  alea_reset(123456);
  auto S = Simulation(envSim);
  Transitions trans(envSim);

  for(Indiv& X : pop)
    X.typeFP = NONFP;
  
  for(Indiv& X : pop) for(int age : range(0,X.ageMax+1)) {
    int annee =X.anaiss + age;
    if(X.statuts[age]==2) X.statuts[age] = 1;
	// Rmq : dans ce qui suit, la condition X.statuts[age] != 0 vaut true ssi X n'est pas migrant. Le cas émigrant est exclus puisque age <= ageMax
	// Rmq : lors du lancement de destinieDemographie(), les statuts des individus sont prolongés jusqu'au décès. La ligne ci-dessous réinitialise les status après findet sauf pour les migrants.
    if (X.statuts[age] != 0 && annee > 2009 && age > X.findet) X.statuts[age] = 999;
	// cumul emploi etude.
	bool cumEmpEtude = alea() < 0.60 * partTx(age,16,30) * partTx(X.anaiss,1950,1990);// Rmq : vaut 1 ssi alea<0.6, age>=16 et anaiss>=1950
	if (X.statuts[age] != 0 && annee > 2009 && age == X.findet) X.statuts[age] =  cumEmpEtude ? 999 : S_SCO ; //cumul emploi etudes avec la meme formule que celle du redressement
	if (X.statuts[age] != 0 && annee > 2009 && age < X.findet) X.statuts[age] =  S_SCO ;
    if (X.statuts[age]==311 || X.statuts[age]==321) X.typeFP = FPE;
    if(X.statuts[age]==312 || X.statuts[age]==322) X.typeFP = FPTH;
  }
  
  if(!options->transNonCalees) 
    for(int t : range(110,AN_NB))
      trans.make_transitions(t,0,999);  
  else
    for(int t : range(110,AN_NB))
      trans.make_transitions_noncalees(t,0,999);
      
  // Réécrit la table emp
  static Rdout df_emp("emp", {"Id", "age", "statut", "salaire"});
  for(Indiv& X : pop) 
    for(auto age : range(X.ageMax)) 
      if (X.Id != 0) {
        X.salaires[age] = (in(X.statuts[age],Statuts_occ)) ? 1 : 0;
        df_emp.push_line(X.Id, age, X.statuts[age], X.salaires[age]);
      }
  
  // Réécrit la table ech (pour mise à jour de typeFP)
  static auto df_ech = Rdout("ech",{"Id","sexe","anaiss","findet","ageMax","mere","pere","neFrance","emigrant","moisnaiss","taux_prim","typeFP","k","enf1","enf2","enf3","enf4","enf5","enf6","pseudo_conjoint","peudip","tresdip","dipl"});
  for(Indiv& X : pop) {
    if(X.Id!=0)
    df_ech.push_line(X.Id,X.sexe,X.anaiss,X.findet,X.ageMax,X.mere,X.pere,X.neFrance,X.emigrant,X.moisnaiss,X.taux_prim,X.typeFP,X.k,X.enf[0],X.enf[1],X.enf[2],X.enf[3],X.enf[4],X.enf[5],X.pseudo_conjoint,X.peudip,X.tresdip,X.dipl);
  }
  
}


double multinomiale(double alea,const vector<double>& vect)
{
  int i = 1;
  while(vect[i] < alea)  i+=2;
  return vect[i-1];
}



int recode_statut(int statut) {
  if(statut/100==3) return TIT;
  if(statut==S_IND || statut==13) return IND; // SCORIE pour coherence avec prg PERL
  if(statut/10==5 || statut==5) return CHO;
  if(statut==S_RET) return RET;
  if(statut==S_NONTIT) return CON;
  if(statut==S_INVALFP) return MAL;
  if(statut==S_INVALIND) return MAL;
  if(statut==S_INVALRG) return MAL;
  if(statut==S_SCO) return INA;
  if(statut==S_INA ) return INA;
  if(statut/100==7 || statut==7) return PRE;
  if(statut==S_NC) return PRI;
  if(statut==S_AVPF) return INA;
  if(statut==S_CAD) return PRI;
  r_assert(0);
  return 0;
}

int duree_emp(Indiv& X, int age) {
  int dur_emp = 0;
  for(int a : range(0,age+1)) // pourquoi age +1 ?
    if(in(X.statuts[a],Statuts_occ))
      dur_emp++;
    return dur_emp;
}

int duree_statut(Indiv& X, int age) {
  int dur_sta = 1; // TODO: 0 ?
  int statut_ref = X.statuts[age];
  int a = age - 1;
  while(a > 0) {
    if(X.statuts[a] != statut_ref) return dur_sta;
    dur_sta++;
    a--;
  }
  r_assert(1);
  return dur_sta;
}


void Transitions::make_transitions_noncalees(int t, int age_min, int age_max) { // Rmq : dans la pratique, age_min=0, age_max=999 : la condition sur l'âge est donc tjs vérifiée
  probas = make_vector2(8,pop.size(),0.0);
  probas_fpe = vector<double>(pop.size(),0.0);
  destination = make_vector2(8,pop.size(),0);
  champ = vector<int>(pop.size(),0.0);
  
  // Calcul de toutes les probas de transition
  for(auto& X : pop) 
    if(X.est_present(t) && X.age(t) >= age_min && X.age(t) <= age_max) 
      ProbTrans(X,t); 
  
  // Tirage aléatoire simple du reste de l'arbre des transitions
  for(int k : {TRANS1, TRANS2, TRANS3, TRANS4, TRANS5, TRANS6, TRANS7}) {
    auto s = tirage(champ, probas[k], TIRAGE_SIMPLE, 0.0,to_string("trans ", k, " ", t+1900));
    for(int i : s) applique_trans(i,t,destination[k][i]);  
  }
    
}


void Transitions::make_transitions(int t, int age_min, int age_max) {
  probas = make_vector2(8,pop.size(),0.0);
  probas_fpe = vector<double>(pop.size(),0.0);
  destination = make_vector2(8,pop.size(),0);
  champ = vector<int>(pop.size(),0);
  
  // Calcul de toutes les probas de transition
  for(auto& X : pop) 
    if(X.est_present(t) && X.age(t) >= age_min && X.age(t) <= age_max) 
      ProbTrans(X,t); 
  
  // Calcul des champs pour calage
 enum{H1519,H2024,H2529,H3034,H3539,H4044,H4549,H5054,F1519,F2024,F2529,F3034,F3539,F4044,F4549,F5054,SENIORS,JEUNES};
  auto cage = vector<int>(pop.size(),0);
  auto cage_FP = vector<int>(pop.size(),0);
  for(auto& X : pop) 
    if(X.est_present(t) && champ[X.Id]) {
      int age = X.age(t);
      cage[X.Id] = (X.sexe == HOMME) ? 
        ( age < 20 ?  H1519 : age<25 ? H2024 : age<30 ? H2529 : age < 35 ?  H3034 : age<40 ? H3539 : age < 45 ?  H4044 : age<50 ? H4549 : age < 55 ?  H5054 : SENIORS):
        ( age < 20 ?  F1519 : age<25 ? F2024 : age<30 ? F2529 : age < 35 ?  F3034 : age<40 ? F3539 : age < 45 ?  F4044 : age<50 ? F4549 : age < 55 ?  F5054 : SENIORS);
	  cage_FP[X.Id]=age < 55 ?  JEUNES : SENIORS;	
    }
    
  
  // déroule l'arbre des transitions
  // par tirage systématique
  // ou par tirage alétoire simple
  
  // Tirages systématiques Inativité
  vector<double> taux_cible_ina = {
      -1,-1,cibles.INA_H2529[t],cibles.INA_H3034[t], cibles.INA_H3539[t],cibles.INA_H4044[t],cibles.INA_H4549[t],cibles.INA_H5054[t],
      -1,-1,cibles.INA_F2529[t],cibles.INA_F3034[t], cibles.INA_F3539[t],cibles.INA_F4044[t],cibles.INA_F4549[t],cibles.INA_F5054[t],-1};
  
  for(int k : {H1519,H2024,H2529,H3034,H3539,H4044,H4549,H5054,F1519,F2024,F2529,F3034,F3539,F4044,F4549,F5054,SENIORS}) {
      double cible = taux_cible_ina[k];
      int type_cale = (cible == -1) ? TIRAGE_SIMPLE : TIRAGE_TAUX ;
      auto s_inact = tirage(champ && (cage==k), probas[TRANS1], type_cale, cible, to_string("INACTIF ",t+1900," ",k));
      for(auto i : s_inact) applique_trans(i,t,destination[TRANS1][i]);
  }
  
  // Tirages systématiques chômage
  vector<double> taux_cible_cho = {
      cibles.CHO_H1519[t],cibles.CHO_H2024[t],cibles.CHO_H2529[t],cibles.CHO_H3034[t],cibles.CHO_H3539[t],cibles.CHO_H4044[t], cibles.CHO_H4549[t], cibles.CHO_H5054[t],
      cibles.CHO_F1519[t],cibles.CHO_F2024[t],cibles.CHO_F2529[t],cibles.CHO_F3034[t],cibles.CHO_F3539[t],cibles.CHO_F4044[t], cibles.CHO_F4549[t], cibles.CHO_F5054[t], -1};
  for(int k : {H1519,H2024,H2529,H3034,H3539,H4044,H4549,H5054,F1519,F2024,F2529,F3034,F3539,F4044,F4549,F5054,SENIORS}) {
    double cible = taux_cible_cho[k];
    int type_cale = (cible == -1) ? TIRAGE_SIMPLE : TIRAGE_TAUX ;
    auto s = tirage(champ && (cage==k), probas[TRANS2], type_cale, cible, to_string("Chomage ",t+1900," ",k));
    for(auto i : s) applique_trans(i,t,destination[TRANS2][i]);  
  }

  // Tirage systématique FP
  auto champ_fp = vector<int>(pop.size(),0);
  auto s_3 = tirage(champ && (cage!=SENIORS), probas[TRANS3], TIRAGE_TAUX, cibles.PART_FP[t],to_string("trans FP ",t+1900));
  for(int i : s_3) champ_fp[i] = 1;
  // Tirage systématique parmi la FP des FPE versus FPTH
  auto s_fpe = tirage(champ_fp, probas_fpe, TIRAGE_TAUX, cibles.PART_FPE[t],to_string("trans FPE ",t+1900));
  for(int i : s_fpe) {pop[i].typeFP = FPE; champ_fp[i] = 0;}
  for(int i : s_3) {
    if(champ_fp[i] && pop[i].typeFP==NONFP) pop[i].typeFP = FPTH; 
    applique_trans(i,t,destination[TRANS3][i]);
    }
  
  auto s_4 = tirage(champ && (cage==SENIORS), probas[TRANS3], TIRAGE_SIMPLE, 0.0, to_string("trans FP ",t+1900));
  for(int i : s_4) {applique_trans(i,t,destination[TRANS3][i]);}

  // Tirage aléatoire simple du reste de l'arbre des transitions
  for(int k : {TRANS4, TRANS5, TRANS6, TRANS7}) {
    auto s = tirage(champ, probas[k], TIRAGE_SIMPLE, 0.0, to_string("trans ", k, " ", t+1900));
    for(int i : s) applique_trans(i,t,destination[k][i]);  
  }
    
}

void Transitions::transMigr(Indiv& X, int age) {
  if(age <= X.findet) X.statuts[age] = S_SCO; 
  else { 
    auto distrib = (X.sexe == HOMME) ? 
      ((X.findet <= 18) ? mig_occ1_hom : (X.findet <= 21) ? mig_occ2_hom : mig_occ3_hom):
      ((X.findet <= 18) ? mig_occ1_fem : (X.findet <= 21) ? mig_occ2_fem : mig_occ3_fem);
    applique_trans(X.Id,X.date(age),multinomiale(alea(), distrib));
  }
}

void Transitions::applique_trans(int id, int t, int st) { // Rmq : st désigne le nouveau statut.
  Indiv& X = pop[id];
  int age = X.age(t);

  // L'individu sort du champ
  champ[X.Id] = 0;
  
  
  if(X.statuts[age-1] == 621 && in(st,{CON,PRI,TIT,IND})) { st = PRI;}
  if(X.statuts[age-1] == 623 && in(st,{CON,PRI,TIT,IND})) { st = TIT;}
  if(X.statuts[age-1] == 624 && in(st,{CON,PRI,TIT,IND})) { st = IND;}
  
  if (st == CON) {X.statuts[age]=11;}
  else if (st == PRI) {X.statuts[age]=1;}
  else if (st == TIT) {
    X.typeFP = (X.typeFP != NONFP) ? X.typeFP : FPTH;
    bool categorie_active = 
      in(X.statuts[age-1],{311,312}) ? true :
      in(X.statuts[age-1],{321,322}) ? false :
      alea() < 0.21; // cible 21% des catégories actives à LT
    
    X.statuts[age] = (categorie_active) ? 
                      ((X.typeFP==FPE) ? 311 : 312):
                      ((X.typeFP==FPE) ? 321 : 322);
  }
  else if (st == CHO) {X.statuts[age]= (X.statuts[age-1]==52) ? 52 : 51;}
  else if (st == IND) {X.statuts[age]=4;}
  else if (st == INA) {X.statuts[age]=(age>X.findet) ? 6 : 63;}
  else if (st == PRE) {X.statuts[age]=7;}
  else if (st == MAL) {
    if(in(X.statuts[age-1],{623,311,312,321,322}))   {X.statuts[age]=623;}
    else if(in(X.statuts[age-1],{4,624}))                 {X.statuts[age]=624;}
    else X.statuts[age]=621;
  }
  else if(st == RET) {X.statuts[age] = 6;}
  else Rcout << X.Id << " " << X.statuts[age-1] << " " << st << " " << age << " " << TIT << " " << X.sexe << " " << probas[TRANS6][X.Id] << endl;
}


/**
 * Modifie champ et probas
 */
 // La méthode ProbTrans(X,t) calcule l'arbre de décision associé à X à la date t ainsi que les probabilités de transition à chaque noeud de l'arbre. L'arbre de décision dépend du statut de X en t-1.
void Transitions::ProbTrans(Indiv& X, int t) {
  int age = X.age(t);
  
  // Individus hors-champ
  if(age <= X.findet && X.statuts[age]== S_SCO) { 
    champ[X.Id]=1; probas[TRANS1][X.Id] = 1; destination[TRANS1][X.Id] = INA;   
    return;
  } // année de l'âge de fin d'étude traité à part
  if(X.statuts[age-1] == 0) { transMigr(X,age); champ[X.Id] = 1; return;} // migrant : année d'arrivée traitée à part. 
  // Rmq  : la condition X.est_present(t) est testée dans la méthode make_transitions(), donc on travaille bien 
  // sur l'âge d'arrivée du migrant.
  if(age > 70) {X.statuts[age]=X.statuts[age-1]; return;} // statuts prolongés après 70 ans
 
  // Prise en compte de l'effet horizon
  int hrzn = 0;
  if(options->effet_hrzn && age >= 55)
    hrzn += affn(t+1900,{2010,0.0,2015,2.0}) + alea(); // arrondi aléatoire
  
  if(options->super_effet_hrzn && age >= 55)
    hrzn += min_max(X.age_exo -60,0.0,7.0) + alea(); // arrondi aléatoire
  
  if(options->effet_hrzn_indiv && age >= 55) {
    double decal_moy = 60 + affn(t+1900,{2010,0.0,2015,2.0,2040,5.0});
    double ageliq = X.retr->liq ? X.retr->liq->agefin_primoliq : decal_moy;
    double ageexo = X.age_exo ? X.age_exo : decal_moy;
    if(t < 114) hrzn += min_max(ageexo-60,0.0,7.0) + alea();
    else hrzn += min_max(ageliq-60,0.0,7.0) + alea();
  }
  
  hrzn = 0; // Rmq : pas d'effet horizon in fine ?
  // Cacul des regresseur
  
  double findet =  X.findet;
  double findetr = min_max(findet-14, 0, 26-14); // Rmq :  "r" pour relatif
  double findet2 = findetr*findetr;
  double gen = min_max((X.anaiss-1935)/35.0,0.0,1.0);

  double adferelatif =  - 5
    + 0.641          * findetr
    + (1-0.641)/12.0 * findet2
    + 3.961          * (1-gen) 
    + 0.0238         * findetr * (1-gen)
    - 0.0296         * findet2 * (1-gen);  
  
  double AG0 = age - hrzn;      // ages pour les modèles de proba
  double AG = age - 1 - hrzn;
  double AG2 = AG*AG/100.0;
  double AG3 = AG*AG*AG/1000.0;
  
  //double ENF1=X.nbEnf[age];
  
  //double SORTANTS_SCO = (AG==findet);
  double SORTANTS_SCO = (X.statuts[age-1] == S_SCO); // Rmq : on considère le statut et non l'âge afin de contourner le problème des migrants, n'est-ce pas ?
  double EXP1A3 = (AG>findet && AG < (findet + 3 + 1));
  double dur_cho = 0;
  double dur_ina = 0;
  double dur_pri = 0;
  
  for(int a : range(age-5,age)) { // TODO: Est-ce bien ça l'esprit ?
    if(X.statuts[a] == S_CHOBIT) dur_cho++;
    else if(X.statuts[a] == S_CHONONBIT) dur_cho++;
    else if(X.statuts[a] == S_INA) dur_ina++;
  }
  for(int a : range(age-10,age))
    if(in(X.statuts[a], Statuts_Priv)) 
      dur_pri++;
    
  double dur_emp = duree_emp(X, age-1);
  double dur_statut = duree_statut(X,age-1);
  int enf1 = X.nb_enf(age, 18) == 1;
  //int enfantm3 = nb_enf(X, age, 3) > 0;
  int enfm1 = X.nb_enf(age, 1);
  int enfm3 = X.nb_enf(age, 3);
  int enftot = X.nb_enf(age, 99);
  
  // prise en compte de l'évolution de la participation des femmes au marché du travail, 
  // fonction croissante et coudée dans le temps, en log
  double probpart = log(1990-(1958+10+13-1));
  
  // variables de choix de modèle et strate de calage
  if(X.statuts[age-1]==999) Rcout << "erreur " << X.Id << " " << age << " " << t << endl;
  
  int periodeReg = X.sexe == HOMME ? 
    ( AG0 < findet +10 ? DEBUTH : AG0 <= 55 ? MILIEUH : FINH):
    ( AG0 < findet +10 ? DEBUTF : AG0 <= 55 ? MILIEUF : FINF);
  
  int statut_avt = recode_statut(X.statuts[age-1]); 
  if(statut_avt == MAL && (periodeReg == DEBUTH || periodeReg == DEBUTF))
    statut_avt = INA; // Jeunes malades mis avec inactifs
  // Force les statuts Retraite et préretraite
  if(statut_avt == RET) {X.statuts[age]=S_RET; return;}
  if(statut_avt == PRE) {X.statuts[age]=X.statuts[age-1]; return;}
  if(AG0 > 54 && X.statuts[age-1]==52) {X.statuts[age]=X.statuts[age-1]; return;}
  //if(X.statuts[age-1] == INA && AG > 60) {X.statuts[age]=X.statuts[age-1]; return;}
  
  champ[X.Id] = 1;
  
  
  // Calcul des probabilités pour chaque transitions
  
  //probas FPE
  double proba_fpe = -0.12 - 1.12*(findet<=19)  +  0.84*(X.sexe==1)  +  3.4*(age-findet>3)+
                     -0.92*(dur_pri>3) - 2.39*(age>30);
  probas_fpe[X.Id] = (X.typeFP == FPTH) ? 0.00001 : 
                          (X.typeFP == FPE) ? 0.99999 : 
                          exp(proba_fpe)/(1+exp(proba_fpe));
  
  // boucle sur chaque noeud de l'arbre  
  for(int st_test : range(7)) {
    EqTrans& eq = eqtd[periodeReg][statut_avt][st_test]; 
    int dest = eq.destination -1;
    destination[st_test][X.Id] = dest;
    
    if(eq.indic != 1) {
      // force la dernière transition
      probas[st_test][X.Id] = 1;
      champ[X.Id] = 1;
      break;
    }
    
    double logit  =
      eq.Intercept +
      adferelatif  * SORTANTS_SCO * eq.findet_rel_a +
      adferelatif * eq.findet_rel +
      adferelatif * EXP1A3 * eq.findet_rel_b +
      AG * eq.age  +
      AG * EXP1A3 * eq.age_b +
      AG * SORTANTS_SCO * eq.age_a +
      AG2 * eq.age2 +
      AG2 * EXP1A3 * eq.age2_b +
      AG2 * SORTANTS_SCO * eq.age2_a +
      AG3 * eq.age3 +
      AG3 * EXP1A3 * eq.age3_b  +
      AG3 * SORTANTS_SCO * eq.age3_a + 
      dur_cho * eq.durchom +
      dur_emp * eq.dur_emp +
      dur_ina * (eq.durINA + eq.durina) +
      dur_statut * eq.dur_statut +
      enfm1 * eq.NbEnf1 +
      enf1 * eq.nbenf98 +
      enf1 * EXP1A3 * eq.nbenf98_b +
      enf1 * SORTANTS_SCO * eq.nbenf98_a +
      enfm3 * eq.NbEnf3 +
      enftot * eq.nbenf +
      EXP1A3 * eq.exp1a3 + 
      probpart * eq.probpart +
      SORTANTS_SCO * eq.sortant +
      (AG==55) * eq.a55 +
      (AG==56) * eq.a56 +
      (AG==57) * eq.a57 +
      (AG>=58) * eq.a58 +
      /*(age==59) * eq.a59 +*/
      0.125 * eq.PreRetrCAATA;
    
    double proba = exp(logit)/(1+exp(logit));
    
    
    if(t==109) {
      static auto df = Rdout("proba",{"proba","constante",
              "logit","Id","t","age0","gen","statut_avt","st_test","dest","periodeReg"
            },
              "statut_avt",levels(CHO,  CON,  INA,  IND,  MAL,  PRI, TIT, XXX, RET, PRE),
              "dest",levels(CHO,  CON,  INA,  IND,  MAL,  PRI, TIT, XXX, RET, PRE), 
              "periodeReg", levels(DEBUTF,DEBUTH,FINF,FINH,MILIEUF,MILIEUH,NBTYPE_TRANS));
      
      df.push_line(proba,eq.Intercept,logit,X.Id,t,12.89,X.anaiss,statut_avt,st_test,dest,periodeReg
           );
    }
    
    probas[st_test][X.Id] = proba;
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