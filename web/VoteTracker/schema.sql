DROP TABLE IF EXISTS scores;

CREATE TABLE scores (
    name TEXT PRIMARY KEY CHECK (name is null or length(name) > 0),
    score INTEGER NOT NULL CHECK (typeof(score) = 'integer')
);

INSERT INTO scores VALUES ("going", 10);
INSERT INTO scores VALUES ("shitsong", -5);
INSERT INTO scores VALUES ("seed with spaces", 2);
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


/*
            listOfSeeds.push_back(std::tuple<std::string, int>{"covid", 0});
            listOfSeeds.push_back(std::tuple<std::string, int>{"partake", 0});
            listOfSeeds.push_back(std::tuple<std::string, int>{"removal", 0});
            listOfSeeds.push_back(std::tuple<std::string, int>{"going", 0});
            listOfSeeds.push_back(std::tuple<std::string, int>{"caboose", 0});
            listOfSeeds.push_back(std::tuple<std::string, int>{"serbian", 0});
            listOfSeeds.push_back(std::tuple<std::string, int>{"amuser", 0});
            listOfSeeds.push_back(std::tuple<std::string, int>{"majesty", 0});
            listOfSeeds.push_back(std::tuple<std::string, int>{"oops", 0});
            listOfSeeds.push_back(std::tuple<std::string, int>{"flaxseed", 0});
            listOfSeeds.push_back(std::tuple<std::string, int>{"reroute", 0});
            */