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

variante_demo_name <- dplyr::tribble(
  ~number, ~fecondite, ~esp_vie,  ~migration, ~name,                            ~full_name,                          ~auto_name,
  "00",	   "central",	 "central",	"central",  "central",	                      "00_central",                        "fec_centrale_ev_centrale_smi_central",
  "11",    "haut",	   "central",	"central",	"fec_haute",                      "11_fec_haute",	                     "fec_haute_ev_centrale_smi_central",
  "12",    "bas",      "central", "central",  "fec_basse",                      "12_fec_basse",                      "fec_basse_ev_centrale_smi_central", 
  "13",    "central",  "haut",    "central",  "ev_haute",                       "13_ev_haute",                       "fec_centrale_ev_haute_smi_central",
  "14",    "central",  "bas",     "central",  "ev_basse",                       "14_ev_basse",                       "fec_centrale_ev_basse_smi_central",
  "15",    "central",  "central", "haut",     "smi_haut",                       "15_smi_haut",                       "fec_centrale_ev_centrale_smi_haut",
  "16",    "central",  "central", "bas",      "smi_bas",                        "16_smi_bas",                        "fec_centrale_ev_centrale_smi_bas",
  "21",    "haut",     "haut",    "haut",     "population_haute",               "21_population_haute",               "fec_haute_ev_haute_smi_haut",
  "22",    "bas",      "bas",     "bas",      "population_basse",               "22_population_basse",               "fec_basse_ev_basse_smi_bas", 
  "23",    "bas",      "haut",    "bas",      "population_agee",                "23_population_agee",                "fec_basse_ev_haute_smi_bas",
  "24",    "haut",     "bas",     "haut",     "population_jeune",               "24_population_jeune",               "fec_haute_ev_basse_smi_haut", 
  "31",    "central",  "haut",    "haut",     "fec_centrale_ev_haute_smi_haut", "31_fec_centrale_ev_haute_smi_haut", "fec_centrale_ev_haute_smi_haut",
  "32",    "central",  "haut",    "bas",      "fec_centrale_ev_haute_smi_bas",  "32_fec_centrale_ev_haute_smi_bas",  "fec_centrale_ev_haute_smi_bas",
  "33",    "central",  "bas",     "haut",     "fec_centrale_ev_basse_smi_haut", "33_fec_centrale_ev_basse_smi_haut", "fec_centrale_ev_basse_smi_haut",
  "34",    "central",  "bas",     "bas",      "fec_centrale_ev_basse_smi_bas",  "34_fec_centrale_ev_basse_smi_bas",  "fec_centrale_ev_basse_smi_bas",
  "35",    "haut",     "central", "haut",     "fec_haute_ev_centrale_smi_haut", "35_fec_haute_ev_centrale_smi_haut", "fec_haute_ev_centrale_smi_haut",
  "36",    "haut",     "central", "bas",      "fec_haute_ev_centrale_smi_bas",  "36_fec_haute_ev_centrale_smi_bas",  "fec_haute_ev_centrale_smi_bas",
  "37",    "haut",     "haut",    "central",  "fec_haute_ev_haute_smi_central", "37_fec_haute_ev_haute_smi_central", "fec_haute_ev_haute_smi_central",
  "38",    "haut",     "haut",    "bas",      "fec_haute_ev_haute_smi_bas",     "38_fec_haute_ev_haute_smi_bas",     "fec_haute_ev_haute_smi_bas",
  "39",    "haut",     "bas",     "central",  "fec_haute_ev_basse_smi_central", "39_fec_haute_ev_basse_smi_central", "fec_haute_ev_basse_smi_central",
  "40",    "haut",     "bas",     "bas",      "fec_haute_ev_basse_smi_bas",     "40_fec_haute_ev_basse_smi_bas",     "fec_haute_ev_basse_smi_bas",
  "41",    "bas",      "central", "haut",     "fec_basse_ev_centrale_smi_haut", "41_fec_basse_ev_centrale_smi_haut", "fec_basse_ev_centrale_smi_haut",
  "42",    "bas",      "central", "bas",      "fec_basse_ev_centrale_smi_bas",  "42_fec_basse_ev_centrale_smi_bas",  "fec_basse_ev_centrale_smi_bas",
  "43",    "bas",      "haut",    "central",  "fec_basse_ev_haute_smi_central", "43_fec_basse_ev_haute_smi_central", "fec_basse_ev_haute_smi_central",
  "44",    "bas",      "haut",    "haut",     "fec_basse_ev_haute_smi_haut",    "44_fec_basse_ev_haute_smi_haut",    "fec_basse_ev_haute_smi_haut",
  "45",    "bas",      "bas",     "central",  "fec_basse_ev_basse_smi_central", "45_fec_basse_ev_basse_smi_central", "fec_basse_ev_basse_smi_central",
  "46",    "bas",      "bas",     "haut",     "fec_basse_ev_basse_smi_haut",    "46_fec_basse_ev_basse_smi_haut",    "fec_basse_ev_basse_smi_haut", 
  "51",    "travail",  "central", "central",  "fec_tres_basse",                 "51_fec_tres_basse",                 "fec_travail_ev_centrale_smi_central",
  "52",    "central",  "travail", "central",  "ev_constante",                   "52_ev_constante",                   "fec_centrale_ev_travail_smi_central",
  "53",    "central",  "central", "travail",  "smi_nul",                        "53_smi_nul",                        "fec_centrale_ev_centrale_smi_travail"
)

readr::write_csv(variante_demo_name, "data-raw/variante_demo_name.csv")
usethis::use_data(variante_demo_name, overwrite = TRUE)
