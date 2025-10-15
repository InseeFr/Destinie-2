library(dplyr)

loglogist <- function(alea, q1, v1, q2, v2) {
  a <- (log(q1 / (1 - q1)) - log(q2 / (1 - q2))) / log(v1 / v2)
  b <- v1**a * (1 - q1) / q1
  return((b * alea / (1 - alea))**(1 / a))
}

#' Outil recode
#' @description [obsolète?] [usage?]
recode <- function(var, ..., .type = "factor") {
  new <- c(...)
  if (is.numeric(var)) {
    old <- as.numeric(names(new))
  } else {
    old <- names(new)
  }
  if (is.factor(var)) {
    var <- as.character(var)
  }

  for (i in seq(old)) {
    var[var == old[i]] <- new[i]
  }

  if (.type == "factor") {
    return(as.factor(var))
  } else if (.type == "numeric") {
    return(as.numeric(var))
  } else if (.type == "integer") {
    return(as.integer(var))
  } else {
    return(as.character(var))
  }
}

#' Outil projection
#' @description [fonction-clé de simulation.R] Permet de projeter des séries suivant une hypothèse donnée.
#' @export
projection <- function(data, ...) {
  tmp2 <- lazyeval::lazy_dots(...)
  for (tmp1 in tmp2) {
    tmp <- mutate(data, .val = !!rlang::parse_expr(as.character(tmp1$expr[3])))
    vars <- as.character(tmp1$expr[2])

    if (vars == ".") {
      vars <- paste0(ls(data), collapse = "|")
    }
    lst_vars <- strsplit(vars, "[ |]")[[1]]
    for (v in lst_vars) {
      if (v != "" && v != "." && v %in% names(data)) {
        fin <- length(data[[v]])
        pos <- dplyr::last(seq_len(fin), order_by = !is.na(data[[v]]))
        per <- (pos + 1):fin
        if (length(per) > 1 && pos < fin) {
          data[[v]][per] <- tmp[[".val"]][per] / tmp[[".val"]][pos] * data[[v]][pos]
        }
      } else if (v != "") {
        warning(v)
      }
    }
  }

  return(data)
}

#' Importation des paramètres stockés dans les fichiers excels
#' 
#' Les options d'importation sont déjà paramétrées.
#' 
#' @param rep Nom du fichier excel a importer
#' @param sheet Nom des onglets a recuperer
#' 
#' @export
read_merge_xls <- function(rep, sheet, ...) {
  df <- as.data.frame(list(...))
  for (s in sheet) {
    df <- left_join(df, xlsx::read.xlsx2(rep, s, startRow = 3, colClasses = rep("numeric", 255)))
  }
  return(df)
}


#' Détermination du nom de la variante démographique associée à un jeu d'hypothèses
#'
#' @param scenario_fecondite : scénario d'évolution de la fécondité
#' @param scenario_esp_vie : scénario d'évolution des espérances de vie
#' @param scenario_migration : scénario d'évolution des flux migratoires
#'
#' @return Le nom de la variante démographique
#' @export
#'
#' @examples
#' get_variante_demo_name("bas", "haut", "central")
get_variante_demo_name <- function(
  scenario_fecondite = NULL,
  scenario_esp_vie = NULL,
  scenario_migration = NULL
) {
  if (is.null(scenario_fecondite) || is.null(scenario_esp_vie) || is.null(scenario_migration)) {
    return(NULL)
  }
  switch(
    paste(scenario_fecondite, scenario_esp_vie, scenario_migration),
    "central central central" = NULL,
    "haut bas haut" = "jeune",
    "bas haut bas" = "agee",
    "haut central central" = "fec_haute",
    "central bas central" = "mortal_haute",
    "central central haut" = "mig_haute",
    "bas central central" = "fec_basse",
    "central haut central" = "mortal_basse",
    "central central bas" = "mig_basse",
    {
      warning("Pas de nom spécifique, utilisation de noms par défaut")
      variante_demo_name |>
        filter(
          fecondite == scenario_fecondite,
          esp_vie == scenario_esp_vie,
          migration == scenario_migration
        ) |>
        pull(name)
    }
  )
}



#' Importation des paramètres relatifs aux scénarios économiques de projection du COR
#' 
#' Les règles d'indexations permettant de prolonger les séries macroéconomiques
#' dépendent de la législation utilisée. Il est donc supposé que cette
#' information est déjà indiquée dans l'environnement de simulation, dans les 
#' options.
#' 
#' @param envir Environnement de simulation (objet Simul)
#' @param date_fin_simulation Année de fin de simulation (2070 par défaut)
#' @param scenario_productivite Scénario COR de productivité de long terme (1,0\% par défaut)
#' @param scenario_chomage Scénario COR de chômage de long terme (7,0\% par défaut)
#' @param scenario_fecondite Scénario d'évolution de la fécondité (optionnel, seulement pour les variantes démo)
#' @param scenario_esp_vie Scénario d'évolution des espérances de vie (optionnel, pour les variantes démo)
#' @param scenario_migration Scénario d'évolution des flux migratoires (pour les variantes démo)
#' @param annee_hypo_cor Année des hypothèses COR à considérer (si NULL : les plus récentes)
#' 
#' @export
import_scenario_eco_cor <- function(
    envir, date_fin_simulation = 2070,
    scenario_productivite = "1,0%", scenario_chomage = "7%",
    scenario_fecondite = NULL, scenario_esp_vie = NULL, scenario_migration = NULL,
    annee_hypo_cor = NULL
) {
  if (is.null(annee_hypo_cor)) {
    # Récupération des dernieres hypothèses COR dispo dans le package
    annee_hypo_cor <- 2023L
    while ((system.file("extdata", paste0("COR_", annee_hypo_cor + 1L), package = "destinie")) != "") {
      annee_hypo_cor <- annee_hypo_cor + 1L
    }
  }
  hypo_path <- system.file("extdata", paste0("COR_", annee_hypo_cor), package = "destinie")
  if (hypo_path == "") {
    stop("Les hypothèses COR de l'année : ", annee_hypo_cor, " ne sont pas disponibles")
  }
  message(paste0("Année des hypothèses du COR : ", annee_hypo_cor))
  if (annee_hypo_cor <= 2021) {
    scenario_cor_disponibles <- c("1,0%-7%", "1,3%-7%", "1,5%-7%", "1,8%-7%", "1,0%-10%", "1,8%-4,5%")
  } else if (annee_hypo_cor == 2022) {
    # Changement des scénarios d'évolution de la productivité du travail à LT depuis les hypos COR 2022
    scenario_cor_disponibles <- c("0,7%-7%", "1,0%-7%", "1,3%-7%", "1,6%-7%", "0,7%-10%", "1,6%-4,5%")
  } else {
    # Nouvelles variantes pour les hypos COR 2023
    scenario_cor_disponibles <- c(
      "0,4%-5%", "0,7%-5%", "1,0%-5%", "1,3%-5%", "1,6%-5%",
      "0,4%-7%", "0,7%-7%", "1,0%-7%", "1,3%-7%", "1,6%-7%",
      "0,4%-10%",
      "0,0%-7%", # Variante zero croissance de productivité
      "1,6%-5%-jeune", # Ancienne variante population jeune
      "1,3%-5%-jeune", # Variante population jeune
      "1,3%-5%-fec_haute", # Sous cas de la variante population jeune
      "1,3%-5%-mortal_haute", # Sous cas de la variante population jeune
      "1,3%-5%-mig_haute", # Sous cas de la variante population jeune
      "0,4%-7%-agee", # Variante population agée
      "0,4%-7%-fec_basse", # Sous cas de la variante population agée
      "0,4%-7%-mortal_basse", # Sous cas de la variante population agée
      "0,4%-7%-mig_basse" # Sous cas de la variante population agée
    )
  }
  
  variante_name <- get_variante_demo_name(
    scenario_fecondite,
    scenario_esp_vie,
    scenario_migration
  )
  scenario_input <- stringr::str_c(
    scenario_productivite, scenario_chomage, variante_name,
    sep = "-"
  )
  if (!scenario_input %in% scenario_cor_disponibles) {
    stop(
      "Veuiller choisir un scénario du COR (cible productivité/chômage) disponible : ",
      paste(scenario_cor_disponibles, collapse = "\n")
    )
  }

  message("Scénario (productivité/chômage) retenu : ", scenario_input)
  message("Import des objets macro, CiblesTrans, EqTrans, EqSalaires, CStructSexeAge et options_salaires dans l'environnement")

  # Ouverture des excels de paramètres économiques (PARAM_transitions.xls et ParamSociaux.xls)
  cibles_trans_path <- file.path(hypo_path, "PARAM_transitions.xls")
  param_eco_path <- file.path(hypo_path, "ParamSociaux.xls")

  # Remplissage de macro selon le scénario considéré
  # => merge de toutes les feuilles excel en 1 seul data.frame stocké dans macro
  if (annee_hypo_cor >= 2021) { # les paramètres sont dépendants du scénario PIB à partir des hypo COR 2021
    sheets <- c(
      "ParamGene",
      paste0("ParamRetrBase_pib", scenario_productivite),
      paste0("ParamRetrComp_pib", scenario_productivite),
      "ParamRev", "ParamAutres", "ParamFam",
      stringr::str_c(
        paste0("cho", scenario_chomage),
        paste0("pib", scenario_productivite),
        variante_name,
        sep = "_"
      )
    )
  } else {
    sheets <- c(
      "ParamGene", "ParamRetrBase", "ParamRetrComp",
      "ParamRev", "ParamAutres", "ParamFam",
      paste0("cho", scenario_chomage, "_pib", scenario_productivite)
    )
  }

  envir$macro <- lapply(sheets, FUN = readxl::read_excel, path = param_eco_path, skip = 2)
  envir$macro <- reshape::merge_recurse(envir$macro) %>% arrange(annee)

  # Prolongement des séries macro à partir des règles d'indexation
  if (is.null(envir$options$anLeg)) {
    stop("anLeg doit être renseignée dans les options de simulation afin de pouvoir appliquer la bonne règle d'indexation")
  }
  envir$macro <- indexation_eco(
    macro = envir$macro |> filter(annee %in% 1900:date_fin_simulation),
    anLeg = envir$options$anLeg
  )

  # Remplissage de CiblesTrans (cibles chômage/inactivité) selon le scénario considéré:
  envir$CiblesTrans <- left_join(
    envir$macro %>% select(annee),
    xlsx::read.xlsx(cibles_trans_path,
      sheetName = paste0("cho", scenario_chomage),
      startRow = 3, colIndex = 1:36
    ),
    by = "annee"
  )

  # Objets indépendants du scénario éco :

  # Coefficients des équations de transition de statut pro
  envir$EqTrans <- xlsx::read.xlsx(
    system.file("extdata", "EqTrans.xls",
      package = "destinie", mustWork = TRUE
    ),
    sheetName = "EqTrans"
  ) %>%
    mutate(indic = as.integer(indic)) %>%
    mutate(ordre = as.factor(paste0("TRANS", ordre + 1))) %>%
    arrange(type_trans, origine, ordre)

  # coefficients des équations de salaire
  envir$EqSalaires <- xlsx::read.xlsx(
    system.file("extdata", "EqSalaires.xls",
      package = "destinie", mustWork = TRUE
    ),
    sheetName = "eq_salaires"
  )

  # correction de structure des salaires entre hommes et femmes
  envir$CStructSexeAge <- xlsx::read.xlsx(
    system.file("extdata", "EqSalaires.xls",
      package = "destinie", mustWork = TRUE
    ),
    sheetName = "CorrectStructSalSexeAge"
  )

  envir$options_salaires <- list() # c'est une liste vide

  return(envir)
}

#' Projection des séries macroéconomiques à partir des règles d'indexation.
#'
#' Certaines séries fournies par le COR ne sont pas renseignées jusqu'à la fin
#' de la simulation. A partir des règles d'indexation, les valeurs manquantes
#' sont complétées.
#'
#' @param macro Table des séries macroéconomiques.
#' @param anLeg Année de législation utilisée.
#'
#' @return Une table macro contenant
indexation_eco <- function(macro, anLeg) {
  # Prise en compte de l'Accord National Interprofessionnel Agirc-Arrco 2023/10
  # indépendemment de l'année de législation.
  if (anLeg <= 2022) {
    macro %>%
      mutate(
        SMPTp = ifelse(is.na(SMPTp), 0, SMPTp),
        SMICp = ifelse(is.na(SMICp), 0, SMICp),
        PIBp = ifelse(is.na(PIBp), 0, PIBp),
        PlafondSSp = ifelse(is.na(PlafondSSp), 0, PlafondSSp),
        Prixp = ifelse(is.na(Prixp), 0, Prixp),
        MinPRp = 1.02,
        # revaloRG, revaloFP, revaloSPC remplies entièrement par données COR (nov 2020)
        RevaloRG = ifelse(is.na(RevaloRG), 1 + Prixp, RevaloRG),
        RevaloFP = ifelse(is.na(RevaloFP), 1 + Prixp, RevaloFP),
        RevaloSPC = ifelse(is.na(RevaloSPC), 1 + Prixp, RevaloSPC),
        MinVieil1 = as.numeric(MinVieil1)
      ) %>%
      projection(
        SMPT ~ cumprod((1 + SMPTp) * (1 + Prixp)),
        PIB ~ cumprod((1 + PIBp) * (1 + Prixp)),
        PlafondSS ~ cumprod((1 + PlafondSSp) * (1 + Prixp)),
        SMIC ~ cumprod((1 + SMICp) * (1 + Prixp)),
        Prix ~ cumprod(1 + Prixp),
        PointFP | PlafRevRG ~ SMPT,
        SalValid ~ SMIC,
        PlafARS1 | PlafARS2 | PlafARS3 | PlafARS4 | PlafARS5 | PlafCF3 | PlafCF4 | PlafCF5 | MajoPlafCF | sGMP | BMAF | SeuilPauvrete ~ SMPT,
        MaxRevRG ~ PlafondSS,
        PlafMajoRevRG ~ cumprod(1 + Prixp),
        MinPR ~ cumprod(MinPRp * (1 + Prixp)),
        MinVieil1 | MinVieil2 | Mincont1 | Mincont2 ~ lag(cumprod(1 + Prixp)),
        # SalRefARRCO et SalRefAGIRC remplies entièrement par données COR (nov 2020)
        SalRefAGIRC_ARRCO | SalRefARRCO | SalRefAGIRC ~ cumprod(
          ifelse(annee %in% c(2016, 2017, 2018), (1 + SMPTp + 0.02) * (1 + Prixp),
                 ifelse(annee %in% seq(2019, 2033), (1 + SMPTp) * (1 + Prixp), (1 + SMPTp - 0.0116) * (1 + Prixp))
          )
        ),
        # ValPtAGIRC et ValPtARRCO remplies entièrement par données COR (nov 2020)
        ValPtAGIRC | ValPtARRCO | ValPtAGIRC_ARRCO ~ cumprod(1 + ifelse(annee %in% c(2016, 2017, 2018), pmax(Prixp - 0.01, 0), Prixp)), # ValPtAGIRC et ValPtARRCO remplies entièrement par données COR (nov 2020)
        MinRevRG | SeuilExoCSG | SeuilExoCSG2 | SeuilTxReduitCSG | SeuilTxReduitCSG2 ~ cumprod(1 + Prixp),
        . ~ 1
      ) %>%
      arrange(annee)
  } else {
    # Réforme 2023, indexation du Mico sur le SMIC.
    macro %>%
      mutate(
        SMPTp = ifelse(is.na(SMPTp), 0, SMPTp),
        SMICp = ifelse(is.na(SMICp), 0, SMICp),
        PIBp = ifelse(is.na(PIBp), 0, PIBp),
        PlafondSSp = ifelse(is.na(PlafondSSp), 0, PlafondSSp),
        Prixp = ifelse(is.na(Prixp), 0, Prixp),
        MinPRp = 1.02,
        # revaloRG, revaloFP, revaloSPC remplies entièrement par données COR
        RevaloRG = ifelse(is.na(RevaloRG), 1 + Prixp, RevaloRG),
        RevaloFP = ifelse(is.na(RevaloFP), 1 + Prixp, RevaloFP),
        RevaloSPC = ifelse(is.na(RevaloSPC), 1 + Prixp, RevaloSPC),
        MinVieil1 = as.numeric(MinVieil1)
      ) %>%
      projection(
        SMPT ~ cumprod((1 + SMPTp) * (1 + Prixp)),
        PIB ~ cumprod((1 + PIBp) * (1 + Prixp)),
        PlafondSS ~ cumprod((1 + PlafondSSp) * (1 + Prixp)),
        SMIC ~ cumprod((1 + SMICp) * (1 + Prixp)),
        Prix ~ cumprod(1 + Prixp),
        PointFP | PlafRevRG ~ SMPT,
        SalValid ~ SMIC,
        PlafARS1 | PlafARS2 | PlafARS3 | PlafARS4 | PlafARS5 | PlafCF3 | PlafCF4 | PlafCF5 | MajoPlafCF | sGMP | BMAF | SeuilPauvrete ~ SMPT,
        MaxRevRG ~ PlafondSS,
        PlafMajoRevRG ~ cumprod(1 + Prixp),
        MinPR ~ cumprod(MinPRp * (1 + Prixp)),
        MinVieil1 | MinVieil2 ~ lag(cumprod(1 + Prixp)),
        Mincont1 | Mincont2 ~ SMIC, 
        # SalRefARRCO et SalRefAGIRC remplies entièrement par données COR
        # Pour l'instant, un décrochage est maintenu entre valeur d'achat et service.
        # Cette hypothèse n'a pas d'importance tant que les séries sont entièrement renseignées par le COR.
        SalRefAGIRC_ARRCO | SalRefARRCO | SalRefAGIRC ~ cumprod((1+SMPTp)*(1+Prixp)),
        # ValPtAGIRC et ValPtARRCO remplies entièrement par données COR
        ValPtAGIRC | ValPtARRCO | ValPtAGIRC_ARRCO ~ cumprod(
          ifelse(annee %in% c(2024, 2025, 2026), 1 + pmax(Prixp - 0.004, 0), (1 + SMPTp - 0.0116) * (1 + Prixp))
        ),
        MinRevRG | SeuilExoCSG | SeuilExoCSG2 | SeuilTxReduitCSG | SeuilTxReduitCSG2 ~ cumprod(1 + Prixp),
        . ~ 1
      ) %>%
      arrange(annee)
  }
}


#' Outil import_scenario_demo_insee
#' @description Importe les paramètres relatifs aux scénarios de projections démographiques de l'Insee
#' @param envir : environnement de simulation (objet Simul)
#' @param date_fin_simulation : année de fin de simulation (2070 par défaut)
#' @param champ_france : champ à considérer (France entière FE ou métropole FM)
#' @param scenario_fecondite : scénario d'évolution de la fécondité
#' @param scenario_esp_vie : scénario d'évolution des espérances de vie
#' @param scenario_migration : scénario d'évolution des flux migratoires
#' @param annee_projection_pop : année des projection de population Insee (si NULL : les plus récentes)
#' @export
import_scenario_demo_insee <- function(envir, date_fin_simulation = 2070, champ_france = "FE",
                                       scenario_fecondite = "central", scenario_esp_vie = "central",
                                       scenario_migration = "central",
                                       annee_projection_pop = NULL) {
  if (is.null(annee_projection_pop)) {
    i <- 2022
    while ((system.file("extdata", paste0("Param_demo_", i), package = "destinie")) != "") {
      i <- i + 5
    }
    annee_projection_pop <- i - 5
    # Dernieres projection de pop dans le package :  annee_projection_pop
  }

  if (
    scenario_fecondite %in% c("central", "bas", "haut", "travail") != TRUE ||
      scenario_esp_vie %in% c("central", "bas", "haut", "travail") != TRUE ||
      scenario_migration %in% c("central", "bas", "haut", "travail") != TRUE
  ) {
    stop("Pour chaque composante (fecondite, esp_vie, migration), veuillez choisir un scénario démographique parmi : central, bas, haut, travail")
  }
  if (annee_projection_pop %% 5 != 2) {
    stop("Veuillez choisir annee_projection_pop parmi 2017, 2022, 2027...")
  }
  if (champ_france != "FE" && annee_projection_pop != 2017) {
    stop("champ_france = 'FE' uniquement en dehors de annee_projection_pop = 2017")
  }

  message(
    "Scénario démographique (fecondité, espérance de vie, migration) retenu : ",
    scenario_fecondite, scenario_esp_vie, scenario_migration
  )
  message("Import des objets Survie, Qmort, espvie, CiblesDemo, mortalite_diff, mortadiff_dip, EqSante et FinEtudeMoy dans l'environnement")

  message("Année des projections de population : ", annee_projection_pop)

  # choix du scenario demographique pour la fécondité, la migration et la mortalité
  liste_hypo <- list(
    "central" = c("*", "", "", ""),
    "bas" = c("", "*", "", ""),
    "haut" = c("", "", "*", ""),
    "travail" = c("", "", "", "*")
  )

  sc_demo <- data.frame(
    fecondite = liste_hypo[[scenario_fecondite]],
    esp_vie = liste_hypo[[scenario_esp_vie]],
    migration = liste_hypo[[scenario_migration]]
  )

  row.names(sc_demo) <- c("central", "bas", "haut", "ue27")
  sc_demo <- t(sc_demo)



  # si le scénario n'est pas central, il faut modifier le fichier excel (procédure faite dans un fichier temporaire)
  if ((scenario_fecondite != "central" | scenario_esp_vie != "central" | scenario_migration != "central")) {
    message("Le scénario diffère des hypothèses centrales : le fichier source des cibles démographiques est modifié dans un fichier temporaire")
    # Import et mise à jour de ciblesdemographie selon les 3 scénarios considérés
    paramDemo <- xlsx::loadWorkbook(
      system.file("extdata", paste0("Param_demo_", annee_projection_pop, "/ciblesDemographie_", champ_france, "_dev.xls"),
        package = "destinie", mustWork = TRUE
      )
    )

    # on sauvegarde une copie temporaire de paramDemo, sur laquelle on fait les modifications
    xlsx::saveWorkbook(paramDemo, file = paste0(tempdir(), "\\paramDemo.xlsx"))
    paramDemo <- xlsx::loadWorkbook(paste0(tempdir(), "\\paramDemo.xlsx"))
    gc()

    feuilles <- xlsx::getSheets(paramDemo)

    xlsx::addDataFrame(
      sc_demo,
      sheet = feuilles$`scenario`,
      row.names = TRUE, col.names = TRUE, startRow = 1, startColumn = 1
    )

    # 1) m a j des cibles de fecondite
    colnames(sc_demo)[4] <- "ue27"
    hyp_fecondite <- readxl::read_excel(
      system.file("extdata", paste0("Param_demo_", annee_projection_pop, "/hyp_fecondite.xls"),
        package = "destinie", mustWork = TRUE
      ),
      sheet = scenario_fecondite
    )

    xlsx::addDataFrame(
      data.frame(hyp_fecondite),
      sheet = feuilles$fecondite_source,
      row.names = FALSE, col.names = TRUE, startRow = 1, startColumn = 1
    )

    # 2) m a j des cibles de mortalite (H et F)
    colnames(sc_demo)[4] <- "mortcte"
    hyp_mortaliteH <- readxl::read_excel(
      system.file("extdata", paste0("Param_demo_", annee_projection_pop, "/hyp_mortalite.xls"),
        package = "destinie", mustWork = TRUE
      ),
      sheet = paste0(scenario_esp_vie, "H"),
      skip = 4
    )

    xlsx::addDataFrame(
      data.frame(hyp_mortaliteH),
      sheet = feuilles$mortaliteH_source,
      row.names = FALSE, col.names = TRUE, startRow = 5, startColumn = 1
    )

    hyp_mortaliteF <- readxl::read_excel(
      system.file("extdata", paste0("Param_demo_", annee_projection_pop, "/hyp_mortalite.xls"),
        package = "destinie", mustWork = TRUE
      ),
      sheet = paste0(scenario_esp_vie, "F"),
      skip = 4
    )

    xlsx::addDataFrame(
      data.frame(hyp_mortaliteF),
      sheet = feuilles$mortaliteF_source,
      row.names = FALSE, col.names = TRUE, startRow = 5, startColumn = 1
    )

    # m a j complémentaires (si nécessaires)

    # espérance de vie  (rq : à reproduire pour annee_projection_pop = 2022)
    espvieH <- readxl::read_excel(
      system.file("extdata", paste0("Param_demo_", annee_projection_pop, "/espvie_ageannee_", champ_france, ".xls"),
        package = "destinie", mustWork = TRUE
      ),
      sheet = paste0(scenario_esp_vie, "_H")
    )

    xlsx::addDataFrame(data.frame(espvieH), sheet = feuilles$espvieH_source, row.names = FALSE, col.names = TRUE, startRow = 1, startColumn = 1)


    espvieF <- readxl::read_excel(
      system.file("extdata", paste0("Param_demo_", annee_projection_pop, "/espvie_ageannee_", champ_france, ".xls"),
        package = "destinie", mustWork = TRUE
      ),
      sheet = paste0(scenario_esp_vie, "_F")
    )

    xlsx::addDataFrame(
      data.frame(espvieF),
      sheet = feuilles$espvieF_source,
      row.names = FALSE, col.names = TRUE, startRow = 1, startColumn = 1
    )


    # 3) m a j des cibles de migration (H et F)
    hyp_soldemigH <- readxl::read_excel(
      system.file("extdata", paste0("Param_demo_", annee_projection_pop, "/hyp_migrations.xls"),
        package = "destinie", mustWork = TRUE
      ),
      sheet = paste0(scenario_migration, "H")
    )

    xlsx::addDataFrame(
      data.frame(hyp_soldemigH),
      sheet = feuilles$soldemigH_source,
      row.names = FALSE, col.names = TRUE, startRow = 1, startColumn = 1
    )


    hyp_soldemigF <- readxl::read_excel(
      system.file("extdata", paste0("Param_demo_", annee_projection_pop, "/hyp_migrations.xls"),
        package = "destinie", mustWork = TRUE
      ),
      sheet = paste0(scenario_migration, "F")
    )

    xlsx::addDataFrame(data.frame(hyp_soldemigF), sheet = feuilles$soldemigF_source, row.names = FALSE, col.names = TRUE, startRow = 1, startColumn = 1)


    # recalcul du tableur, et sauvegarde du fichier .xls en fichier temporaire

    paramDemo$setForceFormulaRecalculation(TRUE)
    paramDemo$getCreationHelper()$createFormulaEvaluator()$evaluateAll()
    xlsx::saveWorkbook(paramDemo, file = paste0(tempdir(), "\\paramDemo.xlsx"))
    gc()


    # ouverture du .xls des parmètres démo à jour (en fichier temp) et ajout des objets dans l'environnement

    morta <- xlsx::loadWorkbook(paste0(tempdir(), "\\paramDemo.xlsx"))
  } else {
    message("Le scénario central n'a pas nécessité de modifier le fichier source des cibles démographiques")
    # si scénario central : il suffit d'ouvrir le fichier des cibles démo présent dans le package
    morta <- xlsx::loadWorkbook(
      system.file("extdata", paste0("Param_demo_", annee_projection_pop, "/ciblesDemographie_", champ_france, "_dev.xls"),
        package = "destinie", mustWork = TRUE
      )
    )
  }

  # vérification nécessaire : il faut s'assurer que la feuille scenario soit cohérente avec le scénario démo :
  scenario <- xlsx::readColumns(morta$getSheet("scenario"),
    startRow = 1, endRow = 4,
    startColumn = 2, endColumn = 5,
    colClasses = "character"
  )
  rownames(sc_demo) <- NULL

  if (all.equal(as.matrix(scenario), as.matrix(sc_demo)) == FALSE) {
    stop("La feuille scenario n'est pas cohérente avec le hypothèses démographiques renseignées")
  }

  # Survie (H/F):
  envir$Survie_H <- as.matrix(xlsx::readColumns(morta$getSheet("SH"), 2, 122, 2, colClasses = "numeric"))
  envir$Survie_F <- as.matrix(xlsx::readColumns(morta$getSheet("SF"), 2, 122, 2, colClasses = "numeric"))
  # Quotients mortalité (H/F):
  envir$Qmort_H <- as.matrix(xlsx::readColumns(morta$getSheet("QH"), 2, 122, 2, colClasses = "numeric"))
  envir$Qmort_F <- as.matrix(xlsx::readColumns(morta$getSheet("QF"), 2, 122, 2, colClasses = "numeric"))
  # Espérance de vie (H/F):
  envir$espvie_H <- as.matrix(xlsx::readColumns(morta$getSheet("espvieH"), 2, 122, 2, colClasses = "numeric"))
  envir$espvie_F <- as.matrix(xlsx::readColumns(morta$getSheet("espvieF"), 2, 122, 2, colClasses = "numeric"))

  ## cibles de fécondité et migration

  NA0 <- function(x) {
    return(ifelse(is.na(x), 0, x))
  }

  # CiblesDemo: data.frame avec info sur les cibles de naissance et de migration
  envir$CiblesDemo <- xlsx::readColumns(morta$getSheet("CiblesDemo"),
    startRow = 2, startColumn = 1, endColumn = 48, colClasses = "numeric"
  ) %>% mutate(across(everything(), NA0))


  # tables complémetaires, indépendantes du scénario considéré :

  # mortalité différentiée par diplôme (NB : exercice à mettre à jour pour pp 2022)
  envir$mortadiff_dip_F <- xlsx::read.xlsx(
    system.file("extdata", paste0("Param_demo_", annee_projection_pop, "/MORTA_DIP.xls"),
      package = "destinie", mustWork = TRUE
    ),
    sheetName = "morta_dif_F", startRow = 1
  )

  envir$mortadiff_dip_H <- xlsx::read.xlsx(
    system.file("extdata", paste0("Param_demo_", annee_projection_pop, "/MORTA_DIP.xls"),
      package = "destinie", mustWork = TRUE
    ),
    sheetName = "morta_dif_H", startRow = 1
  )

  envir$mortalite_diff <- xlsx::read.xlsx(
    system.file("extdata", "PARAM_mortalite_diff.xls",
      package = "destinie", mustWork = TRUE
    ),
    sheetName = "Mortalite_diff", startRow = 2
  )

  # équations de santé
  envir$EqSante <- xlsx::read.xlsx(
    system.file("extdata", "sante.xls",
      package = "destinie", mustWork = TRUE
    ),
    sheetName = "adl"
  )

  # âge moyen de fin d'étude
  envir$FinEtudeMoy <- xlsx::read.xlsx(
    system.file("extdata", "PARAM_etude.xls",
      package = "destinie", mustWork = TRUE
    ),
    sheetName = "TABLEFINDET0", startRow = 2
  )

  gc()
  return(envir)
}
