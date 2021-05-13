import sqlite3
import click

from flask import  g
from flask.cli import with_appcontext

def get_db():
    if 'db' not in g:
        g.db = sqlite3.connect(
            'scores.db',
            detect_types=sqlite3.PARSE_DECLTYPES
        )
        g.db.row_factory = sqlite3.Row
    
    return g.db

def close_db():
    db = g.pop('db', None)

    if db is not None:
        db.close()

def init_db():
    db = get_db()

    with open('schema.sql') as f:
        db.executescript(f.read())

@click.command('init-db')
@with_appcontext

def init_db_command():
    init_db()
    click.echo('Initialised the DB')

def init_app(app):
    app.cli.add_command(init_db_command)