#!/bin/bash

SUFFIX=.xlsx
SOURCE=$1
BASE=${SOURCE%"$SUFFIX"}

for age in 62 63 64 65 66 67;
do
  for regime in ACTUEL COMM_PM;
  do
    input_path=${BASE}-${age}-${regime}${SUFFIX}
    #echo $i $INPUT
    cp $SOURCE $input_path
    Rscript demo/simulation.R --file $input_path --regime $regime --age-exo $age --library ~/R-tests
    rm $input_path
  done
done
