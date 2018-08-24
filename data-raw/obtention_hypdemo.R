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
options(java.parameters = "-Xmx2048m") #souvent nécessaire pour charger les fichiers excel
library(xlsx)
library(openxlsx)
library(dplyr)

################
#on se place dans le répertoire Destinie/destinie
####################
#setwd()
#######################
# construction des paramètres démographiques
#############################################
# il faut choisir le champ "FE" ou "FM"
# et les hypothèses pour la projection du solde migratoire m
# l'espérance de vie ev
# et la fécondité f
# 1 signifie scénario central
# 2 scénario bas
# 3 scénario haut
# 4 travail
###############
champ="FE"
m=1
ev=1
f=1

##############
fec=ifelse(f==1,"Cent",
           ifelse(f==2,"Bas",
                  ifelse(f==3,"Haut","Travail")))
evie=ifelse(ev==1,"Cent",
            ifelse(ev==2,"Bas",
                   ifelse(ev==3,"Haut","Travail")))
mi=ifelse(m==1,"Cent",
          ifelse(m==2,"Bas",
                 ifelse(m==3,"Haut","Travail")))
Cent=c("*","","","")
Bas=c("","*","","")
Haut=c("","","*","")
Travail=c("","","","*")
vec=c(1,2,3,4)

x=data.frame(fecondite=ifelse(vec==f,"*","")
             ,esp_vie=ifelse(vec==ev,"*",""),migration=ifelse(vec==m,"*",""))

row.names(x)<-c("Cent","Bas","Haut","Travail")
sc_demo=t(x)
rm(x)

wb=xlsx::loadWorkbook(".\\parametres\\Param_demo\\sc_demo.xls")
feuilles=xlsx::getSheets(wb)
feuille=feuilles[[2]]
xlsx::addDataFrame(sc_demo,sheet = feuille,row.names = T,col.names=T, startRow = 1,startColumn = 1)


xlsx::saveWorkbook(wb, ".\\parametres\\Param_demo\\sc_demo.xls") 

openXL(paste0(".\\parametres\\Param_demo\\ciblesDemographie_",champ,".xls"))
######################
#on met à jour les liens, on sauvegarde le fichier qui contient les paramètres de projections choisis
###################
demo=new.env()
morta <- xlsx::loadWorkbook(paste0(".\\parametres\\Param_demo\\ciblesDemographie_",champ,".xls"))
demo$Survie_H <- as.matrix(xlsx::readColumns(morta$getSheet("SH"),2,122,2,colClasses = "numeric"))
demo$Survie_F <- as.matrix(xlsx::readColumns(morta$getSheet("SF"),2,122,2,colClasses = "numeric"))
demo$Qmort_H <- as.matrix(xlsx::readColumns(morta$getSheet("QH"),2,122,2,colClasses = "numeric"))
demo$Qmort_F <- as.matrix(xlsx::readColumns(morta$getSheet("QF"),2,122,2,colClasses = "numeric"))
demo$espvie_H<- as.matrix(xlsx::readColumns(morta$getSheet("espvieH"),2,122,2,colClasses = "numeric"))
demo$espvie_F<- as.matrix(xlsx::readColumns(morta$getSheet("espvieF"),2,122,2,colClasses = "numeric"))
demo$mortadiff_dip_F <- xlsx::read.xlsx(paste0(".\\parametres\\Param_demo\\MORTA_DIP.xls"),sheetName ="morta_dif_F",startRow = 1)
demo$mortadiff_dip_H <- xlsx::read.xlsx(paste0(".\\parametres\\Param_demo\\MORTA_DIP.xls"),sheetName ="morta_dif_H",startRow = 1)
demo$mortalite_diff <-  xlsx::read.xlsx(".\\parametres\\PARAM_Mortalite_diff.xls",sheetName = "Mortalite_diff",startRow = 2)



NA0<-function (x)
{return (ifelse(is.na(x),0,x))}
demo$CiblesDemo <- xlsx::read.xlsx(paste0(".\\parametres\\Param_demo\\ciblesDemographie_",champ,".xls"),sheetName ="CiblesDemo",startRow = 2,colClasses=rep("numeric",48))%>%
  mutate_all(funs(NA0))
####################
# on sauvegarde le tout dans le réperoire data
save(demo,file=paste0("data/fec_",fec,"_vie_",evie,"_mig_",mi,".Rdata"))
