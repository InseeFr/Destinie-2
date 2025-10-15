#' Construction d'une table de revenu et liens familiaux
#'
#' La table est construite en partant de la table `emp` de la simulation.
#' Plusieurs jointures sont réalisées pour construire les éléments d'intérêt :
#' * `salairenet` -> salaires nets.
#' * `ech` -> année de naissance.
#' * `fam` -> liens familiaux : enfants, conjoint, parents.
#' * `retraites` -> pensions, allocations, retraites nettes.
#'
#' @param envir Environnement de simulation.
#'
#' @return Une table contenant pour chaque individu et année :
#'   * age, année de naissance, statut matrimonial.
#'   * les identifiants des enfants, du conjoint, des pères et mères.
#'   * le revenu.
#'   Les lignes sont renseignées pendant la durée de vie de l'individu, bornées
#'   par le début et la fin de la simulation.
#' @export
get_famille_revenu <- function(envir) {
  envir$emp |>
    dplyr::filter(statut != 0L) |>  # On conserve les individus encore présents
    select(Id, age, statut) |>
    left_join(
      envir$salairenet |> select(Id, age, salaires_net),
      by = c("Id", "age")
    ) |>
    dplyr::rename(salaire = salaires_net) |>
    select(Id, age, salaire) |>
    left_join(envir$ech |> select(Id, anaiss), by = "Id") |>
    left_join(
      envir$fam |>
        group_by(Id) |>
        mutate(prem_obs = first(annee)) |>
        filter(annee == prem_obs) |> # Hypothèse implicite : les années sont bien ordonnées par Id.
        select(Id, prem_obs),
      by = "Id"
    ) |> 
    # on ne garde que les les observations à partir de la première observation dans fam
    mutate(annee = age + anaiss) |>
    dplyr::filter(annee >= prem_obs) |>
    select(-prem_obs) |>
    left_join(
      envir$fam |> select(Id, annee, starts_with("enf"), conjoint, matri, pere, mere),
      by = c("Id", "annee")
    ) |>
    mutate(conjoint = if_else(matri %in% c(3L, 4L), 0L, conjoint)) |> # pour les veufs et séparés, le conjoint est retiré
    left_join(
      envir$retraites |> select(Id, annee, retraite_nette, pension, min_vieil),
      by = c("Id", "annee")
    ) |>
    mutate(
      across(c(pension, salaire, retraite_nette, min_vieil), \(x) tidyr::replace_na(x, 0)),
      salaire = if_else(pension > 0, 0, salaire),
      revenu = salaire + retraite_nette + min_vieil
    ) |>
    select(-salaire, -pension, -retraite_nette, -min_vieil)
}

#' Calcul des revenus et des unités de consommation du ménage pour chaque individu x année
#' 
#' Plusieurs hypothèses sont faites pour réaliser ce calcul :
#'   * On considère que les enfants vivent avec leurs parents lorsqu'ils ont
#'   moins de 21 ans, n'ont pas de revenu et ne sont pas mariés.
#'   L'état "concubinage" n'est pas considéré car il semble absent.
#'   * Les parents partagent les UC de leurs enfants, ainsi que leurs UC 
#'   lorsqu'ils vivent ensemble.
#'   * On considère qu'ils vivent ensemble à partir du moment que la variable
#'   "conjoint" est indiquée. Un prétraitement de la table d'entrée permet
#'   d'enlever cette information pour certaines personnes.
#'   * Les enfants à charge récupèrent les revenus et les UC de leurs deux
#'   parents, qu'ils soient séparés ou non.
#'
#' @param famille_revenu Une table contenant les revenus et liens familiaux des
#'   individus chaque année.
#'
#' @return Une table contenant :
#'   * `Id` : identifiant de l'individu.
#'   * `annee` : année en cours.
#'   * `revenu` : revenu de l'individu.
#'   * `nbre_uc` : nombre d'UC du ménage de l'individu (la moitié pour un parent).
#'   * `conjoint` : identifiant du conjoint.
#'   * `est_adulte` : `TRUE` si l'individu n'est pas considéré comme un enfant à charge.
#' @export
get_revenu_uc <- function(famille_revenu) {
  message("Calcul du nombre d'UC pour les adultes")
  revenu_uc_adultes <- famille_revenu |>
    dplyr::filter(!((age <= 21) & (revenu == 0) & (matri != 2))) |>
    select(Id, annee, age, revenu, conjoint, starts_with("enf")) |>
    tidyr::pivot_longer(starts_with("enf"), names_to = "n_enf", values_to = "enf") |>
    left_join(
      famille_revenu |> 
        transmute(
          enf = Id,
          annee = annee,
          age_enf = annee - anaiss,
          salaire_enf = revenu,
          matri_enf = matri
        ),
      by = c("enf", "annee")
    ) |>
    mutate(
      # On verifie pour chaque enfant s'il est à charge ou non c'est à dire :
      # * s'il a moins de 21 ans
      # * s'il n'a pas de revenu
      # * s'il n'est pas marié
      # Dans le cas contraire, on le retire des UC du parent
      enf = case_when(
        !between(age_enf, 0, 21) ~ 0L,
        is.na(salaire_enf) | salaire_enf > 0 ~ 0L,
        is.na(matri_enf) | matri_enf == 2 ~ 0L,
        TRUE ~ enf
      )
    ) |>
    mutate(
      en_couple = ifelse(conjoint > 0, 0.75, 1),
      in_0_14 = between(age_enf, 0, 14),
      in_15_21 = between(age_enf, 15, 21)
    ) |>
    group_by(Id, annee) |>
    summarize(
      revenu = revenu[1],
      nbre_uc = en_couple[1] + sum((0.15 * in_0_14 + 0.25 * in_15_21) * (enf > 0), na.rm = TRUE),
      conjoint = conjoint[1],
      .groups = "drop"
    )

  message("Calcul du nombre d'UC et du revenu pour les jeunes")
  # On calcule la somme des revenus et UC des parents
  revenu_uc_jeunes <- famille_revenu |>
    dplyr::filter(age <= 21 & revenu == 0 & matri != 2) |>
    select(Id, annee, age, pere, mere) |>
    tidyr::pivot_longer(c(pere, mere), names_to = "n_parent", values_to = "parent") |>
    left_join(
      revenu_uc_adultes |> select(parent = Id, annee, revenu, nbre_uc),
      by = c("parent", "annee")
    ) |>
    group_by(Id, annee) |> 
    summarize(
      revenu = sum(revenu, na.rm = TRUE),
      nbre_uc = sum(nbre_uc, na.rm = TRUE),
      .groups = "drop"
    ) |>
    mutate(
      nbre_uc = if_else(nbre_uc == 0, 1, nbre_uc)
    )

  bind_rows(
    revenu_uc_adultes |> mutate(est_adulte = TRUE),
    revenu_uc_jeunes |> mutate(est_adulte = FALSE)
  )
}

#' Calcul des niveaux de vie à partir d'une table des revenus et unités de consommation
#' 
#' Par construction de la table des revenus, le niveau de vie s'obtient en
#' prenant le rapport entre la somme des revenus des parents et la somme des UC
#' des parents.
#'
#' @param revenu_uc Une table contenant les revenus et unités de consommation des 
#'
#' @return Une table contenant :
#'   * `Id` : identifiant de l'individu.
#'   * `annee` : année en cours.
#'   * `niveau_vie` niveau de vie de l'individu.
#' @export
get_niveau_vie_from_revenu_uc <- function(revenu_uc) {
  message("Calcul du niveau de vie pour les adultes")
  niveau_vie_adultes <- revenu_uc |>
    dplyr::filter(est_adulte) |>
    select(Id, annee, revenu, nbre_uc, conjoint) |>
    left_join(
      revenu_uc |> select(conjoint = Id, annee, revenu_conjoint = revenu, nbre_uc_conjoint = nbre_uc),
      by = c("conjoint", "annee")
    ) |>
    dplyr::rename(
      revenu_Id = revenu,
      nbre_uc_Id = nbre_uc
    ) |>
    tidyr::pivot_longer(
      c(starts_with("revenu"), starts_with("nbre_uc")),
      names_pattern = "(.*?)_(Id|conjoint)",
      names_to = c(".value", "orig")
    ) |>
    group_by(Id, annee) |>
    summarize(
      niveau_vie = sum(revenu, na.rm = TRUE) / sum(nbre_uc, na.rm = TRUE),
      .groups = "drop"
    )
  
  message("Calcul du niveau de vie pour les jeunes")
  niveau_vie_jeunes <- revenu_uc |>
    dplyr::filter(!est_adulte) |>
    transmute(
      Id = Id,
      annee = annee,
      niveau_vie = revenu / nbre_uc
    )
  
  bind_rows(niveau_vie_adultes, niveau_vie_jeunes)
}

#' Calcul des niveaux de vie des individus par année
#'
#' @param envir Environnement de simulation.
#'
#' @return Une table contenant :
#'   * `Id` : identifiant de l'individu.
#'   * `annee` : année en cours.
#'   * `niveau_vie` niveau de vie de l'individu.
#' @export
get_niveau_vie <- function(envir) {
  envir |>
    get_famille_revenu() |>
    get_revenu_uc() |>
    get_niveau_vie_from_revenu_uc()
}
