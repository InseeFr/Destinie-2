#!/bin/bash

curl 'http://127.0.0.1:5000/basic' --data 'naissance=1980&debut=22&carriere=SMPT&proportion=1&age=67&modele=ACTUEL' --output /dev/null --fail --silent
