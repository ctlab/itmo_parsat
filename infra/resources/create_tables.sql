CREATE TABLE IF NOT EXISTS Launches (
    launch_id SERIAL PRIMARY KEY,
    input_path VARCHAR(512) NOT NULL,
    config_path VARCHAR(512) NOT NULL,
    log_path VARCHAR(512) NOT NULL,
    backdoor BOOLEAN NOT NULL,
    vcs_tag VARCHAR(128),
    result VARCHAR(16),
    finished_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
