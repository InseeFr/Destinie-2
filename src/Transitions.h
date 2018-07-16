#pragma once
#include "Simulation.h"

/**
 * \file Transitions.h
 * \brief Fichier contenant la structure \ref Transitions.
 * 
 * \ref Transitions contient à la fois les imports des équations de transition sur le marché du travail et des données de calage. 
 * Elle contient aussi les fonctions réalisant les transitions ainsi définies.
 * Usage :
 * ```
 * // [[Rcpp::export]]
 * void destinieTransMdt(Environment envSim) {
 *    // A la fin de la fonction les tables R sont exportées et tous les objects C sont détuits
 * }
 * ```
 * 
 */




/**
 * \struct EqTrans
 *  \brief EqTrans contient les équations de transition sur le marché du travail.
 */
struct EqTrans {
  double
  
      /**
       * \brief Coefficient devant l'indicatrice d'âge égale à 55 ans <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
      */
      _(a55)         ,
      /**
       * \brief Coefficient devant l'indicatrice d'âge égale à 56 ans <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(a56)             , 
      /**
       * \brief Coefficient devant l'indicatrice d'âge égale à 57 ans <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(a57),     
      /**
       * \brief Coefficient devant l'indicatrice d'âge égale à 58 ans <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(a58),
      
      /**
       * \brief  Coefficient devant l'âge <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(age)             ,
      /**
       * \brief Coefficient devant l'âge l'année suivant la fin des études <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(age_a),   
      /**
       * \brief Coefficient devant l'âge, de 1 à 3 ans après la fin des études <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(age_b),
      
      /**
       * \brief Coefficient devant l'âge au carré  <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(age2)        , 
      /**
      * \brief Coefficient devant l'âge l'année suivant la fin des études, au carré <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
      */
      _(age2_a)          ,  
      /**
       * \brief Coefficient devant l'âge, de 1 à 3 ans après la fin des études, au carré <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(age2_b),  
      /**
       * \brief  Coefficient devant l'âge au cube <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(age3),
      
      /**
       * \brief  Coefficient devant l'âge l'année suivant la fin des études, au cube <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(age3_a)      ,  
      /**
       * \brief  Coefficient devant l'âge, de 1 à 3 ans après la fin des études, au cube <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(age3_b)          ,  
      /**
       * \brief Coefficient devant le nombre d’années en emploi depuis la fin de la formation initiale <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(dur_emp),
      
      /**
       * \brief Coefficient devant le nombre d’années consécutives dans le dernier statut <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(dur_statut)  ,  
      /**
       * \brief Coefficient devant la durée au chômage dans les 5 dernières années <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(durchom)         ,  
      /**
       * \brief Coefficient devant la durée en inactivité dans les 5 dernières années <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(durINA), 
      /**
      * \brief Coefficient devant le nombre d’années en inactivité (hors scolarité) dans les 5 dernières années <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       *Note: les descriptions de durINA et de durina sont peut-être interverties.
      */
      _(durina),
      /**
      * \brief Coefficient devant une variable caractéristique d'avoir de 1 à 3 
      * années d'expériences après les études <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
      */
      _(exp1a3)      ,  
      /**
       * \brief Coefficient devant l’âge de fin d’études relatif <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014
       */
      _(findet_rel)      ,  
      /**
       * \brief Coefficient devant l’âge de fin d’études relatif, l'année suivant la fin de la scolarité <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014 
       */
      _(findet_rel_a),
      
      /**
       * \brief Coefficient devant l’âge de fin d’études relatif, de 1 à 3 ans d'expérience
       *  suivant la fin de la scolarité <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014 
       */
      _(findet_rel_b),  
      /**
       * \brief Coefficient devant la constante <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014 
       */
      _(Intercept)       ,  
      /**
       * \brief Coefficient devant le nombre d'enfants <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014 
       */
      _(nbenf),
      
      /**
       * \brief Coefficient devant le nombre d'enfants de moins de 1 an <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014 
       */
      _(NbEnf1)      ,  
      /**
       * \brief Coefficient devant le nombre d'enfants de moins de 3 ans <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014 
       */ 
      _(NbEnf3)          , 
      /**
       * \brief Coefficient devant le nombre d'enfants 
       * (pour les équations sur le début de la vie professionnelle) <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014 
       */
      _(nbenf98),
      
      /**
       * \brief Coefficient devant le nombre d'enfants la première année après la fin des études <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014 
       */
      _(nbenf98_a)   ,  
      /**
       * \brief Coefficient devantle nombre d'enfants entre 1 et 3 ans après la fin des études <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014 
       */
      _(nbenf98_b)       ,  
      /**
       * \brief Coefficient, non utilisé dans le modèle, devant le fait d'être en pré-retraite. 
       * Note : Ce coefficient était nécessaire lors de la phase d'estimation, pour ne pas biaiser
       * les projections. En effet :
       * "L’estimation des équations de transition a été effectuée à une période durant laquelle 
       * les actifs pouvaient partir en préretraite ; le volume d’inactifs y était donc particulièrement 
       * élevé. Afin de neutraliser cet effet des préretraites et donc de ne pas surestimer en projection 
       * les volumes d’inactifs,deux variables ont été créées sur l’échantillon d’estimation :
       *  une variable d’éligibilité à la préretraite4 pour l’individu considéré, à partir de l’âge, 
       *  l’année etla durée cotisée ; une variable relative au volume de préretraites l’année d’observation.
       *  Le produit de ces deux variables permet de créer une nouvelle variable utilisée comme variable 
       *  explicative des équations de transition pour tenir compte des préretraites et notamment mieux
       *  modéliser l’effet de l’extinction de certains dispositifs." <br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014 
       */
      _(PreRetr_ARPE),
      
      /**
       * \brief voir PreRetr_ARPE
       */
      _(PreRetrCAATA),  
      /**
       * \brief voir PreRetr_ARPE
       */
      _(PreRetrhorsCAATA),  
      /**
      * \brief Coefficient visant à prendre en compte l'évolution de la participation des femmes au marché du travail, 
        avec une fonction croissante et coudée dans le temps, en log.
        Les raisons expliquant sa modalité d'implémentation actuelle ne sont plus connues.
      */
      _(probpart),
      
      /**
       * \brief Coefficient devant une indicatrice pour la première transition à la sortie des études initiales.<br>
       * Source : estimé à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
       *de Bachelet, Leduc, Marino, 2014 
       */
      _(sortant)     ;
  int
       /**
       * \brief Le statut de destination de la transition modélisée (voir _origine).
       */
      _(destination)    ,
      /**
      * \brief ordre de la transition car les estimations de transition sont des logit emboîtés.
      * De 0 à 6.
      */
      _(ordre),
      /**
      * \brief Le type de transition modélisée.
      * Par sexe, le début de carrière (à partir l’enquête Génération 1998),
      * le milieu de carrière ou la fin de carrière (avec l’échantillon inter-régimes de cotisants (EIC)).
      * Les modalités prises sont donc : DEBUTH, DEBUTF, MILIEUH, MILIEUF, FINH, FINF.
      */
      _(type_trans),
      /**
      * \brief Le statut de départ de la transition modélisée.
      * Il y a 7 statuts envisagés : PRI (salarié du secteur privé), INA (inactif), CHO (chômage),
      * MAL (malade ou invalide), IND (être indépendant), TIT (titulaire de la Fonction Publique),
      * CON (les contractuels de la Fonction Publique). <br>
      * Note: Dans l'estimation, une personne est considérée malade ou invalide une année si le nombre 
      * de trimestres cotisés est nul, si sa rémunération est nulle et si elle a validé des trimestres 
      * au titre de la maladie (et plus de trimestres maladie que de trimestres au titre du chômage).
      */
      _(origine)            ,
      /**
      * \brief indic==2 correspond à la dernière transition (état complémentaire des états précédemment examinés).
      */
      _(indic);
};




/**
 * \struct CiblesTrans 
 * \brief CiblesTrans contient les cibles de calage pour avoir la bonne structure de population sur le marché du travail.
 */

struct CiblesTrans {
  NumericVector
  
  /**
   * \brief Cible de taux de chômage en projection chez les hommes de 15 à 19 ans  <br>
   * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
   * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012 <br>
   * Feuille : Param_transitions/ choX%
   */
      _(CHO_H1519),
      /**
      * \brief Cible de taux de chômage en projection chez les hommes de 20 à 24 ans  <br>
      * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
       * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
       * Feuille : Param_transitions/ choX%
      */ 
      _(CHO_H2024),
    /**
    * \brief Cible de taux de chômage en projection chez les hommes de 25 à 29 ans  <br>
    * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
     * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
     * Feuille : Param_transitions/ choX%
    */  
    _(CHO_H2529),
    /**
     * \brief Cible de taux de chômage en projection chez les hommes de 30 à 34 ans <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
     * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
     * Feuille : Param_transitions/ choX%
     */ 
    _(CHO_H3034),
    /**
     * \brief Cible de taux de chômage en projection chez les hommes de 35 à 39 ans  <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
     * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
     * Feuille : Param_transitions/ choX%
     */
    _(CHO_H3539),
    /**
     * \brief Cible de taux de chômage en projection chez les hommes de 40 ans à 44 ans  <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
     * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
     * Feuille : Param_transitions/ choX%
     */
    _(CHO_H4044), 
    /**
     * \brief Cible de taux de chômage en projection chez les hommes de 45 à 49 ans  <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
     * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
     * Feuille : Param_transitions/ choX%
     */
    _(CHO_H4549) ,
    /**
    * \brief Cible de taux de chômage en projection chez les hommes de 50 à 54 ans  <br>
    * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
     * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
     * Feuille : Param_transitions/ choX%
    */
    _(CHO_H5054),
    /**
     * \brief Cible de taux de chômage en projection chez les femmes de 15 à 19 ans   <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
     * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
     * Feuille : Param_transitions/ choX%
     */
      _(CHO_F1519),
      /**
       * \brief Cible de taux de chômage en projection chez les femmes de 20 à 24 ans <br>
       * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
       * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
       * Feuille : Param_transitions/ choX%
       */
      _(CHO_F2024),
      /**
       * \brief Cible de taux de chômage en projection chez les femmes de 25 à 29 ans  <br>
       * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
       * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
       * Feuille : Param_transitions/ choX%
       */
      _(CHO_F2529),
      /**
       * \brief Cible de taux de chômage en projection chez les femmes de 30 à 34 ans  <br>
       * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
       * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
       * Feuille : Param_transitions/ choX%
       */
      _(CHO_F3034),
      /**
       * \brief Cible de taux de chômage en projection chez les femmes de 35 à 39 ans  <br>
       * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
       * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
       * Feuille : Param_transitions/ choX%
       */
      _(CHO_F3539),
      /**
       * \brief Cible de taux de chômage en projection chez les femmes de 40 à 44 ans  <br>
       * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
       * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
       * Feuille : Param_transitions/ choX%
       */
      _(CHO_F4044),
      /**
       * \brief Cible de taux de chômage en projection chez les femmes de 45 à 49 ans  <br>
       * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
       * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
       * Feuille : Param_transitions/ choX%
       */
      _(CHO_F4549) ,
      /**
       * \brief Cible de taux de chômage en projection chez les femmes de 50 à 54 ans  <br>
       * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage ; 
       * estimés sur l'Enquête Emploi en Continu pour les années 2010-2011-2012  <br>
       * Feuille : Param_transitions/ choX%
       */
      _(CHO_F5054),
      /**
       * \brief Cible de taux d'inactivité en projection chez les hommes de 15 à 19 ans  <br>
       * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
       * Feuille : Param_transitions/ choX%
       */
      
	  _(INA_H1519),
	  /**
	  * \brief Cible de taux d'inactivité en projection chez les hommes de 20 à 24 ans  <br>
	  * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
	   * Feuille : Param_transitions/ choX%
	  */
	  _(INA_H2024),
	  /**
	   * \brief Cible de taux d'inactivité en projection chez les hommes de 25 à 29 ans  <br>
	   * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
	   * Feuille : Param_transitions/ choX%
	   */
	  _(INA_H2529),
	  /**
	   * \brief Cible de taux d'inactivité en projection chez les hommes de 30 à 34 ans <br>
	   * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
	   * Feuille : Param_transitions/ choX%
	   */
	  _(INA_H3034), 
	  /**
	   * \brief Cible de taux d'inactivité en projection chez les hommes de 35 à 39 ans  <br>
	   * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
	   * Feuille : Param_transitions/ choX%
	   */
	  _(INA_H3539),
	  /**
	   * \brief Cible de taux d'inactivité en projection chez les hommes de 40 à 44 ans <br>
	   * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
	   * Feuille : Param_transitions/ choX%
	   */
	  _(INA_H4044),
	  /**
	   * \brief Cible de taux d'inactivité en projection chez les hommes de 45 à 49 ans <br>
	   * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
	   * Feuille : Param_transitions/ choX%
	   */
	  _(INA_H4549) ,
	  /**
	   * \brief Cible de taux d'inactivité en projection chez les hommes de 50 à 54 ans  <br>
	   * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
	   * Feuille : Param_transitions/ choX%
	   */
	  _(INA_H5054),
	  /**
	   * \brief Cible de taux d'inactivité en projection chez les femmes de 15 à 19 ans  <br>
	   * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
	   * Feuille : Param_transitions/ choX%
	   */
    _(INA_F1519), 
    /**
     * \brief Cible de taux d'inactivité en projection chez les femmes de 20 à 24 ans  <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
     * Feuille : Param_transitions/ choX%
     */
    _(INA_F2024),
    /**
     * \brief Cible de taux d'inactivité en projection chez les femmes de 25 à 29 ans  <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
     * Feuille : Param_transitions/ choX%
     */
    _(INA_F2529),
    /**
     * \brief Cible de taux d'inactivité en projection chez les femmes de 30 à 34 ans <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
     * Feuille : Param_transitions/ choX%
     */
    _(INA_F3034), 
    /**
     * \brief Cible de taux d'inactivité en projection chez les femmes de 35 à 39 ans  <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
     * Feuille : Param_transitions/ choX%
     */
    _(INA_F3539), 
    /**
     * \brief Cible de taux d'inactivité en projection chez les femmes de 40 à 44 ans  <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
     * Feuille : Param_transitions/ choX%
     */
    _(INA_F4044), 
    /**
     * \brief Cible de taux d'inactivité en projection chez les femmes de 45 à 49 ans  <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
     * Feuille : Param_transitions/ choX%
     */
    _(INA_F4549) ,
    /**
     * \brief Cible de taux d'inactivité en projection chez les femmes de 50 à 54 ans  <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
     * Feuille : Param_transitions/ choX%
     */
    _(INA_F5054),
    /**
     * \brief Cible de taux d'inactivité en projection chez les femmes de 50 à 54 ans  <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
     * Feuille : Param_transitions/ choX%
     */
    _(PART_FP),
    /**
     * \brief Part de la Fonction Publique d'État dans la Fonction Publique  <br>
     * Source : cible proposée par le COR, avec plusieures variantes de taux de chômage <br>
     * Feuille : Param_transitions/ choX%
     */
    _(PART_FPE);
};
  
  
  
  
  
/**
 * \brief énumération des groupes dans lesquels sont rangés les statuts. Les équations de transitions gouvernent les
 * transitions entre ces groupes. Les statuts finaux sont affinés par la fonction applique_trans.
 */
enum {CHO,  CON,  INA,  IND,  MAL,  PRI,  TIT, XXX, RET, PRE};

/**
 * \brief énumération contenant les labels :
 * - du noeud de l'arbre de décision
 * - des cibles de taux de chomage
 * - des cibles de taux d'inactivité
 */
enum {
  TRANS1, TRANS2, TRANS3, TRANS4, 
  TRANS5, TRANS6, TRANS7,
  CHO_H1519,  CHO_H2024,  CHO_H2529,  CHO_H3034,  CHO_H3539,  CHO_H4044,  CHO_H4549 , CHO_H5054,
  CHO_F1519,  CHO_F2024,  CHO_F2529,  CHO_F3034,  CHO_F3539,  CHO_F4044,  CHO_F4549 , CHO_F5054,
  INA_H1519,  INA_H2024,  INA_H2529,  INA_H3034,  INA_H3539,  INA_H4044,  INA_H4549 , INA_H5054,
  INA_F1519,  INA_F2024,  INA_F2529,  INA_F3034,  INA_F3539,  INA_F4044,  INA_F4549,  INA_F5054,
  FP_M60, FPE_M60, NBTRANS};

/**
 * \brief énumération contenant les labels des périodes de transition (début, milieu et fin de carrière) pour les deux sexes
 */
enum {DEBUTF,DEBUTH,FINF,FINH,MILIEUF,MILIEUH,NBTYPE_TRANS};



/**
 * \struct Transitions
 * \brief La structure Transitions contient tous les éléments utilisés dans la fonction destinieTransMdt.
 * 
 * La structure Transitions contient :
 * <ul>
 *  <li> en attributs: 
 *    <ul>
 *              <li> les coefficients des variables explicatives des équations de transition
 *              <li> les cibles annuels de taux de chômage et d'inactivité, utilisés en cas de transitions calées
 *              <li> les probabilités de transition de chaque individu à chaque noeud de
 *                 l'arbre de décision
 *              <li> la probabilité, pour chaque individu, de transiter vers la FPE conditionnellement
 *                 au fait de transiter vers la FP
 *              <li> pour chaque individu, une indicatrice de sortie de l'arbre de décision.
 *              <li> pour chaque individu et chaque noeud de l'arbre de décision, le statut de sortie
 *                 éventuelle de l'arbre.
 *    </ul>
 * <li> en méthodes :
 *    <ul>
 *              <li> une fonction simulant, sans calage, les transitions d'une année
 *              <li> une fonction simulant, avec calage, les transitions d'une année
 *              <li> une fonction qui applique effectivement la transition à un individu, en remplissant le nouveau statut
 *              <li> une fonction qui simule la transition pour un migrant
 *              <li> une fonction calculant, pour un individu et une date donnés, l'arbre de décision ainsi que les probas
 *                 de transition à chaque noeud de l'arbre.
 *    </ul>
 *</ul>                 
 */
struct Transitions {
  Environment& env;
  
  /**
   * \brief eqtd contient les coefficients des variables de l'équation de transition, et ce, pour une période, un sexe,
   *  un statut initial et un noeud donnés.
   *  
   * Source : estimés à l'occasion de "Les biographies du modèle Destinie II : rebasage et projection"
   * de Bachelet, Leduc, Marino, 2014
   */
  vector3<EqTrans> eqtd = Rdin<EqTrans>("EqTrans", 
    "type_trans", levels(DEBUTF,DEBUTH,FINF,FINH,MILIEUF,MILIEUH),
    "origine",levels(CHO,CON,INA,IND,MAL,PRI,TIT),
    "ordre", levels(TRANS1,TRANS2,TRANS3,TRANS4,TRANS5,TRANS6,TRANS7));
  
  /**
   * \brief cibles contient les cibles de taux de chômage et d'inactivité
   */
  CiblesTrans cibles = Rdin<CiblesTrans>("CiblesTrans");
  int cpt = 0;
  
  /**
   * \brief probas est une matrice contenant les probabilités de transition de chaque individu à chaque noeud de
   *  l'arbre de décision
   */
  vector2<double> probas;
  
  /**
   * \brief probas_fpe contient, pour chaque individu, la probabilité de transiter vers la FPE conditionnellement
   *  au fait de transiter vers la FP
   */
  vector<double> probas_fpe;
  
  /**
   * \brief destination contient, pour chaque individu et chaque noeud de l'arbre de décision, le statut de sortie
   *  éventuelle de l'arbre. Ce statut dépend de la période de transition, du sexe, du statut antétieur et de la
   *  profondeur de l'arbre
   */
  vector2<int> destination;
  
  /**
   * \brief champ contient, pour chaque individu, une indicatrice de sortie de l'arbre de décision. Cette indicatrice
   *  vaut 1 tant que l'individu n'a pas transité, et 0 sinon.
   */
  vector<int> champ;
  /**
   * \brief Constructeur de la structure \ref Transitions
   */
  Transitions(Environment& env) : env(env) {}

  /**
   * \fn make_transitions_noncalees(int t, int age_min, int age_max)
   * \brief Simule, sans calage, les transitions de l'année t par logits emboîtés
   */
  void make_transitions_noncalees(int t, int age_min, int age_max);  
  
  /**
   * \fn make_transitions(int t, int age_min, int age_max)
   * \brief Simule, avec calage, les transitions de l'année t par logits emboîtés. Afin de mettre en oeuvre le calage,
   *  les trois premiers noeuds de l'arbre sont identiques quel que soit le statut en t-1 : transition vers l'inactivité,
   *  vers le chômage ou vers la FP.
   */
  void make_transitions(int t, int age_min, int age_max);
  
  /**
   * \fn applique_trans(int id, int t, int position_arbre)
   * \brief Applique effectivement la transition en remplissant le statut de l'individu id en t. Cette fonction affine
   *  le statut de transition, jusqu'alors rangé dans un groupe.
   */
  void applique_trans(int id, int t, int position_arbre);
  
  /**
   * \fn transMigr(Indiv& X, int age)
   * \brief Simule la transition pour le migrant X à l'âge age. Pour les migrants, les transitions respectent, en coupe,
   *  des distributions de statut par sexe et par groupes d'âge (trois en tout).
   */
  void transMigr(Indiv& X, int age);
  
  /**
   * \fn ProbTrans(Indiv& X, int t)
   * \brief Calcule l'arbre de décision associé à X à la date t ainsi que les probas de transition à chaque noeud de
   *  l'arbre. L'arbre de décision dépend du statut de X en t-1.
   */
  void ProbTrans(Indiv& X, int t);
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