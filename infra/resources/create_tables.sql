CREATE TABLE IF NOT EXISTS Launches (
    launch_id SERIAL PRIMARY KEY,
    input_path VARCHAR(512) NOT NULL,
    config_path VARCHAR(512) NOT NULL,
    log_path VARCHAR(512) NOT NULL,
    commit_hash VARCHAR(128),
    result VARCHAR(16),
    started_at TIMESTAMP,
    finished_at TIMESTAMP,
    description VARCHAR(512),
    config TEXT
);

CREATE VIEW InfoAll AS SELECT input_path, config_path, log_path, result, finished_at - started_at as dur from Launches;

CREATE VIEW InfoPassed AS SELECT * from InfoAll where result = 'PASSED';

CREATE VIEW InfoFailed AS SELECT * from InfoAll where result = 'FAILED';
