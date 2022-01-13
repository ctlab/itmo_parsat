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
