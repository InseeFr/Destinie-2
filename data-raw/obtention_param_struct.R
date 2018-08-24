# Destinie 2
# Copyright © 2005-2018, Institut national de la statistique et des études économiques
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

##############################################
library(xlsx)
library(dplyr)

################
#on se place dans le répertoire Destinie/destinie
####################
#setwd()
############
#chargement des paramètres demo 
################
struct=new.env()
struct$FinEtudeMoy <-  xlsx::read.xlsx(".\\parametres\\PARAM_etude.xls",sheetName = "TABLEFINDET0", startRow = 2)

struct$EqSalaires <-  xlsx::read.xlsx(".\\parametres\\EqSalaires.xls",sheetName="eq_salaires")
struct$CStructSexeAge <-  xlsx::read.xlsx(".\\parametres\\EqSalaires.xls",sheetName="CorrectStructSalSexeAge")
struct$EqTrans <-  xlsx::read.xlsx(".\\parametres\\EqTrans.xls","EqTrans") %>%  
  mutate(indic=as.integer(indic)) %>%
  mutate(ordre=as.factor(paste0("TRANS",ordre+1))) %>%
  arrange(type_trans,origine,ordre)
struct$EqSante <-  xlsx::read.xlsx(".\\parametres\\sante.xls",sheetName="adl")

save(struct,file="data/eq_struct.rda")
