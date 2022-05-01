#!/usr/bin/env bash
source scripts/base.sh

IPS_NAME="itmo-parsat"
DB_NAME="infra_pg_db"
MOUNT=""

function do_desc() {
    echo "Docker containers manipulation utility."
    echo "Usage: d option* -- native-option*"
}

function get_ips_cid() {
    cid=$(docker container ls | grep "$IPS_NAME" | awk '{print $1}')
}

function get_pg_cid() {
    cid=$(docker container ls | grep "$DB_NAME" | awk '{print $1}')
}

function do_run_pg() {
    get_pg_cid
    if [[ -n "$cid" ]]; then
        echo "DB container is already running: $cid"
    else
        cid=$(
            docker run --name "$DB_NAME" \
                -p 5432:5432 \
                -e POSTGRES_USER=ips \
                -e POSTGRES_PASSWORD=${POSTGRES_PASSWORD} \
                -e POSTGRES_DB=infra_db \
                -e PGDATA=/var/lib/postgresql/data/pgdata \
                -d \
                -v "$(pwd)/../ips-artifacts/pg_db":/var/lib/postgresql/data \
                -v "$(pwd)/scripts/docker/docker-entrypoint-initdb.d":/docker-entrypoint-initdb.d \
                postgres:13.3 \
        )
        echo "Started DB container: $cid."
    fi
}

function do_run_all() {
    do_run_pg
    do_run_ips
}

function do_stop_pg() {
    get_pg_cid
    if [[ -n "$cid" ]]; then
        echo "Stopping DB container $cid."
        docker stop "$cid"
        docker rm "$DB_NAME" || true
    else
        echo "DB Container is not running."
    fi
}

function do_stop_ips() {
    get_ips_cid
    if [[ -n "$cid" ]]; then
        echo "Stopping IPS container $cid."
        docker stop "$cid"
    else
        echo "IPS Container is not running."
    fi
}

function do_stop_all() {
    do_stop_ips
    do_stop_pg
}

function do_mount() {
    MOUNT="--mount type=bind,source=$1,target=$1"
}

function do_run_ips() {
    get_ips_cid
    if [[ -n "$cid" ]]; then
        echo "IPS container is already running: $cid."
    else
        cid=$( \
            docker run -dt \
                --cpus=8 \
                --memory=32g \
                --net=host $MOUNT \
                --mount type=bind,source="$(pwd)",target="/home/$(id -un)/itmo-parsat" \
                "$IPS_NAME" \
        )
        echo "Started IPS container: $cid."
    fi
}

function do_build() {
    echo "Stopping IPS container..."
    do_stop_ips
    echo "Building IPS container..."
    docker build -t "$IPS_NAME" --build-arg user=$(id -un) .
}

function do_attach_ips() {
    get_ips_cid
    if [[ -n "$cid" ]]; then
        echo "Attaching to IPS container $cid..."
        docker exec -it "$cid" /bin/bash
    else
        echo "Container is not running."
    fi
}

function do_attach_pg() {
    get_pg_cid
    if [[ -n "$cid" ]]; then
        echo "Attaching to PG container $cid..."
        docker exec -it "$cid" /bin/bash
    else
        echo "Container is not running."
    fi
}

function show_status() {
    local cid="$1"
    local name="$2"
    if [[ -n "$cid" ]]; then
        echo "$name container is running: $cid"
    else
        echo "$name container is not running."
    fi
}

function do_status() {
    get_pg_cid
    show_status "$cid" "PG"
    get_ips_cid
    show_status "$cid" "IPS"
}

add_option "-s|--stop-all" "  Stop all running containers" do_stop_all   0
add_option "-a|--attach-ips" "Attach to ips container"     do_attach_ips 0
add_option "-b|--build" "     Rebuild IPS container"       do_build      0
add_option "-r|--run-ips" "   Run IPS container"           do_run_ips    0
add_option "--stop-ips" "     Stop IPS container"          do_stop_ips   0
add_option "--stop-pg" "      Stop PG container"           do_stop_pg    0
add_option "--attach-pg" "    Attach to PG container"      do_attach_pg  0
add_option "--run-pg" "       Run PG container"            do_run_pg     0
add_option "--run" "          Run all containers"          do_run_all    0
add_option "--status" "       Display containers' status"  do_status     0

function main() {
    parse_options "$@"
}

_main "$@"
