#!/bin/bash

set -ev

cd `dirname $0`

git pull

R CMD INSTALL . --preclean --debug

cd server
source .venv/bin/activate
pip install --requirement requirements.txt

systemctl reload destinie
systemctl status destinie
