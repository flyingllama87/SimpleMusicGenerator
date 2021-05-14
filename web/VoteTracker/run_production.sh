#! /bin/bash
set -eo pipefail

export FLASK_ENV="production"
export FLASK_APP="VoteTracker"
source ./venv/bin/activate
waitress-serve --call 'VoteTracker:vt_start'