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
#documentation Rpackage
library(devtools)
find_rtools()
Rcpp::compileAttributes()
devtools::document()
#puis lancer un clean+Rebuild

##############################################
#documentation C++ : creation d un pdf de documentation
library(rdoxygen)
##############################################


#on indique que le working directory est le dossier clone
setwd(dirname(rstudioapi::getActiveDocumentContext()$path))
getwd()


#######################
# on edite la documentation
#######################
# pour changer la place de la documentation, je recharge  les fonctions de rdoxygen
# doxy_init() 

replace_tag <- function (fileStrings, tag, newVal) {
  
  # get and count lines with the tag
  iLine  <- grep(paste0("^", tag, "\\>"), fileStrings)
  nLines <- length(iLine)
  
  if (nLines == 0){
    # if tag is not present, add it with its value at the bottom
    line <- paste0(tag, "\t= ", newVal)
    iLine <- length(fileStrings) + 1
  } else if (nLines > 0){
    # if tag is present once, replace its value
    line <- gsub("=.*", paste0("= ", newVal), fileStrings[iLine])
    if(nLines > 1){
      # if tag is present multiple times, do nothing and throw warning
      warning(paste0(
        "File has", nLines, 
        "for key", tag, ". ",
        "Check it up manually."
      ))
    }
  }
  fileStrings[iLine] <- line
  
  return(fileStrings)
}

doxy_init_bis=function (rootFolder = ".") 
{
  doxyFileName <- "Doxyfile"
  initFolder <- getwd()
  if (rootFolder != ".") {
    setwd(rootFolder)
  }
  rootFileYes <- length(grep("DESCRIPTION", dir())) > 0
  doxDir <- "documentation/doxygen"
  if (!file.exists(doxDir)) {
    dir.create(doxDir, recursive = TRUE)
  }
  setwd(doxDir)
  system(paste0("doxygen -g ", doxyFileName))
  doxyfile <- readLines("Doxyfile")
  doxyfile <- replace_tag(doxyfile, "EXTRACT_ALL", "YES")
  doxyfile <- replace_tag(doxyfile, "INPUT", "src/")
  doxyfile <- replace_tag(doxyfile, "OUTPUT_DIRECTORY", "inst/doxygen/")
  cat(doxyfile, file = doxyFileName, sep = "\n")
  setwd(initFolder)
  return(NULL)
}

doxy_init_bis()

doxy_edit(pathToDoxyfile = "./documentation/doxygen/Doxyfile",options = 
            c("GENERATE_HTML"="NO","OUTPUT_DIRECTORY"="./documentation/doxygen","PROJECT_NAME"="Destinie 2","OUTPUT_LANGUAGE"="FRENCH",
                      "INPUT_ENCODING"="UTF-8","DOXYFILE_ENCODING"="UTF-8","SHOW_NAMESPACES"="NO","EXCLUDE_PATTERNS" = "*/range.h /\ */OutilsRcpp.h",
              "COMPACT_LATEX"="YES","EXTRACT_ALL"="NO","HIDE_SCOPE_NAMES"="YES","LATEX_TIMESTAMP"="YES","HAVE_DOT"="NO" 
              ,"REPEAT_BRIEF"="NO","CITE_BIB_FILES"="./documentation/doxygen/latex/bibliography.bib","LATEX_BIB_STYLE"="apalike-fr","LATEX_SOURCE_CODE"="YES",
              "EXTRA_PACKAGES"="{booktabs}    {lscape}     {listings} "))

doxy(pathToDoxyfile = "./documentation/doxygen/Doxyfile")



#Lorsque les #include minimaux et  nécessaires auront été repris:
#tout en haut
#telecharger graphviz, par exemple ici: https://graphviz.gitlab.io/_pages/Download/Download_windows.html
#et changer le lien vers le dossier contenant dot.exe :
#dossier_graphvz <- "D:/Program_Files/graphviz-2.38/release/bin"
#dans le doxy_edit, rajouter les options:
#"HAVE_DOT"="YES","DOT_PATH"=dossier_graphvz,"COLLABORATION_GRAPH" = "NO",
#"CLASS_DIAGRAMMS"          = "NO","CALLER_GRAPH"="YES"

