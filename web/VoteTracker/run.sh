#! /bin/bash

FLASK_APP="VoteTracker:vt_start"
export FLASK_APP
FLASK_ENV="production"
export FLASK_ENV
flask run