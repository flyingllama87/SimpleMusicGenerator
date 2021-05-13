from flask import Flask
from flask_jsonrpc import JSONRPC
import os
from traceback import print_exc
from pprint import pprint
import json

from VoteTracker import db

app = Flask(__name__)

jsonrpc = JSONRPC(app, '/api')

db.init_app(app)

@jsonrpc.method('app.index')
def index() -> str:
    return u'Welcome to VoteTracker!'

@jsonrpc.method('app.UpVote')
def UpVote(name:str="seed") -> str:
    try:
        db_connection = db.get_db()
        previous_score = db_connection.execute('SELECT * FROM scores WHERE name = ?', (name,)).fetchone()
        if previous_score and previous_score['score']:
            new_score = previous_score['score'] + 1
            db_connection.execute('UPDATE scores SET score = ? WHERE name = ?', (new_score, name))
            db_connection.commit()
            print(f"Score updated for seed \"{name}\" to \"{new_score}\" by client")
            return "Score updated"
        else:
            db_connection.execute('INSERT INTO scores (name, score) VALUES (?, ?)',
            (name, 1))
            db_connection.commit()
            print(f"Score added by client for seed \"{name}\".")
            return "Score added"
    except:
        print_exc()
        raise ValueError
    finally:
        db.close_db()

@jsonrpc.method('app.DownVote')
def DownVote(name:str="seed") -> str:
    try:
        db_connection = db.get_db()
        previous_score = db_connection.execute('SELECT * FROM scores WHERE name = ?', (name,)).fetchone()
        if previous_score and previous_score['score']:
            new_score = previous_score['score'] - 1
            db_connection.execute('UPDATE scores SET score = ? WHERE name = ?', (new_score, name))
            db_connection.commit()
            print(f"Score updated for seed \"{name}\" to \"{new_score}\" by client")
            return "Score updated"
        else:
            db_connection.execute('INSERT INTO scores (name, score) VALUES (?, ?)',
            (name, -1))
            db_connection.commit()
            print(f"Score added by client for seed \"{name}\".")
            return "Score added"
    except:
        print_exc()
        raise ValueError
    finally:
        db.close_db()

@jsonrpc.method('app.GetScores')
def GetScores() -> str:
    try:
        db_connection = db.get_db()
        db_scores = db_connection.execute('SELECT * FROM scores ORDER BY score DESC').fetchall()
        scores = []
        for row in db_scores:
            score = {}
            score['name'] = row['name']
            score['score'] = row['score']
            scores.append(score)
        print("Received request to get scores")
        return json.dumps(scores)
    except:
        print_exc()
        raise ValueError
    finally:
        db.close_db()

if __name__ == "__main__":
    app.run()