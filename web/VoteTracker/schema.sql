DROP TABLE IF EXISTS scores;

CREATE TABLE scores (
    name TEXT PRIMARY KEY CHECK (name is null or length(name) > 0),
    score INTEGER NOT NULL CHECK (typeof(score) = 'integer')
);

INSERT INTO scores VALUES ("going", 10);
INSERT INTO scores VALUES ("shitsong", -5);