
# Pour lancer le server en local

```
virtualenv .venv --python=python3.7
source .venv/bin/activate
pip install --requirement requirements.txt

FLASK_APP=server.py flask run --debugger --reload
```


# Pour la production

