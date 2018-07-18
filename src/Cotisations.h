/**
 * \file Cotisations.h
 * \brief Fichier contenant les outils permettant le calcul des cotisations prélevées sur les revenus d'activité et la pension de retraite.
 */
 
 #pragma once

class Indiv;


/**
 * \fn CSGRet(Indiv& X, int age)
 * \brief \ref CSGRet renvoie le montant de CSG prélevé sur la pension totale (droit direct et réversion) perçue par X à l'âge age.
 * 
 * \param X individu
 * \param age Âge de l'individu
 */
double CSGRet(Indiv& X, int age);

/**
 * \fn CSGSal(Indiv& X, int age)
 * \brief \ref CSGSal renvoie le montant de CSG prélevé sur les revenus d'activité perçus par X à l'âge age.
 * 
 * \param X individu
 * \param age Âge de l'individu
 */
double CSGSal(Indiv& X, int age);

/**
 * \fn CotAut(Indiv& X, int age)
 * \brief Renvoie le montant des cotisations salariales, autres que la CSG et
 * les cotisations retraite, prélevées sur les revenus d'activité perçus par X à l'âge age, à savoir :
 * - dans le public : la cotisation maladie, la cotisation au Fonds de solidarité 
 * - dans le privé (non-cadre) : les cotisations au titre du chômage et de la maladie
 * - dans le privé (cadre) : les cotisations au titre du chômage, de la maladie, et APEC
 * - pour les non-salariés : la cotisation au titre de la maladie
 * 
 * \param X individu
 * \param age Âge de l'individu
 */
double CotAut(Indiv& X, int age);

/**
 * \fn CotRet(Indiv& X, int age)
 * \brief Renvoie le montant des cotisations retraite (part salariale) prélevées sur les revenus d'activité perçus par X à l'âge age.
 *  
 * \param X individu
 * \param age Âge de l'individu
 */
double CotRet(Indiv& X, int age);

/**
 * \fn CotRetrPatr(Indiv& X, int age)
 * \brief Renvoie le montant des cotisations retraite (part patronale) prélevées sur les revenus d'activité perçus par X à l'âge age.
 *  
 * \param X individu
 * \param age Âge de l'individu
 */
double CotRetrPatr(Indiv& X, int age) ;

/**
 * \fn SalMoy(Indiv& X, int age, int nb_an)
 * \brief Renvoie le salaire moyen net entre l'âge age-nb_an+1 et l'âge age. Les salaires pris en compte sont revalorisés sur le SMPT.
 * 
 * \param X individu
 * \param age Âge de l'individu
 * \param nb_an nombre d'années prises en compte dans le calcul du salaire moyen
 */
double SalMoy(Indiv& X, int age, int nb_an);

/**
 * \fn PNet(Indiv& X, int age)
 * \brief Renvoie la pension totale (droit direct et réversion) perçue par X à l'âge age, nette de la CSG et des cotisations
 *  maladies dans les complémentaires du privé
 * 
 * \param X individu
 * \param age Âge de l'individu
 */
double PNet(Indiv& X, int age);

/**
 * \fn cotAGFFTot(Indiv& X, int age)
 * \brief Renvoie le montant de la cotisation AGFF (parts employeur et salariale) prélevée sur les revenus d'activité perçus
 *  par X à l'âge age
 * 
 * \param X individu
 * \param age âge de l'individu
 */
double cotAGFFTot(Indiv& X, int age);

/**
 * \class Cotisations
 * \brief La classe Cotisations contient, entre autres choses, le salaire et les cotisations retraites à une date donnée.
 * 
 * La classe Cotisations contient les éléments suivants :
 * <ul>
 * <li> attributs :
 * <ul>
 *             <li> l'année
 *             <li> l'identifiant de l'individu
 *             <li> l'âge de l'individu au cours de l'année
 *             <li> le statut de l'individu au cours l'année
 *             <li> la fraction de l'année précédant la première liquidation
 *             <li> le salaire de l'individu au cours de l'année
 *             <li> le salaire net de l'individu au cours l'année
 *             <li> le montant des cotisations retraite (part salariale) prélevées sur les rémunérations de l'individu au cours de l'année
 *             <li> le montant des cotisations retraite (part employeur) prélevées sur les rémunérations de l'individu au cours de l'année
 *  </ul>
 * <li> méthodes :   un constructeur
 * </ul>
 */
class Cotisations {
public:
  int Id, annee, age, statut;
  double partavtprimo, salaire, salaire_net, cotis_retr, cotis_retr_patr;
  
  Cotisations(Indiv& X, int t) {
      Id = X.Id;
      annee = t;
      age = X.age(t);
      partavtprimo = X.retr->primoliq ? min_max(arr_mois(X.retr->primoliq->agefin_primoliq - age, X.moisnaiss+1),  0, 1) : 1;
      statut = (X.retr->primoliq && partavtprimo < 1) ? 8 : X.statuts[age];
      salaire         = X.salaires[age] * partavtprimo;
      salaire_net     = X.salaires[age] - CotRet(X,age) - CotAut(X,age) - CSGSal(X,age) * partavtprimo;
      cotis_retr      = CotRet(X,age) * partavtprimo;
      cotis_retr_patr = CotRetrPatr(X,age) * partavtprimo;
  }
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