CREATE TABLE IF NOT EXISTS Launches (
    launch_id SERIAL PRIMARY KEY,
    test_group VARCHAR(32) NOT NULL,
    input_name VARCHAR(512) NOT NULL,
    config_name VARCHAR(512) NOT NULL,
    log_path VARCHAR(512) NOT NULL,
    branch VARCHAR(64) NOT NULL,
    commit_hash VARCHAR(128),
    result VARCHAR(16),
    started_at TIMESTAMP,
    finished_at TIMESTAMP,
    description VARCHAR(512),
    config TEXT
);

CREATE VIEW InfoAll AS SELECT test_group, input_name, config_name, log_path, result, finished_at - started_at as dur from Launches;

CREATE VIEW InfoPassed AS SELECT * from InfoAll where result = 'PASSED';

CREATE VIEW InfoFailed AS SELECT * from InfoAll where result = 'FAILED';
