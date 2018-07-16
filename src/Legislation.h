/**
 * \file Legislation.h
 * \brief Contient la classe \ref Leg qui définit l'ensemble des paramètres législatifs pour un individu à un âge donné
 */


#pragma once
#include "Simulation.h"
#include "Indiv.h"

#define UNDEF 9999


/**
 * \class Leg
 * \brief Contient les valeurs des paramètres législatifs pour un individu*age donnés
 * 
 * \ref Leg contient les valeurs des paramètres législatifs pour un individu X et une année
 * donnés. Ces paramètres sont ceux qui ne sont pas directement importés depuis le
 * fichier Excel de paramètres sociaux. Ces paramètres législatitifs 
 * dépendent en général des caractéristiques individuelles, tels que la génération le sexe
 * l'année, et qui sont utiles pour le calcul des droits à la liquidation avec DroitsRetr.
 * 
 */

class  Leg {
  public:
    double DureeMinFPA;         	  ///<	Durée minimale de carrière pour bénéficier d'une pension de fonctionnaire pour la catégorie active (ajout  23/09/2011)
    int    an_leg;                     ///<	année de législation
    int    an_leg_DRA;                 ///<	année de la législation appliquée en ce qui concerne les départs en retraite anticipé
    int    an_leg_SAM;                 ///<	année de législation appliquée en ce qui concerne le calcul du SAM
    double DureeCibRG;              ///<	 Durée cible au régime général
    double DureeProratRG;           ///<	 Durée de proratisation du régime général
    double DureeCalcSAM;            ///<	 Nombre d'années pour calcul du SAM dans le RG
    double TauxPleinRG;             ///<	 Taux plein du régime général
    double DecoteRG;                ///<	 Coefficient de décote du régime général (par année d'écart au taux plein)
    double SurcoteRG1;              ///<	 Coefficient de surcote du régime général, 1ere année
    double SurcoteRG2;              ///<	 Coefficient de surcote du régime général, 2eme année
    double SurcoteRG3;              ///<	 Coefficient de surcote du régime général, 3eme année
    double TauxRGMax;               ///<	 Taux maximal de la pension RG
    double MajTauxRGMax;            ///<	 Majoration du taux maximal de pension pris en compte pour le plafonnement
    double DureeCibFP;              ///<	 Duree cible de la fonction publique
    double DureeProratFP;           ///<	durée de référence pour une carrière complète à la FP (au dénominateur du coefficient de proratisation)
    double DecoteFP;                ///<	 Decote fonction publique
    double SurcoteFP;               ///<	 Surcote fonction publique
    double AgeAnnDecFP;             ///<	 Age d'annulation de la décote fonction publique.
    double AgeMinMG;                ///<	 Age minimum pour toucher le minimum garanti (ajout  27/10/2010)
    double AgeMaxFP;                ///<	 Age de liquidation maximal FP
    int    LegMin;                  ///<	 Millésime de la législation appliquée pour le calcul des minima de pensions
    int    LegDRA;                  ///<	 Millésime de la législation appliquée pour les départs anticipés
    int    LegSAM;                  ///<	 Millésime de la législation appliquée pour le calcul du SAM
    double AgeMinRG;                ///<	 Age minimum d'ouverture des droits, RG
    double AgeMaxRG;                ///<	 Age de liquidation maximal RG
    double AgeAnnDecRG;             ///<	 Age d'annulation de la décote au RG
    double DureeMinFP;              ///<	 Durée minimale de carrière pour bénéficier d'une pension de fonctionnaire (ajout  25/10/2010)
    double AnOuvDroitFP;            ///<	 année d'ouverture des droits dans la fonction publique (peut être différent de l'année des 60 ans pour les catégories actives et les mères de 3 enfants) (ajout Patrick 09/12/2011)
    double AgeSurcote;              ///<	 Age minimal pour le calcul de la surcote (60 ans avant la réforme de 2010, 62 ans progressivement après)
    double DeplafProrat;            ///<	 Durée au-delà de la durée de référence prise en compte dans le calcul du coefficient de proratisation (dans tous les régimes) (dans le cadre d'un déplafonnement progressif de ce coefficient)
    double DureeValCibDRA[5];       ///<	 Durée validée requise pour une départ anticipée pour carrière longue (plusieurs "jeux" de condition de départ sont possibles, il s'agit donc d'un tableau)
    double DureeCotCibDRA[5];       ///<	 Durée cotisée requise pour une départ anticipée pour carrière longue (plusieurs "jeux" de condition de départ sont possibles, il s'agit donc d'un tableau)
    double DebActCibDRA[5];         ///<	 âge de début d'activité requis pour une départ anticipée pour carrière longue (plusieurs "jeux" de condition de départ sont possibles, il s'agit donc d'un tableau)
    double AgeDRA[5];               ///<	 âge d'ouverture des droits dans le cadre d'un départ anticipée pour carrière longue (sous condition de durée et âge défini par l'indice)
    double AgeMinFP;                ///<	 Age minimum d'ouverture des droits, FP (cet âge est défini par individu, après analyse de la catégorie pertinente 
    double ageouvdroitfp;           ///<	 Age d'ouverture des droits pour les fonctionnaires sédentaires calculé en fonction de la génération
    int    durfpa;                  ///<	 Durée passée dans la catégorie active
    
    
    /**
     * \fn Leg(const Indiv& X, int age, int an_leg)
     * \brief Créer un objet législation pour un individu particulier, à un age donné.
     * 
     * Créer un objet législation pour un individu particulier, à un age donné, et une législation donnée.
     * 
     * appelle les sous-méthodes suivantes :
     * - leg_priv (Législation régimes RG, RSI, Arrco, Agirc)
     * - leg_pub  (Législation régime FP)
     * - leg_spe  (Régimes spéciaux)
     * - leg_dra  (Législation sur les départ en retraite anticipée pour carrière longue)
     * - an_ouv_droit_fp (Calcul de l'année d'ouverture des droits au FP)
     **/
    Leg(const Indiv& X, int age, int an_leg);   
    
private:
    void leg_priv(const Indiv & X, int age);
    void leg_pub(const Indiv & X, int age);
    void leg_spe(const Indiv & X, int age);
    void leg_dra(const Indiv & X, int age);
    double an_ouv_droit_fp(const Indiv & X, int age);
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