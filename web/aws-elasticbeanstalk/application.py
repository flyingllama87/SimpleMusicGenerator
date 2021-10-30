from flask import Flask, request
from flask_jsonrpc import JSONRPC
import os
from traceback import print_exc
from pprint import pprint
import json

import db

def vt_start():
    application = Flask(__name__)

    def validate_seed(seed: str):
        return seed.replace(" ", "").isalnum()

    db.init_app(application)

    @application.route('/api/check')
    def check() -> str:
        return u'Welcome to VoteTracker!'

    @application.route('/')
    def index() -> str:
        return u'Welcome to VoteTracker!'

    @application.route('/api/UpVote', methods=['POST'])
    def UpVote() -> str:
        try:
            if (validate_seed(request.form['seed']) == False):
                raise Exception("Seed val needs to be alpha numeric or spaces only")
            name = request.form['seed']
            db_connection = db.get_db()
            previous_score = db_connection.execute('SELECT * FROM scores WHERE name = ?', (name,)).fetchone()
            if previous_score and previous_score['score']:
                new_score = previous_score['score'] + 1
                db_connection.execute('UPDATE scores SET score = ? WHERE name = ?', (new_score, name))
                db_connection.commit()
                print(f"Score updated for seed \"{name}\" to \"{new_score}\" by client")
                return f"Score updated for seed \"{name}\""
            else:
                db_connection.execute('INSERT INTO scores (name, score) VALUES (?, ?)',
                (name, 1))
                db_connection.commit()
                print(f"Score added by client for seed \"{name}\".")
                return f"Score added for seed \"{name}\""
        except:
            print_exc()
        finally:
            db.close_db()

    @application.route('/api/DownVote', methods=['POST'])
    def DownVote() -> str:
        try:
            if (validate_seed(request.form['seed']) == False):
                raise Exception("Seed val needs to be alpha numeric or spaces only")
            name = request.form['seed']
            db_connection = db.get_db()
            previous_score = db_connection.execute('SELECT * FROM scores WHERE name = ?', (name,)).fetchone()
            if previous_score and previous_score['score']:
                new_score = previous_score['score'] - 1
                db_connection.execute('UPDATE scores SET score = ? WHERE name = ?', (new_score, name))
                db_connection.commit()
                print(f"Score updated for seed \"{name}\" to \"{new_score}\" by client")
                return f"Score updated for seed \"{name}\""
            else:
                db_connection.execute('INSERT INTO scores (name, score) VALUES (?, ?)',
                (name, -1))
                db_connection.commit()
                print(f"Score added by client for seed \"{name}\".")
                return f"Score added for seed \"{name}\""
        except:
            print_exc()
        finally:
            db.close_db()

    @application.route('/api/GetScores')
    def GetScores() -> str:
        try:
            db_connection = db.get_db()
            db_scores = db_connection.execute('SELECT * FROM scores ORDER BY score DESC').fetchall()
            csv_scores = ""
            # csv_scores = "NAME,SCORE\n"
            scores = []
            for row in db_scores:
                score = {}
                score['name'] = row['name']
                score['score'] = row['score']
                scores.append(score)
                csv_scores = csv_scores + str(row['name']) + "," + str(row['score']) + "\n"
            print("Received request to get scores")
            return csv_scores
        except:
            print_exc()
        finally:
            db.close_db()
    
    return application

if __name__ == "__main__":
    application = vt_start()
    application.run(port=5000,debug=False)
else:
    application = vt_start()
