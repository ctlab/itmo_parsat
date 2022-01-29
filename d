#!/bin/bash
set -e

function get_ips_cid() {
    cid=$(docker container ls | grep itmo-parsat | awk '{print $1}')
}

function get_pg_cid() {
    cid=$(docker container ls | grep infra_pg_db | awk '{print $1}')
}

function run_pg() {
    get_pg_cid
    if [[ -n "$cid" ]]; then
        echo "DB container is already running."
    else
        cid=$(
            docker run --name infra_pg_db \
                -p 5432:5432 \
                -e POSTGRES_USER=ips \
                -e POSTGRES_PASSWORD=ips \
                -e POSTGRES_DB=infra_db \
                -e PGDATA=/var/lib/postgresql/data/pgdata \
                -d \
                -v "$(pwd)/../ips-artifacts/pg_db":/var/lib/postgresql/data \
                -v "$(pwd)/docker/docker-entrypoint-initdb.d":/docker-entrypoint-initdb.d \
                postgres:13.3 \
        )
        echo "Started DB container: $cid."
    fi
}

function stop_pg {
    get_pg_cid
    if [[ -n "$cid" ]]; then
        echo "Stopping DB container $cid."
        docker stop "$cid"
    else
        echo "DB Container is not running."
    fi
    docker rm infra_pg_db || true
}

function stop_ips {
    get_ips_cid
    if [[ -n "$cid" ]]; then
        echo "Stopping IPS container $cid."
        docker stop "$cid"
    else
        echo "IPS Container is not running."
    fi
}

function run_ips() {
    get_ips_cid
    if [[ -n "$cid" ]]; then
        echo "IPS container is already running: $cid."
    else
        cid=$( \
            docker run -dt \
                --net=host \
                --mount type=bind,source="$(pwd)",target="/home/$(id -un)/itmo-parsat" \
                --mount type=bind,source="$(pwd)/../ips-artifacts",target="/home/$(id -un)/ips-artifacts" \
                itmo-parsat \
        )
        echo "Started IPS container: $cid."
    fi
}

function attach() {
    get_ips_cid
    if [[ -n "$cid" ]]; then
        echo "Attaching to IPS container $cid..."
        docker exec -it "$cid" /bin/bash
    else
        echo "Container is not running."
    fi
}

function process_arg() {
    local arg="$1"
    for (( i = 1; i < "${#arg}"; i++ )); do
        local par="${arg:$i:1}"
        case "$par" in
            r) {
                run_pg
                run_ips
            } ;;
            s) {
                stop_pg
                stop_ips
            } ;;
            a) {
                attach
            } ;;
            b) {
                echo "Stopping IPS container..."
                stop_ips
                echo "Stopping DB container..."
                stop_pg
                echo "Building IPS container..."
                docker build -t itmo-parsat \
                    --build-arg user=$(id -un)  \
                    --build-arg uid=$(id -u)    \
                    --build-arg group=$(id -gn) \
                    --build-arg gid=$(id -g) .
            } ;;
            *) {
                echo "Unrecognized argument: $par"
                exit 1
            } ;;
        esac
    done
}

function main() {
    ln -s docker/ips.Dockerfile Dockerfile || true
    if [[ -n "$1" ]]; then
        while [[ -n "$1" ]]; do
            case "$1" in
                -*) {
                    process_arg "$1"
                } ;;
                *) {
                    echo "Unrecognized argument: $1"
                    exit 1
                } ;;
            esac
            shift
        done
    else
        attach
    fi
}

main "$@"
