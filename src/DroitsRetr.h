/**
 * \file DroitsRetr.h
 * \brief Calcul des droits à la retraite (cf. DroitsRetr)
 */

#pragma once
#include  "Simulation.h"
#include "Indiv.h"
#include  "Legislation.h"
class Indiv;




class DroitsRetr {
  /**
 * \class DroitsRetr
 * \brief Droits directs par régime.
 * 
 * Cette classe détermine les droits directs à la liquidation pour un individu,
 * une législation et un âge de liquidation donnés. 
 * 
 * L'objet est créé dans la fonction \ref SimDir de la classe \ref Retraite et utilisé, entre autres, par les
 * fonctions \ref TestLiq et \ref TestSecondLiq du fichier \ref OutilsComp.h. Cette classe stocke les éléments
 * constitutifs des droits directs (montant à la liquidation, majorations, durée validée, coefficient de
 * décote/surcote, ...).
 * 
 * Un objet de cette classe est créé pour chaque individu à chaque âge testé. 
 * Un objet (ou deux si la liquidation a lieu en deux étapes) est ensuite conservé en cas de liquidation.
 * 
 * Calcul des pensions de droits directs des régimes de base (régimes en annuités) 
 * ===============================================================================
 * 
 * Les pensions de droit direct des régimes de base sont calculées pour trois régimes : le RG, la SSI et la FP (SRE et CNRACL). La pension à la liquidation est le produit de trois termes : le taux de liquidation , le coefficient de proratisation et le salaire de référence (salaire annuel moyen, SAM, pour le RSI et le RG, dernier salaire pour la FP).
 * 
 * Pension = SAM x tauxliq x taux_prorat
 * 
 * - SAM = Salaire annuel moyen = moyenne des salaires perçus pendant les
 * meilleures années d'activité (10 à 25 meilleures années selon l'année
 * de naissance), dans la limite du PSS, pour les salaires reportés
 * à partir de 2005.
 * - tauxliq = Taux de liquidation = fixé en fonction de la durée d'assurance, tous
 * régimes confondus. Le taux maximal, dit « taux plein », est de 50% (75% pour la fonction publique).
 * - taux_prorat = Coefficient de proratisation. C'est le rapport entre le nombre de trimestres d'assurance (cotisés ou non) et la durée de référence de 150 à 164 trimestres, en fonction de l'année
 * de naissance (durée d’assurance requise à partir de la génération 1948).
 * 
 * 
 * Les durées d’assurance sont calculées par la fonction durees_base, et les durées majorées par durees_majo. Les taux de liquidation sont calculés par la fonction DecoteSurcote. Le salaire de référence est calculé par la fonction SalBase. 
 * 
 * Cette formule de base est en outre modifiée par le minimum contributif (ou garanti pour la FP) et majorée d’une bonification pour les mères de 3 enfants.
 * 
 * Il existe des minima de pension (minimum contributif au RG et au RSI, minimum garanti à la FP). Le montant du minimum est calculé au prorata de la durée validée (et peut donc être très faible) : 
 * le dispositif ne relève donc pas \textit{stricto sensu} d’une logique de revenu minimum. La comparaison pension/minimum de pension est faite 
 * à la liquidation par la fonction AppliqueMin.
 *  
 * 
 * Calcul des pensions de droits directs des régimes complémentaires (régimes en points)
 * =====================================================================================
 * 
 *  A la liquidation, la pension est calculée comme le produit suivant:
 *	Taux de liquidation * nombre de points * valeur de service du point
 *
 *	Le taux de liquidation dépend de 
 *   - l'âge à la liquidation 
 *   - de la durée validée dans les régimes de base 
 *	Un taux d’abattement, équivalent à la décote à l’AGIRC ARRCO, est ensuite appliqué.
 *
 *   Le nombre de points dépend de la carrière. 
 *   Au cours de celle-ci, l'acquisition de points se fait via les cotisations selon la formule:
 *   Nombre de points = (Revenu salarial * taux de cotisation) / valeur d’achat du point.
 *   
 *   Un taux d’appel fixe la part de cotisations non génératrices de droits. 
 * 
 * Les droits à la liquidation sont calculés en quatre étapes :
 * =========================================================
 * 
 * 
 * - calcul des durées de base (\ref durees_base)
 * - calcul des durées majorées (\ref durees_majo ). Cette étape détermine l'âge d'ouverture des droits.
 * - calcul de la décote ou de la surcote (\ref DecoteSurcote). Permet le calcul des taux de liquidation.
 * - calcul des pensions à liquidation (\ref Liq et \ref SecondLiq). Ces dernières étapes appellent les méthodes suivantes :
 *    - \ref SalBase      : Calcul du SAM
 *    - \ref Points        : Calcul des points à l'AGIRC et à l'ARRCO
 *    - \ref LiqPublic	  : Calcul de la pension FP
 *    - \ref LiqPrive		  : Calcul de la pension au RG, au RSI, ainsi qu'à l'Arrco et à l'Agirc
 *    - \ref AppliqueMin    : Application des minina de pensions
 *    - \ref AppliqueBonif  : Application de la bonification pour enfants
 * 
 * Exemple d'utilisation :
 * =======================
 * 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Indiv& X = M->pop[12];
 * Leg l = Leg(X, 60, min(X.anaiss+60,options->anLeg)); 
 *
 * DroitsRetr dr = DroitsRetr(X,l,60+3/12.0);
 * if(dr.AgeMin()) {
 *     dr.liq();
 *     Rcout << "L'individu 12 peut liquider avec une pension : " 
 *     Rcout << dr.pension;
 * }
 * else {
 *     Rcout << "L'individu 12 n'a pas atteint l'âge min.pour liquider";
 * }
 * 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 */

  public:
    // Caractéristiques de liquidation
  const Indiv& X;             ///< Référence à l'individu corespondant aux droits calculés
  Leg& l;                     ///< Référence à l'objet Leg utilisé pour le calcul des droits
  double agetest;             ///< Âge fin de mois testé
  
  double agefin_totliq = 0,       ///< Âge fin de mois de liquidation totale  
         agefin_primoliq = 0;     ///< Âge fin de mois de primo-liquidation 
         
  int    ageliq = 0,              ///< Âge à la liquidation de la totalité des droits (âge entier au 31/12 de l'année) 
         ageprimoliq = 0;         ///< Âge à la primo-liquidation des droits (âge entier au 31/12 de l'année)
  
  double  duree_cho = 0,          ///< Durée cumulée au chômage (au RG)   
          duree_PR = 0,           ///< Durée cumulée en pré-retraite 
          duree_inv = 0,          ///< Durée cumulée en invalidité 
          duree_snat = 0,         ///< Durée cumulée service national 
          duree_avpf = 0,         ///< Durée cumulée passée en AVPF 
          duree_emprg = 0,        ///< Durée cumulée avec cotisation au RG 
          duree_fp = 0,           ///< Durée cumulée à la fonction publique 
          duree_fpa = 0,          ///< Durée cumulée à la fonction publique (catégorie active) 
          duree_fps = 0,          ///< Durée cumulée à la fonction publique (catégorie sédentaire) 
          duree_ag = 0,           ///< Durée cotisée avec affiliation à l'Agirc 
          duree_emp = 0,          ///< Durée totale en emploi 
          duree_in = 0,           ///< Durée cotisée au régime des indépendants 
          duree_rg = 0,           ///< Durée cotisée au régime général
		      duree_ag_ar =0,         ///< Durée d'affiliation au régime complémentaire des salariés du privé unifié
		      duree_ar =0,            ///< Durée d'affiliation à l'Arrco  
          duree_tot = 0;          ///< Durée totale cotisée 
  
  double dureecotmin_tot = 0,     ///< Durée totale cotisée, servant pour le calcul des minimum contributif et garanti 
         dureecotdra_tot = 0;     ///< Durée totale cotisée, servant pour le calcul de l'éligibilité à la retraite anticipée pour carrière longue 
  
  double duree_rg_maj = 0,        ///< Durée RG incluant AVPF et MDA (selon options)  
         duree_fp_maj = 0,        ///< Durée FP incluant MDA éventuelle (selon options) 
         duree_in_maj = 0,        ///< Durée indépendant incluant MDA éventuelle (selon options) 
         duree_tot_maj  = 0;      ///< Durée totale incluant AVPF et MDA (selon options) 
  
  double durdecote_fp = 0,        ///< Durée (en année) déterminant la surcote à la fp (durée = 0 pour un départ sans surcote)  
         dursurcote_fp = 0,       ///< Durée (en année) déterminant la décote à la fp (durée = 0 pour un départ sans décote) 
         durdecote_rg = 0,        ///< Durée (en année) déterminant la décote au rg et pour les indépendants (durée = 0 pour un départ sans décote) 
         dursurcote_rg = 0;       ///< Durée (en année) déterminant la surcote au rg et pour les indépendants (durée = 0 pour un départ sans surcote) 
  
  double tauxliq_fp = 0,          ///< taux de liquidation au RG (=1 en cas de départ sans décote ni surcote) et au régime des indépendants 
         tauxliq_rg = 0,          ///< taux de liquidation à la FP (=1 en cas de départ sans décote ni surcote) 
         tauxliq_ar = 0;          ///< taux de liquidation à l'ARRCO età l'AGIRC (=1 en cas de départ sans décote ni surcote) 
  
  double  majo_min_rg = 0,        ///< montant de la majoration de pension au RG liée à l'application du minimum contributif (montant différentiel inclus dans $pension_rg) 
          majo_min_in = 0,        ///< montant de la majoration de pension chez les indépendants liée à l'application du minimum contributif (montant différentiel inclus dans $pension_rg) 
          majo_min_fp = 0,        ///< montant de la majoration de pension à la FP liée à l'application du minimum garanti (montant différentiel inclus dans $pension_rg) 
          majo_3enf_rg = 0,       ///< montant de la majoration de pension totale (droits directs + réversion) pour 3 enfants au RG 
          majo_3enf_ar = 0,       ///< montant de la majoration de pension totale (droits directs + réversion) pour 3 enfants à l'ARRCO 
          majo_3enf_ag = 0,       ///< montant de la majoration de pension totale (droits directs + réversion) pour 3 enfants à l'AGIRC 
          majo_3enf_in = 0,       ///< montant de la majoration de pension totale (droits directs + réversion) pour 3 enfants chez les indépendants 
          majo_3enf_fp = 0,       ///< montant de la majoration de pension totale (droits directs + réversion) pour 3 enfants à la FP 
		      majo_3enf_ag_ar = 0;	  ///< montant de la majoration de pension totale (droits directs + réversion) pour 3 enfants à l'ARRCO 
  
  double min_cont = 0,            ///< minimum contributif au RG 
         min_cont_in = 0,         ///< minimum contributif au RSI 
         min_garanti = 0;         ///< minimum garanti 

  double  sr_fp = 0,              ///< salaire de référence pour le calcul de la pension FP  
          sam_rg = 0,             ///< SAM servant au calcul de la pension RG 
          sam_in = 0,             ///< SAM pour le calcul de la pension d'indépendant 
          sam_rgin = 0,           ///< SAM calculé sur l'ensemble des revenus d'activité annuels au RG et chez les indépendants 
          sam_uni  = 0 ;          ///< SAM calculé sur l'ensemble des revenus d'activité (tous régimes), selon les règles du RG 
  
  double points_arrco = 0,        ///< cumul points ARRCO 
         points_agirc = 0,        ///< cumul points AGIRC 
         ntp_FP = 0 ,             ///< nombre de points ARRCO "fictifs", calculé sur la base des salaires des fonctionnaires (ne seront pris en compte que si le fonctionnaire n'atteint pas la condition de fidélité)
		 points_agirc_arrco =0,   ///< cumul points régime unifié AGIRC-ARRCO (cf. accord de 2015)
		 maj_points_arrco = 0,	  ///< cumul des points ARRCO dus pour majoration au titre des 3 enfants
		 maj_points_agirc = 0,	  ///< cumul des points AGIRC dus pour majoration au titre des 3 enfants
		 maj_points_agirc_arrco = 0 ; ///<cumul des points régime unifié AGIRC-ARRCO pour la majoration au titre des 3 enfants 
		 
  double VFU_rg = 0,              ///< montant du versement forfaitaire unique au RG  
         VFU_ar = 0,              ///< montant du versement forfaitaire unique à l'Arrco 
         VFU_ag = 0;              ///< montant du versement forfaitaire unique à l'Agirc 
  
  double pliq = 0,                ///< pension totale à la liquidation 
         pension_fp = 0,          ///< pension fonction publique 
         pension_rg = 0,          ///< pension régime général 
         pension_ar = 0,          ///< pension arrco 
         pension_ag = 0,          ///< pension agirc 
		     pension_ag_ar =0 ,		    ///< pension des complémentaires unifiées
         pension_in = 0,          ///< pension d'indépendant 
         pension = 0;             ///< pension directe totale (somme des précédentes) 
         
  
  double taux_prorat_rg = 0,      ///< taux de proratisation au RG 
         taux_prorat_in = 0,      ///< taux de proratisation au RSI 
         taux_prorat_fp = 0;      ///< taux de proratisation FP 

  
  double coeffTemp = 1.0;         ///< coefficient temporaire Agirc Arrco
  double ageAnnulCoeffTemp = 0.0; ///< âge d'annulation du coefficient temporaire Agirc Arrco
  
  
  double tp = 0;                  ///< indicatrice taux plein 

  bool   primoliq = false,            ///< Indicatrice primo-liquidation 
         liq      = false;            ///< Indicatrice liquidation totale 
  bool   indic_mc = false,            ///< Indicatrice minimum contributif au RG 
         indic_mg = false,            ///< Indicatrice minimum garanti 
         indic_mc_in = false;         ///< Indicatrice minimum contributif au RSI 
  bool   dar = false;                 ///< Indicatrice départ anticipé pour carrière longue 
  int type_liq = liq_non;     ///< Motif de liquidation (par défaut, l'individu n'a pas liquidé)
  
  int t = 0;                      ///< année testée 
  int age = 0;                    ///< age au 31/12 testé 
  double datetest = 0;            ///< date testée 

  /** \brief Contruit un nouvel objet DroitsRetr pour un individu, une législation et un âge test donnés */
  DroitsRetr(const Indiv& X, Leg& leg, double f_agetest);
  
  /**  \fn Méthode duree_trim
   *   \brief Calcule la durée en trimestres passée dans un ou plusieurs statuts 
   * 
   *     @param status_cpt : vecteur des statuts comptabilisés 
   *     @return durée en années (multiple de 0.25)
   * 
   *   Complément de la fonction \ref durees_base, en fonction de l'âge de test.
   *   Calcule les durées passées par l'individu X dans un ou plusieurs états, jusqu'à
   *   l'âge test. Les durées pour l'année civile de l'âge en argument sont arrondies 
   *   au nombre de trimestres civils jusqu'au
   *   1er du mois suivant cet âge.
   */

  double duree_trim(const vector<int>& statuts_cpt, int anneLimite= options->AN_MAX);

  
  
  /** 
   *  \fn Méthode durees_base()
   *  \brief Calcul des durées de base (durées cotisations et AVPF) 
   * 
   *  Calcule les durées cotisées dans les différents régimes et la durée AVPF pour
   *  l'individu X. 
   * 
   * Cette méthode calcule :
   * \ref  duree_cho    ,  \ref    duree_PR     ,  \ref    duree_inv    ,  
   * \ref    duree_snat   ,  \ref    duree_avpf   ,  \ref    duree_emprg  ,  
   * \ref    duree_fpa    ,  \ref    duree_fps    ,  \ref    duree_ag     ,    
   * \ref    duree_emp    et  \ref    duree_in.
   * 
   * 
   * 
  */
  void durees_base();
  
  /** \brief Calcul des durées majorées 
   * 
   *  Repart des durées calculées par \ref durees_base, et calcule les durées majorées 
   *  (\ref duree_rg_maj , \ref duree_fp_maj , \ref duree_in_maj , \ref duree_tot_maj)
   *  prenant en compte la Mda selon les options (`option.NoMDA`).
   *  
  */
  void durees_majo();
  
  /** \brief Calcule les durées de décote et surcote et les taux de liquidation
   * 
   * Calcule les durées de décote et surcote et les taux de liquidation
   * (les durées de décote et surcote sont exprimées en année, 
   *   mais pouvent prendre des valeurs arrondies à 0.25 près,
   *   c'est-à-dire au trimestre près) 
   * 
   *  Cette fonction nécessite d'avoir lancé auparavant les fonctions durees_majo et durees_base
   * 
   *  Cette méthode renseigne les variables suivantes :
   *  - \ref dursurcote , \ref durdecote_fp , \ref dursurcote_fp , \ref durdecote_rg
   *  - \ref tauxliq_rg , \ref tauxliq_fp , \ref tauxliq_ar
   * 
   *  Le calcul tient compte de plus :
   *  - de l'annulation de la décote en cas d'invalidité
   *  - de l'annulation de la décote en cas de liquidation au titre de l'inaptitude HORS INVALIDITE (proba etat inapt = f(X.k) )
   *  - du taux de liquidation (coefficient d'abattement) dans les régimes complémentaires ARRCO et AGIRC (suppression de la décote si taux plein au RG)
   */
  void DecoteSurcote();
  
  /** 
   *  \brief Indique si l'individu a atteint l'âge minimal d'ouverture des droits. 
   *  @return retourne true si l'individu a atteint l'âge minimal d'ouverture des droits, false sinon.
   *  
   *  Cette méthode indique si l'individu a atteint l'age minimal d'ouverture des droits. En particulier, elle
   *  recalcule l'âge minimal d'ouverture des droits (RG et FP) en cas de départ anticipé pour carrière longue
   *  (sauf si option.NoRetrAntCarrLongues).
   * 
   *  Remarque: fait appel aux méthodes \ref durees_base, \ref durees_majo et \ref DecoteSurcote
   */
  int AgeMin(); 
  
  /** \brief Retourne l'âge de mise à la retraite d'office de l'individu 
   *
   *  Nécessite que \ref durees_base, \ref durees_majo et \ref DecoteSurcote aient été appelées au préalable.
   * 
   *  Remarque: ne modifie aucune variable.
   */
  double AgeMax();
  
  /** \brief Calcule les pensions hors FP lors de la seconde liquidation (appel de \ref liq_privee et \ref liq_public) 
   * 
   * Calcule les pensions hors FP lors de la seconde liquidation (appel de \ref liq_privee et \ref liq_public) 
    Permet la liquidation finale de tous les droits pour les personnes qui liquident en 2 temps
	(cas des polyaffiliés public-privé, dont le niveau de pension FP (seule) suffit pour déterminer
	leur âge de départ à la retraite, mais qui n'ont pas encore atteint l'âge d'ouverture des droits dans le privé
	au moment de la liquidation dans le public)

	Par hypothèse, on considère que la liquidation finale a lieu dès que la possibilité de partir au taux plein dans le privé
	est atteinte.
  */
  void SecondLiq();
  
  /** \brief Calcule les pensions aux différents régimes (appel de liq_privee et liq_public)

  Simule la liquidation à l'âge courant. Cette fonction n'a pas de valeur de
  retour. Elle modifie les variables pension_rg, pension_fp ... et pension et ageliq.   
  
  Remarque (09/12/2011) :
  Pour permettre la programmation de la liquidation en deux temps des polyaffiliés public/privé,
  deux sous-modules LiqPrive et LiqPublic ont été créés, reprennant la partie de Liq correspondante.
  Ces deux fonctions calculent l'avantage principal de droit direct, HORS MINIMUM et hors bonification.
  Il faut donc ensuite appeler les fonctions qui appliquent ces deux types de majoration.

  */ 
  void Liq();
  


///\{ \name Méthodes de calculs intermédiaires

  /** \brief calcul du SAM à partir des salaires portés au compte.
   * 
   * Calcul du salaire annuel moyen à partir des salaires versés
   * au compte. Le calcul prend en compte l'ensemble des salaires, 
   * contrairement à \ref calc_sam
  */
  double calc_sam(double* spc_begin, double* spc_end);
  
  /** \brief calcul du SAM à partir des salaires portés au compte pour les n meilleures années 
   * 
   * Calcul du salaire annuel moyen à partir des salaires versés
   * au compte. Le calcul ne prend en compte que les n meilleures années,
   * contrairement à \ref calc_sam
  */
  double calc_sam(double* spc_begin, double* spc_end, int duree_calc);
  
  /** \brief Calcul des SAM et du salaire de référence pour la FP 
   * 
   * Calcule les salaires servant de base au calcul des pensions de base, ie. les
   * SAM pour le RG et le RSI et le salaire de référence pour la FP.
   * Les résultats sont stockés dans \ref sam_rg, \ref sam_in et \ref sr_fp.
   * De plus, sont mis à jour les totaux samuni_rgin et samuni.
   *
   * Cette fonction prend en compte pour les législations 1993 et postérieures
   * (l.Legsam), les changements sur le nombre d'années pris en compte dans le
   * calcul du SAM.
   * 
   * Pour les législations postérieures à 2004, les polyaffiliés voient leur
   * calcul de durée de prise en compte du \sig{sam} modifiée.
   * 
   * Pour la fonction publique, on tient compte de l'option FPCarTot. Si cette option est utilisée, on calcule le spc.
   * comme pour le privé (en retenant le nombre d'année en vigueur pour le \sig{rg}).
   * 
   * 
   * - SpcToutesAnnees: on ne retient plus les 25 meilleurs années mais toutes les années.
   * 
   * Prise en compte de l'AVPF via les options NoAVPF et NoSpcAVPF.
   * 
   * Possibilité de calculer un sam unifié RG-IN (option SAMRgInUnique), RG-IN-FP (option SAMUnique), ou un sam
   * distinct par régime (option SAMSepare).
   *
   */
  void SalBase(int AnneeRefAnticip=9999);
  
  /** \brief Calcul des décomptes de points ARRCO et AGIRC.
   * 
    Calcule les nombres de points accumulés dans les régimes ARRCO et AGIRC et stocke les résultats dans
    les variables \ref points_arrco, \ref points_agirc.
	
    La fonction intègre le calcul des points gratuits attribués au titre des périodes de chômage et de préretraite, ainsi que les points gratuits de GMP (garantie minimale de points) à l'Agirc
  */
  void Points(int AnneeRefAnticip=9999);
  
  /** \brief calcul du minmum contributif au RG et au RSI (appelée par AppliqueBonif) 
   * 
   * Calcul du montant attribuable au titre du minimum contributif. Suppose que les
   * durées de cotisation sont connues, donc des appels préalables de DurBase et
   * DurMajo (source, Doc N° 4, séance COR du 27 juin 2006)
   * 
   */
  void MinCont(int AnneeRefAnticip=9999); 
  
  /** \brief calcul du minimum garanti (appelée par AppliqueBonif)
   * 
    Calcul du montant attribuable au titre du minimum garanti : période transitoire
    entre 2004 et 2013 avec application progressive de la réforme. Suppose que
    \brief duree_fp ait été renseignée, i.e. un appel préalable de \ref durees_base.
   */
  double MinGaranti();
  
  /** \brief Application du minimum contributif et du minimum garanti 
   * 
		Fonction appelée par liq ou secondLiq.
		Cette fonction applique les dispositifs de minimum contributif et de minimum garanti 
		à l'ensemble des pensions, le calcul des avantages principaux
		de droits directs hors min ayant été réalisé au préalable.
  
		L'écrêtement des minima est programmé selon une règle tous régimes, prise dans la LFSS 2009 
		pour une application à partir de 2012.
  */
  void AppliqueMin();
  
  /** \brief Application des majorations de pensions pour enfants à l'ensemble des pensions de droit direct 

    Cette fonction applique les majorations de pensions pour enfants 
    à l'ensemble des pensions, de droit direct ou dérivé.
    
    Comme cette fonction n'est appelée que juste après le calcul des avantages principaux 
    de droit direct OU dérivé, on utilise les variables
    $agefin_primoliq, $agefin_totliq et $agerevliq pour savoir à quelles pensions la bonification 
    doit être appliquée (et, a contrario,
     auxquelles elle l'a déjà été au cours d'étapes précédentes).
     
    Remarque : on utilise notamment le fait que, pour les droits directs, 
    la fonction est appelée APRES le calcul du montant de pension (hors bonification)
    mais AVANT la définition des variables d'âge fin.
    
    Remarque : ne sont pas encore programmées ici :
    - la modification du taux de bonification à l'Agirc et à l'Arrco pour les points acquis après 2012
    - la proratisation de la limite de 1000 euros par an pour la bonification Agirc et Arrco après 2012
   */
  void AppliqueBonif();
  
  
  /** \brief Calcul de la pension au RG, au RSI, à l'Arrco et à l'Agirc (appelée par Liq ou SecondLiq) */
  void LiqPrive(int AnneeRefAnticip=9999);
  
  /** \brief Calcul de la pension FP (appelée par Liq)*/
  void LiqPublic();
  
///\}
  

///\{ \name Méthodes renvoyant des informations supplémentaires

  /** \brief Renvoie le dernier salaire brut */
  double dernier_salaire_brut();
  
  /** \brief Renvoie l'age au dernier emploi */
  double age_dernier_emploi();
  
  /** \brief Renvoie la pension perçue à un âge et à un régime donnés (tout régime par défaut) */ 
  double pension_age(int age, int regime);
  
  // \brief Méthodes facilitant le calcul d'indicateur en renvoyant
  // la valeur d'une variable pour un régime particulier
  /** \brief Renvoie la durée majorée dans le régime */
  double duree_maj(int regime) {
    if(regime==REG_TOT) return duree_tot_maj;
    if(regime==REG_RG) return duree_rg_maj;
    if(regime==REG_IN) return duree_in_maj;
    if(regime==REG_FP) return duree_fp_maj;
    return duree_rg_maj;
  }
  
  /** \brief Renvoie la durée, hors majorations, dans le régime */
  double duree(int regime) {    
    if(regime==REG_TOT) return duree_tot;
    if(regime==REG_RG) return duree_rg;
    if(regime==REG_IN) return duree_in;
    if(regime==REG_FP) return duree_fp;
    if(regime==REG_AR) return duree_ar;
    if(regime==REG_AG) return duree_ag;
	if(regime==REG_AGIRC_ARRCO) return duree_ag_ar;
    return 0;
  }
  
  /** \brief Renvoie la part de l'année précédant la liquidation totale en fonction de l'âge */
  double partavtliq(int age) {
      return agefin_totliq ? min_max(arr_mois(agefin_totliq - age, X.moisnaiss+1),  0, 1) : 1;
  }
  
  /** \brief Renvoie la part de l'année précédant la primo-liquidation en fonction de l'âge */
  double partavtprimo(int age) {
      return agefin_primoliq ? min_max(arr_mois(agefin_primoliq - age, X.moisnaiss+1),  0, 1) : 1;
  }
  
  /** \brief Renvoie la pension proratisée en fonction de la date de revalorisation */
  double pension_prorat(double pension, double daterevalo, double RevaloReg, double partavtliq=0) { // TODO: mettre ailleurs
      return pension * (1/RevaloReg*
               (max(daterevalo, partavtliq)-partavtliq)
               +(1-max(daterevalo, partavtliq)));
  }

  /** \brief Renvoie la pension proratisée par âge et par régime */
  double pension_prorat(int age, int regime) {
    int t = X.date(age);
    double pens(0);
    if((regime==REG_TOT || regime==REG_RG) && pension_rg>0) pens += pension_prorat(pension_rg, M->daterevalobase[t],  M->RevaloRG[t], partavtliq(age));
    if((regime==REG_TOT || regime==REG_IN) && pension_in>0) pens += pension_prorat(pension_in, M->daterevalobase[t],  M->RevaloRG[t], partavtliq(age));
    if((regime==REG_TOT || regime==REG_FP) && pension_fp>0) pens +=  pension_prorat(pension_fp, M->daterevalobase[t],  M->RevaloFP[t], partavtprimo(age));
    if((regime==REG_TOT || regime==REG_AR) && pension_ar>0) pens +=  pension_prorat(pension_ar, M->daterevalocomp[t],   M->ValPtARRCO[t-1]/ M->ValPtARRCO[t], partavtliq(age));
    if((regime==REG_TOT || regime==REG_AG) && pension_ag>0) pens +=  pension_prorat(pension_ag, M->daterevalocomp[t],   M->ValPtAGIRC[t-1]/ M->ValPtAGIRC[t], partavtliq(age));
	if((regime==REG_TOT || regime==REG_AGIRC_ARRCO) && pension_ag_ar>0) pens +=  pension_prorat(pension_ag_ar, M->daterevalocomp[t],   M->ValPtAGIRC_ARRCO[t-1]/M->ValPtAGIRC_ARRCO[t], partavtliq(age));
    return pens;    
  }
  
  const  vector<int> statuts_cho         = {S_CHO, S_CHOBIT, S_CHONONBIT} ;
const  vector<int> statuts_pr          = {S_PR};    
const  vector<int> statuts_inv         = {S_INVAL, S_INVALRG} ;     
const  vector<int> statuts_SNat        = {S_SNAT} ; 
const  vector<int> statuts_Avpf        = {S_AVPF} ; 
const  vector<int> statuts_emprg       = {S_NC, S_NONTIT, S_CAD}  ; 
const  vector<int> statuts_fpa         = {S_FPAE, S_FPAAE, S_FPATH, S_FPAATH}; 
const  vector<int> statuts_fps         = {S_FPSE, S_FPSTH, S_INVALFP} ; 
const  vector<int> statuts_cad         = {S_CAD}  ;
const  vector<int> statuts_ind         = {S_IND} ;
const  vector<int> statuts_ind_assimil = {S_IND, S_INVALIND};


/** \brief Renvoie la durée en emploi dans le régime */
  double duree_emp_reg(int regime) {
    if(regime==REG_TOT) return duree_emp;
    if(regime==REG_RG) return duree_emp - duree_fpa - duree_fps - duree_in;
    if(regime==REG_IN) return duree_in;
    if(regime==REG_FP) return duree_fp;
    if(regime==REG_AR) return ((l.an_leg < 2015 || t < 119 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco)? duree_emp - duree_fpa - duree_fps - duree_in : duree_trim(Statuts_occ,2018) - duree_trim(statuts_fpa,2018) - duree_trim(statuts_fps,2018 ) - options->NoAssimil ? duree_trim(statuts_ind,2018) : duree_trim(statuts_ind_assimil,2018));
    if(regime==REG_AG) return duree_ag;
	if(regime==REG_AGIRC_ARRCO) return ((l.an_leg < 2015 || t < 119 || options->NoAccordAgircArrco || options->NoRegUniqAgircArrco)? 0 : duree_emp - duree_fpa - duree_fps - duree_in - (duree_trim(Statuts_occ,2018) - duree_trim(statuts_fpa,2018) - duree_trim(statuts_fps,2018) - options->NoAssimil ? duree_trim(statuts_ind,2018) : duree_trim(statuts_ind_assimil,2018))) ;
    return 0;
  }
  
  /** \brief Renvoie le montant du VFU dans le régime */
  double VFU(int regime) {
    if(regime==REG_TOT) return VFU_rg+VFU_ar+VFU_ag;
    if(regime==REG_RG) return VFU_rg;
    if(regime==REG_AR) return VFU_ar;
    if(regime==REG_AG) return VFU_ag;    
    //if(regime==REG_AGIRC_ARRCO) return VFU_ag_ar;  
    return 0; // Pas de VFU modélisé dans les autres régimes
  }
  
  /** \brief Renvoie, pour un régime donné, une indicatrice valant 1 en cas de bénéfice du minimum de pension, et 0 sinon */
  double min_reg(int regime) {
    if(regime==REG_TOT) return indic_mc || indic_mg || indic_mc_in;
    if(regime==REG_RG) return indic_mc;
    if(regime==REG_FP) return indic_mg;
    if(regime==REG_IN) return indic_mc_in;    
    return 0; 
  }
  
  /** \brief Renvoie, pour un régime donné, le montant de la majoration accordée au titre des enfants. Lorsque
   * regime=REG_TOT, comprend également le montant des minima de pensions.
   */
  double majo_3enf(int regime) {
    if(regime==REG_TOT) return (
        majo_min_rg + majo_min_in + majo_min_fp + 
        majo_3enf_rg + majo_3enf_ar + majo_3enf_ag + 
        majo_3enf_in + majo_3enf_fp);
    if(regime==REG_RG) return majo_3enf_rg;
    if(regime==REG_IN) return majo_3enf_in;
    if(regime==REG_FP) return majo_3enf_fp;
    if(regime==REG_AR) return majo_3enf_ar;
    if(regime==REG_AG) return majo_3enf_ag;
    return 0;
  }
  
  /** \brief Renvoie, pour un régime donné, le montant de la majoration accordée au titre du minimum de pension */
  double majo_min(int regime) {
    if(regime==REG_TOT) return majo_min_rg + majo_min_in + majo_min_fp;
    if(regime==REG_RG) return majo_min_rg;
    if(regime==REG_FP) return majo_min_fp;
    if(regime==REG_IN) return majo_min_in;    
    return 0;
  }
  
  /** \brief Renvoie, pour un régime donné, le coefficient de décote/surcote */
  double taux_liq(int regime) {
    if(regime==REG_TOT) return 0;
    if(regime==REG_RG) return tauxliq_rg;
    if(regime==REG_IN) return tauxliq_rg;
    if(regime==REG_FP) return tauxliq_fp;
    if(regime==REG_AR) return tauxliq_ar;
    if(regime==REG_AG) return tauxliq_ar;
	if(regime==REG_AGIRC_ARRCO) return tauxliq_ar; // Idem dans le cadre du régime unifié : taux liquidation == tauxliq_ar
    return 0;
  }
  
  /** \brief Renvoie, pour un régime donné, le coefficient de proratisation */
  double taux_prorat(int regime) {
    if(regime==REG_TOT) return taux_prorat_rg+taux_prorat_in+taux_prorat_fp;
    if(regime==REG_RG) return taux_prorat_rg;
    if(regime==REG_IN) return taux_prorat_in;
    if(regime==REG_FP) return taux_prorat_fp;
    return 0;
  }

  /** \brief Renvoie, pour un régime donné, la pension annualisée à la liquidation */
  double pensionliq(int regime) {
    if(regime==REG_TOT) return pension;
    if(regime==REG_RG) return  pension_rg;
    if(regime==REG_IN) return  pension_in;
    if(regime==REG_FP) return  pension_fp;
    if(regime==REG_AR) return  pension_ar;
    if(regime==REG_AG) return  pension_ag;
    if(regime==REG_AGIRC_ARRCO) return pension_ag_ar;

    return 0;
  }
  
  /** \brief Renvoie, pour un régime donné, l'âge au 31/12 de l'année de liquidation */
  double age_liq(int regime) {
    if(regime == REG_TOT) return ageprimoliq;
    if(regime == REG_FP) return ageprimoliq;
    if(regime == REG_FPE) return ageprimoliq;
    if(regime == REG_FPT) return ageprimoliq;
    return ageliq;
  }
  
  /** \brief Renvoie le salaire brut moyen des cinq années précédant la liquidation */
  double derSalNet() {
    return X.SalMoyBrut(ageliq,5);
  }
  
  /** \brief Renvoie l'âge au dernier emploi */
  int ageFinEmp() {
    for(int age = ageliq; age >= 0; age--) {
      if(in(X.statuts[age], Statuts_occ))
        return age;
    }
    return 0;
  }
  
  /** \brief Renvoie l'âge au dernier statut d'activité */
  int ageFinAct() {
    for(int age = ageliq; age >= 0; age--) {
      if(in(X.statuts[age], Statuts_act))
        return age;
    }
    return 0;
  }
  /** \brief Calcul du coefficient temporaire de décote ou surcote pour les régimes complémentaires 
  suite à l'accord Agirc-Arcco d'octobre 2015
  
   */
  void coeffTempo();

///\}
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
