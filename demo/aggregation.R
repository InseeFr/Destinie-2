library(openxlsx)
library(plyr)
library(dplyr)
library(stringr)


file = '/home/thomas/repos/Destinie-2/cas-types/salarie_smpt_g80_2_/salarie_smpt_g80_2_.xlsx'
file = "/home/thomas/repos/Destinie-2/cas-types/salarie_smpt_g80_2_/v3/priv80smptadj.xlsx"
file = "/home/thomas/repos/Destinie-2/cor2 CSG 9.1/fulltest.xlsx"
file = "/home/thomas/repos/Destinie-2/cas-types/salarie_smpt_g80_2_/salarie_smpt_g80_2_.xlsx"
#file = "/home/thomas/repos/Destinie-2/cas-types/salarie_smpt_g90_2_/salarie_smpt_g90_2_.xlsx"
prefix = str_sub(basename(file),0,-6)

actual_value_field='pension_neut_m'
expected_values = read.xlsx(file, 'expected') %>% select(source, expected=value)

folder = dirname(file)
files = dir(folder)

result_files = files[str_detect(files, paste0("^", prefix, ".*.results."))]

single=T

get_data <- function(name, field) {
  df = read.xlsx(paste(folder, name, sep='/'), field)
  if(single) {
    df = df %>% group_by(Id) %>% mutate(debut=min(annee)) %>% filter(annee == debut) %>% select(-debut)
  }
  df$source = str_sub(name, str_length(prefix) + 2, -6 - 8)
  df$actual = df[[actual_value_field]] 
  df <- df %>% left_join(expected_values, by="source")
  df
}

results = lapply(result_files, FUN=get_data, 'retraites')
summary_path = paste(folder, paste0(prefix, '.summary.xlsx'), sep='/')
write.xlsx(rbind.fill(results), summary_path, sheetName='summary')
