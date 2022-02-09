#!/bin/bash
set -e

ROOT="$PWD"
INFRA_DIR="$ROOT/../ips-artifacts"

export GLOG_minloglevel=0
export GLOG_logtostderr=1

declare -a order
declare -A actions
declare -A descs
declare -A needs_arg

function add_option() {
    PATTERN="$1"
    DESC="$2"
    ACTION="$3"
    NEEDS_ARG="$4"
    order+=($PATTERN)
    actions["$PATTERN"]="$ACTION"
    descs["$PATTERN"]="$DESC"
    needs_arg["$PATTERN"]="$NEEDS_ARG"
}

function do_help() {
    do_desc
    for i in "${!order[@]}"; do
        pattern="${order[$i]}"
        echo "    $pattern        ${descs[$pattern]}"
    done
    exit 1
}

function parse_options() {
    while [[ -n "$1" ]]; do
        if [[ $1 == "--" ]]; then
            shift
            echo "Next command will be executed as native"
            NEXT_NATIVE=1
        fi
        FOUND=0
        for i in "${!order[@]}"; do
            pattern="${order[$i]}"
            if [[ "$1" =~ ^$pattern$ ]]; then
                FOUND=1
                ACTION="${actions[$pattern]}"
                NEEDS_ARG="${needs_arg[$pattern]}"
                DESC="${descs[$pattern]}"
                echo "Found: '$pattern' $DESC"
                shift
                if [[ $NEXT_NATIVE -eq 1 ]]; then
                    $ACTION $@
                    exit 0
                elif [[ $NEEDS_ARG -eq 1 ]]; then
                    $ACTION $1
                    shift
                else
                    $ACTION
                fi
            fi
        done
        if [[ $FOUND -eq 0 ]]; then
            echo "Invalid option: $1"
            exit 1
        fi
    done
}

add_option "-h|--help" "Display help message" do_help 0

function _main() {
    if [[ -z "$1" ]]; then
        do_help
    else
        main "$@"
    fi
}
