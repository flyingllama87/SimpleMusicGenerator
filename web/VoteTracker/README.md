Simple Python/Flask VoteTracking server for Morgan's Music Generator

Needs flask and click installed.

To init-db run:

```
FLASK_APP="VoteTracker:vt_start"
export FLASK_APP
flask init-db
```

Setup python 3 virtual env with:

```
sudo virtualenv venv
. ./venv/bin/activate
pip3 install -r requirements.txt
```

Run with run.sh

