# Destinie 2

Le modèle Destinie 2 (modèle Démographique Économique et Social de Trajectoires INdividuelles sImulÉs, version 2) est un modèle de microsimulation dynamique, développé et géré par l’Insee, dont l’objectif principal est la projection à long terme des retraites. Il succède à Destinie 1, premier modèle de microsimulation élaboré sur ce domaine en France, depuis 2010.


## Prise en main


Dans RStudio, ouvrir le projet Destinie-2 puis « Install and Restart ».


Voir page 106 de la documentation.

### Depuis un shell

Depuis le dossier Destinie-2 (root du dépôt)

```
R CMD INSTALL . --preclean --debug
Rscript demo/simulation.R --file server/example.xlsx
test -f server/example.results.xlsx
```


Il sera très vraisemblablement nécessaire de lancer plusieurs fois la commande suivante avec divers noms de libraries.

```
Rscript -e ".libPaths('~/R-tests');install.packages(c('xlsx'))"
```

### Notes de debug 😅


```
# 3. Installer les packages nécessaires à Destinie :
install.packages(c("devtools","pkgbuild"))
```

ERROR: dependencies 'usethis', 'covr', 'httr', 'roxygen2', 'rversions' are not available for package 'devtools'


```
export LC_ALL=C
apt-get install libssl-dev libcurl4-openssl-dev libxml2-dev
```

```
# 4. Installation du package Destinie :
# pour obtenir un "TRUE".
library(devtools)
devtools::find_rtools()
# ou pkgbuild::find_rtools()


install.packages(c("zip", "openxlsx", "plogr", "rJava", "tidyselect", "xlsxjars", "xlsx"))


devtools::install()
```

```
Rscript -e ".libPaths('~/R-tests');install.packages(c('ggplot2', 'tidyr', 'dplyr', 'openxlsx', 'xlsx'))"
Rscript -e ".libPaths('~/R-tests');install.packages(c('xlsx'))"
```


https://github.com/hannarud/r-best-practices/wiki/Installing-RJava-(Ubuntu)

apt-get install default-jre
apt-get install default-jdk

R CMD javareconf

apt-get install r-cran-rjava


## Utilisation

Ce modèle est mobilisé lors des exercices de projection de long terme conduits par le Conseil d’Orientation des Retraites (COR) ou la Commission européenne, mais aussi dans le cadre d'études afin d'éclairer le débat économique et social dans le domaine de la protection sociale (retraites, mais également dépendance ou encore dépenses de santé). En effet, la construction d’un échantillon démographique, couplée à la constitution des liens familiaux et des trajectoires professionnelles, offre au modèle une perspective plus généraliste, qui s’étend au-delà des retraites stricto sensu. Sa dimension « ménages » le distingue d’autres modèles de microsimulation. Par ailleurs, la taille modeste de l'échantillon permet une simulation rapide et ainsi la réalisation de nombreuses variantes, au prix d’une plus faible précision des résultats.



## Méthodologie

Le modèle s’appuie sur un échantillon représentatif de la population française au 1er janvier 2010, construit à partir de l’enquête Patrimoine 2009-2010. Composé d'environ 62 000 individus, cet échantillon initial contient des informations sur les liens familiaux ainsi que l'historique des positions occupées sur le marché du travail. 
La simulation est effectuée au niveau individuel. Le modèle renouvelle la population initiale en simulant des naissances, des décès et des flux migratoires. Il projette ensuite les carrières professionnelles, les revenus d’activité et le départ à la retraite de chaque individu. 
Les personnes en emploi sont réparties en trois grands groupes (les salariés du secteur privé, les titulaires de la fonction publique et les indépendants), comme les retraités (en autant de régimes de base). Ce modèle simplifie un certain nombre d’éléments législatifs (par exemple, les indépendants sont simulés en étant tous soumis aux mêmes règles de calcul des droits). Destinie 2 permet de reconstituer l’ensemble de la trajectoire professionnelle d’un individu (statuts d’activité et revenus) et simule la liquidation de la retraite selon les règles en vigueur pour chaque régime. 

## Trois modules principaux

Le modèle comprend trois modules distincts, écrits en C++ et transcrits en R : le module générateur des biographies démographiques, le module générateur des trajectoires professionnelles et le module «  retraite ».
La construction des trajectoires démographiques s’appuie sur les projections démographiques de l’Insee publiées en 2016, couvrant le champ France entière sur la période 2013-2070. Le nombre de décès, de naissances et les flux migratoires sont ainsi calés chaque année de la projection. 
Les parcours professionnels des individus sont observés jusqu’en 2009, année de base. À compter de 2010, leurs carrières (statuts d’activité et revenus) sont projetées en respectant des contraintes de calage fondées sur des hypothèses macroéconomiques. Ces hypothèses portent sur les gains de productivité du travail, le taux de chômage, tous deux repris des publications du COR, et le taux d’activité, issu des projections de population active de l’Insee.
Le module « retraite » simule le départ à la retraite et calcule le montant des droits associés. Ces droits incluent les droits directs et la pension de réversion des régimes de base (le régime général de la Sécurité sociale, le Service des Retraites de l’État (SRE), la Caisse nationale de retraites des agents des collectivités locales (CNRACL), la Sécurité Sociale pour les Indépendants (SSI)) et de certains régimes complémentaires (Association pour le régime de retraite complémentaire des salariés (Arrco), Association générale des institutions de retraite complémentaire des cadres (Agirc)). Outre la réversion, la dimension « ménages » permet de simuler l’attribution de l’Allocation de Solidarité aux Personnes Âgées (Aspa).

## Plusieurs options de simulation

Le modèle permet une simulation à la carte via un vaste choix d'options. Certaines options permettent d’évaluer la robustesse des résultats obtenus, d'autres ont été rajoutées à l’occasion d’études spécifiques. Les options incluent :
- le scénario démographique de l’Insee utilisé en projection relatif à la mortalité, à la fécondité ou encore au solde migratoire.
- la législation à appliquer : soit la dernière législation intégrée, soit une précédente législation.
- le type de mortalité : prise en compte ou non d'une mortalité différenciée selon le niveau de diplôme.

Les autres options présentes dans le modèle sont documentées sur le dépôt en ligne.

## Licence
Destinie 2
Copyright © 2005-2018, Institut national de la statistique et des études économiques
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

## Contact
#########################	FRANCAIS	#########################

En cas de question, merci de lire attentivement la documentation fournie,
notamment les fichiers documentation.pdf et conseils_pratiques.txt.
Veuillez noter qu'il existe de nombreuses ressources en ligne décrivant le 
modèle, tel Buffeteau et al (2011), Bachelet et al (2014), et d'autres.
Certains documents de travail déjà parus pourraient vous aider : ils sont
énumérés dans le fichier Destinie_DT.txt
En cas de retours sur le modèle, merci d'envoyer un courriel à
dg75-g201@insee.fr

#########################	ENGLISH		#########################

If you have questions, please read carefully the provided documentation,
especially the files documentation.pdf and conseils_pratiques.txt (in French). 
You can also find online useful ressources describing the model, 
such as Buffeteau et al (2011), Bachelet et al (2014), and other:
they are listed in the file Destinie_DT.txt
If you have any feedback on the model, please send an e-mail at 
dg75-g201@insee.fr



