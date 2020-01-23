library(openxlsx)
library(tidyr)

default.ech = c(
  sexe = 1,
  findet = 0,
  typeFP = 20,
  anaiss = 1980,
  neFrance = 1,
  k = 0,
  taux_prim = 0,
  moisnaiss = 1,
  emigrant = 0,
  tresdip = 0,
  peudip = 0,
  dipl = 0
)

default.fam = c(
  pere = 0,
  mere = 0,
  enf1 = 0,
  enf2 = 0,
  enf3 = 0,
  enf4 = 0,
  enf5 = 0,
  enf6 = 0,
  matri = 1,
  annee = 2019,
  conjoint = 0
)

etudiant=63
non_cadre=1

id='PlafondSS'
data_path = 'demo/carrieres.xlsx'

# COR2
#debuts = read.xlsx(data_path, 'debut') %>% select(generation, debut=!!id)
#carriere = read.xlsx(data_path, 'serie') %>% select(age, proportion=!!id)
#base = 'SMPT'

debuts = read.xlsx(data_path, 'debut') %>% select(generation) %>% mutate(debut=22)
carriere = read.xlsx(data_path, 'serie') %>% select(age) %>% mutate(proportion=0.75)
baseName = 'PlafondSS'

ids = debuts %>% mutate(Id=row_number())
ech = ids %>% mutate(!!!default.ech) %>%
  mutate(anaiss = generation) %>% select(-generation,-debut)
head(ech)

fam = ids %>% select(Id) %>% mutate(!!!default.fam)
head(fam)

base = demoSimulation$macro %>% mutate(base=demoSimulation$macro[[baseName]]) %>% select(annee, base)

# statut=c(rep(etudiant,debut), rep(non_cadre,age_mort-debut))
age_mort=80
debut=22
ages = data.frame(age=0:(age_mort-1))
emp_a = crossing(ids, ages)
emp_b = emp_a %>% mutate(
  debut_i=floor(debut),
  annee=generation+age
)
emp_c = emp_b %>% left_join(carriere, by="age") %>%
  mutate(proportion=if_else(is.na(proportion), 0, proportion)) %>%
  left_join(base, by='annee') %>% mutate(salaire=proportion*base)
emp = emp_c %>% mutate(
  salaire=if_else((age<debut_i), 0, salaire),
  statut=if_else((age<debut_i), etudiant, non_cadre)
) %>% select(Id, age, statut, salaire)

## Create a new workbook
wb <- createWorkbook("fullhouse")
e=environment()
for (field in c("ech", "emp", "fam")){
  addWorksheet(wb, field)
  writeData(wb, field, e[[field]])
}
## Save workbook
outputfile = 'fulltest.xlsx' #str_c(str_sub(sourcepath,end=-6), 'results.xlsx', sep=".")
saveWorkbook(wb, outputfile, overwrite = TRUE)