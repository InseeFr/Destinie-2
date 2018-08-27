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
library(destinie)
################
#on se place dans le répertoire Destinie/destinie
####################
#setwd()
############
#chargement des paramètres économiques
################
scencho <- "7%" # ou 4,5 ou 10
cho<-ifelse(scencho=="7%","7",
           ifelse(scencho=="4,5%","45","10"))
scenprod <- "1,5%" #ou 1,0 ou 1,5 ou 1,8
prod<-ifelse(scenprod=="1,5%","15",
             ifelse(scenprod=="1,3%","13",
                    ifelse(scenprod=="1,0%","10","18")))
ciblesTrans <- ".\\parametres\\Projection_COR_2018\\PARAM_transitions.xls"
param_eco <- ".\\parametres\\Projection_COR_2018\\ParamSociaux_2018.xls"
fin_simul=2120

eco=new.env()
eco$macro <-
  read_xls(param_eco,c("ParamGene","ParamRetrBase","ParamRetrComp","ParamRev","ParamAutres","ParamFam",paste0("cho",scencho,"_pib",scenprod)),annee=1900:fin_simul)
 

eco$CiblesTrans <-  xlsx::read.xlsx(ciblesTrans,sheetName = paste0("cho",scencho),startRow = 3,colIndex=1:36)
assign(paste0("eco_cho_",cho,"_prod",prod),eco)
save(list=paste0("eco_cho_",cho,"_prod",prod),file=paste0("data/eco_cho_",cho,"_prod",prod,".rda"))