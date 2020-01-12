#!/bin/bash

set -ev

cd `dirname $0`

git pull

R CMD INSTALL . --preclean --debug

cd server
set +v
source .venv/bin/activate
set -v
pip install --requirement requirements.txt

systemctl reload destinie
systemctl status destinie
