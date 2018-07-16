


library(dplyr)
library(reshape2)
#' Outil import_series
#' @description [obsolète?] Importation de séries de parametres macroéconomiques et sociaux
#' @param scCor : nom du fichier excel à importer
import_series <- function(scCor){
  liste_series <- list(
    "ParamGene" = c("annee",  "Prix",   "PlafondSS",   "SMIC",   "PointFP",   "SMPT",   "PIB"),
    "ParamRetrBase" = c(
      "annee",
      "RevaloSPC",        "RevaloRG",         "RevaloFP",         "TauxSalRGSP",      "TauxEmpRGSP",      
      "TauxSalRGSalTot",  "TauxEmpRGSalTot",  "TauxFP",           "TauxEmplFPE",      "TauxEmplFPTH",     
      "TauxEmplFPMoy",    "MinVieil1",        "MinVieil2",        "Mincont1",         "Mincont2",         
      "MinPR",            "CoefMdaRG",        "CoefMdaFP",        "SeuilVFURG",       "SalValid"
    ),
    "ParamRetrComp" =
      c( "annee",
         "TauxARRCO_1",    "TauxARRCO_2",    "TauxARRCO_S1",   "TauxARRCO_S2",   "TauxAppARRCO",   
         "SalRefARRCO",    "ValPtARRCO",     "TauxAGIRC_B",    "TauxAGIRC_C",    "TauxAGIRC_SB",   
         "TauxAGIRC_SC",   "TauxAppAGIRC",   "SalRefAGIRC",    "ValPtAGIRC",     "GMP", "sGMP" ,          
         "TauxMalComp" 
      ),
    "ParamAutres" = c(
      "annee",           "TauxMalSP",        "TauxMalTot",       "TauxMalPubTr",     "TauxMalPubTot",    
      "TauxAGFF_1",       "TauxAGFF_2",       "TauxAssedic",      "TauxCSGSal",       "TauxMalRet",   "TauxMalRetFra",    
      "TauxMalRetEtr",  "TauxCSGRetFort",   "TauxCSGRetMin",    "SeuilExoCSG",      "seuilAbbat" ,
      "SeuilPauvrete"
    ),
    "ParamRev" = c(
      "annee",           "TauxRevRG",        "MinRevRG",       "MaxRevRG",     "PlafRevRG",    
      "TauxRevARRCO",       "TauxRevAGIRC",       "TauxRevFP",      "TauxRevInd"
    ),
    "ParamFam" = c(
      "annee", "BMAF", "PlafCF1", "PlafCF2", "PlafCF3", "PlafCF4", "PlafCF5", "MajoPlafCF", 
      "PlafARS1", "PlafARS2", "PlafARS3", "PlafARS4", "PlafARS5"
    )
  )
  
  wb <- xlsx::loadWorkbook(paste0(".\\parametres\\",scCor))
  df <- data.frame(annee=1900:2170)
  for(sheet in names(liste_series)) {
    name = paste(sheet);
    size = length(unlist(liste_series[sheet]));
    print(name)
    dfs <- readColumns(wb$getSheet(name),1,size,2,colClasses = "numeric")
    names(dfs) <- unlist(liste_series[sheet])
    df <-merge(df, dfs,  all.x=TRUE)
  }
  df <- as.data.frame(lapply(df,function(x) ifelse(is.na(x), 0, x)))

  return(df);
}

loglogist <- function(alea,q1,v1,q2,v2){
  a <- (log(q1/(1-q1)) - log(q2/(1-q2))) / log(v1/v2);
  b <- v1**a*(1-q1)/q1;
  return( (b*alea/(1-alea))**(1/a) );
}


#' Outil recode
#' @description [obsolète?] [usage?]
recode <- function(var,...,.type="factor") {
  new <- c(...)
  if(is.numeric(var)) {old <- as.numeric(names(new))} 
  else {  old <- names(new)}
  if(is.factor(var)) {  var <- as.character(var)}
  
  for(i in seq(old)) {
    var[var==old[i]] = new[i]
  }
  
  if(.type=="factor") {return (as.factor(var))}
  else if(.type=="numeric") {return (as.numeric(var))}
  else if(.type=="integer") {return (as.integer(var))}
  else {return(as.character(var))}
}

#' Outil projection
#' @description [fonction-clé de simulation.R] Permet de projeter des séries suivant une hypothèse donnée.
projection <- function(data,...) {
  tmp2 <- lazyeval::lazy_dots(...)
  for(tmp1 in tmp2) {
    tmp <- mutate_(data,.val=as.character(tmp1$expr[3]))
    vars <- as.character(tmp1$expr[2])
    
    if(vars==".") {
      vars = paste0(ls(data),collapse = "|")
    }
    lst_vars <- strsplit(vars,"[ |]")[[1]]
    for(v in lst_vars) {
      if(v != "" && v != "." && v %in% names(data)) {
        fin <- length(data[[v]])
        pos <- dplyr::last(1:fin,order_by=!is.na(data[[v]]))
        per <- (pos+1):fin
        if(length(per) > 1 && pos < fin) {
        data[[v]][per] <- tmp[[".val"]][per] / tmp[[".val"]][pos] * data[[v]][pos] 
        }
      } else if(v!=""){
        warning(v)
      }
    } 
  }
  data[[".val"]] <- tmp[[".val"]]
  
  return(data)
}

#' Outil read_xls
#' @description Importe les paramètres stockés dans les fichiers excel avec les options 
#' d'importation correctement paramétrées 
#' @param rep : nom du fichier excel a importer
#' @param sheet : nom des onglets a recuperer
read_xls <- function(rep,sheet,...) {
  df <- as.data.frame(list(...))
  for(s in sheet) {
    df <- left_join(df,read.xlsx2(rep,s,startRow=3,colClasses = rep("numeric",255)))
  }
  return(df)
}