
import os
from flask import Flask, flash, request, redirect, url_for, send_file
from werkzeug.utils import secure_filename

import datetime
from time import sleep

UPLOAD_FOLDER = '/var/log/destinie/files'
ALLOWED_EXTENSIONS = {'xlsx'}

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER


@app.route('/example.xlsx', methods=['GET'])
def example():
    return send_file('example.xlsx', as_attachment=True)


@app.route('/expert', methods=['GET', 'POST'])
def expert_mode():
    if request.method == 'POST':
        sleep(3)
        # check if the post request has the file part
        if 'file' not in request.files:
            flash('No file part')
            return redirect(request.url)
        file = request.files['file']
        # if user does not select file, browser also
        # submit an empty part without filename
        if file.filename == '':
            flash('No selected file')
            return redirect(request.url)
        if file and allowed_file(file.filename):
            filename = secure_filename(file.filename)
            prefix = str(datetime.datetime.now()).replace(':', '-').replace(' ', '--')
            file_path = os.path.join(app.config['UPLOAD_FOLDER'], '%s-%s' % (prefix, filename))
            file.save(file_path)


            result_path = '%s.results.xlsx' % file_path[:-5]
            
            myCmd = 'Rscript ../demo/simulation.R --file %s' % file_path

            os.system(myCmd)
            return send_file(result_path, as_attachment=True)
    return '''
    <!doctype html>
    <title>Version expert</title>
    <h1>Version expert</h1>
    <p>
        Cette version demande une saisie assez détaillée des informations. Un fichier Excel doit être rempli puis envoyé à partir du formulaire ci-dessous.
        Un exemple de fichier Excel <a href="example.xlsx">est disponible</a>.
    </p>
    <p>
        3 catégories d'informations sont nécessaires&nbs;:
        <ul>
            <li>Les caractéristiques de l'individu et les informations qui le concernent (année de naissance, sexe, etc.)</li>
            <li>Les salaires bruts et les statuts (étudiant, cadre, non-cadre, chomeur, fonctionnaire, contractuel, etc.) tout au long de la vie.</li>
            <li>Les informations sur la situation familiale (mode expert++ à vos risques et périls)</li>
        </ul>
        Afin de bien remplir le fichier Excel, il peut être utile de consulter la
        <a
            target="_blank"
            rel="noopener"
            href="https://github.com/nosretraites/Destinie-2/blob/master/documentation/documentation.pdf"
            >
            documentation de Destinie</a>.
    </p>
    <p>
        Le résultat de la simulation est ausi un fichier Excel avec des données supplémentaires.
        <ul>
            <li>Des informations sur le moment de passage à la retraite (liquidation)</li>
            <li>L'évolution de la retraite</li>
            <li>Le détail des salaires nets (calculés à partir des salaires bruts)</li>
            <li>Le détail des cotisations sociales</li>
        </ul>
    </p>
    <p>Bon jeu</p>

    <form method=post enctype=multipart/form-data>
      <input type=file name=file>
      <input type=submit value=Envoi>
    </form>
    <div><a href="/">Accueil</a></div>
    '''

@app.route('/basic', methods=['GET'])
def basic_mode():
    return '''
    <!doctype html>
    <title>Simuler les retraites</title>
    <h1>Version basique</h1>
    <p>À faire</p>
    <div><a href="/">Accueil</a></div>
    '''

@app.route('/', methods=['GET'])
def home():
    return '''
    <!doctype html>
    <title>Simuler les retraites</title>
    <h1>Simuler les retraites</h1>
    <div><a href="/basic">Version basique</a></div>
    <div><a href="/expert">Version expert</a></div>
    '''
