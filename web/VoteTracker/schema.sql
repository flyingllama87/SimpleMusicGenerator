DROP TABLE IF EXISTS scores;

CREATE TABLE scores (
    name TEXT PRIMARY KEY CHECK (name is null or length(name) > 0),
    score INTEGER NOT NULL CHECK (typeof(score) = 'integer')
);

INSERT INTO scores VALUES ("going", 10);
INSERT INTO scores VALUES ("covid", 5);
INSERT INTO scores VALUES ("partake", 2);
INSERT INTO scores VALUES ("removal", 2);
INSERT INTO scores VALUES ("caboose", 6);
INSERT INTO scores VALUES ("serbian", 3);
INSERT INTO scores VALUES ("amuser", 3);
INSERT INTO scores VALUES ("majesty", 2);
INSERT INTO scores VALUES ("oops", 2);
INSERT INTO scores VALUES ("flaxseed", 5);
INSERT INTO scores VALUES ("reroute", 10);
INSERT INTO scores VALUES ("moistbox", 1);
INSERT INTO scores VALUES ("slip", 3);
INSERT INTO scores VALUES ("gurgling", 3);
INSERT INTO scores VALUES ("street", 3);
INSERT INTO scores VALUES ("MMMMMMMMMMMMMMM", -1);