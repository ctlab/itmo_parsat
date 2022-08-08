#!/usr/bin/env bash
source scripts/base.sh

ROOT="$PWD"
VERBOSE="6"

HOSTNAME="$(hostname -s)"
if [[ "$HOSTNAME" == "sphinx" ]] || [[ "$HOSTNAME" == "orthrus-1" ]] || [[ "$HOSTNAME" == "orthrus-2" ]]; then
  RESOURCES_DIR="/mnt/tank/scratch/ibdzhiblavi/ips"
else
  RESOURCES_DIR="$ROOT/resources"
fi

CNF_PATH="$RESOURCES_DIR/cnf/large/unsat_pancake_vs_selection_7_4-@2.cnf"
CFG_ROOT="$RESOURCES_DIR/config"
LOG_CFG_PATH="$RESOURCES_DIR/config/log.json"

BUILD_DIR="$ROOT/build"
SOLVE_BIN="$BUILD_DIR/cli/solve_bin"
INFRA_BIN="$BUILD_DIR/cli/infra_bin"
VERIFY_BIN="$BUILD_DIR/cli/verify_bin"
SEARCH_BIN="$BUILD_DIR/cli/rb_search_bin"
TEST_BIN="$BUILD_DIR/core/tests/unit/unit_tests"
BENCH_BIN="$BUILD_DIR/core/tests/bench/benchmark"

SLV_CFG="naive.json"

NEXT_NATIVE=0
BUILD_DEBUG=""
RUN_CMD=""
CMAKE_OPTS=""
SEED=

export GLOG_logtostderr=1
export ASAN_OPTIONS=exitcode=1337

function do_doc() {
    rm -rf doc/* || true
    PROJECT_NAME="itmo-parsat" doxygen ./resources/doxygen.conf &> /dev/null
}

function do_input() {
    CNF_PATH="$1"
}

function do_config() {
    SLV_CFG="$1.json"
}

function do_seed() {
    SEED=$1
}


function do_prop_config() {
    PRP_CFG="$1.json"
}


function do_set_build_mode() {
    BUILD_CFG="$1"
}

function do_run_cmd() {
    echo "ARG: $@"
    RUN_CMD="$@"
}

function do_format() {
    find . \( -iname *.h -iname *.hpp -o -iname *.cpp -o -iname *.cc \) \
        -and -not -path './*build*/*' -and -not -path '**/.undodir/*' | xargs clang-format -i
}

function do_chk_format() {
    find . \( -iname *.h -iname *.hpp -o -iname *.cpp -o -iname *.cc \) \
        -and -not -path './*build*/*' -and -not -path '**/.undodir/*' | xargs clang-format --dry-run --Werror
}

function do_clean() {
    rm -rf build/*
}

function do_set_verbose() {
    VERBOSE="$1"
}

function do_psql() {
    psql -d infra_db -U ips -h "$PSQL_HOST"
}

function do_set_pg_host() {
    PSQL_HOST="$1"
}

function do_set_res() {
    RESOURCES_DIR="$1"
}

function do_set_cmake() {
    CMAKE_OPTS="$CMAKE_OPTS -D$1=ON"
}

function do_unset_cmake() {
    CMAKE_OPTS="$CMAKE_OPTS -D$1=OFF"
}

function do_sanitize() {
    do_set_cmake "SANITIZE"
}

function do_t_sanitize() {
    do_set_cmake "THREAD_SANITIZE"
}

function do_pgo_gen() {
    do_set_cmake "PGO_GEN"
    do_unset_cmake "PGO_USE"
}

function do_pgo_use() {
    do_set_cmake "PGO_USE"
    do_unset_cmake "PGO_GEN"
}

function do_build() {
    cd build
    CC=gcc-10 CXX=g++-10 cmake .. \
        -DARCH=$(gcc -march=native -Q --help=target | grep march | head -n 1 | awk '{print $2}') \
        -DCMAKE_BUILD_TYPE="$BUILD_CFG" $CMAKE_OPTS
    CC=gcc-10 CXX=g++-10 make -j $(nproc)
    cd "$ROOT"
}

function do_verify() {
    ./scripts/hooks/pre-commit
}

function do_verify_cert() {
    $RUN_CMD $VERIFY_BIN $@
}

function do_unit() {
    $RUN_CMD $TEST_BIN $@
}

function do_bench() {
    $RUN_CMD $BENCH_BIN $@
}

function do_infra() {
    $RUN_CMD $INFRA_BIN \
        --branch $(git status | grep branch | awk '{print $3}') \
        --commit $(git rev-parse --verify HEAD) \
        --resources_dir $RESOURCES_DIR \
        --working_dir $INFRA_DIR \
        --exec $SOLVE_BIN \
        --pg_host $PSQL_HOST $@
}

function do_solve() {
    if [[ -z "$@" ]]; then
        $RUN_CMD $SOLVE_BIN \
            --model-path proof.txt \
            --print-model \
            --verbose "$VERBOSE" \
            --input "$CNF_PATH" \
            --solve-config "$CFG_ROOT/$SLV_CFG" \
            --log-config "$LOG_CFG_PATH"
    else
        $RUN_CMD $SOLVE_BIN $@
    fi
}

function do_search() {
  if [[ -z "$@" ]]; then
    $RUN_CMD $SEARCH_BIN \
      --verbose "$VERBOSE" \
      --seed $SEED \
      --log-config resources/config/log.json \
      --algorithm-config "$CFG_ROOT/$SLV_CFG" \
      --prop-config "$CFG_ROOT/$PRP_CFG" \
      --stats-path stats.csv \
      --rb-path backdoor.txt \
      --heuristic-path heuristic.txt \
      --input "$CNF_PATH"
  else
      $RUN_CMD $SEARCH_BIN $@
  fi
}

function do_run_pgo() {
    echo "Running PGO benchmarks"
    local inputs_file="./resources/pgo/inputs.txt"
    local configs_file="./resources/pgo/configs.txt"
    while read input; do
        while read config; do
            echo "in: $input, cfg: $config"
            ./ips -v "$VERBOSE" -c "$config" -i "$input" -s || true
        done < "$configs_file"
    done < "$inputs_file"
}

function do_sync() {
    HOST="$1"
    rsync --recursive --delete --whole-file \
        --exclude 'doc/' \
        --exclude '**/*.undodir' \
        --exclude '.idea/' \
        --exclude '.git/' \
        --exclude '*build*/' \
        --exclude '**/*.csv' \
        --exclude '**/*.cnf' \
        ~/itmo-parsat/ $HOST/itmo-parsat
}

function do_singular() {
    if ! [[ -f "ips.sif" ]]; then
        echo "Building singularity container"
        singularity build --fakeroot ips.sif Singularity.def
    fi
    singularity exec ips.sif ./ips -g DEV_FAST -b
}

function do_desc() {
    echo "Running and/or building IPS."
    echo "Usage: ./run.sh option* -- native-option*"
}

add_option "-s|--solve" "    Run cli binary"             do_solve          0
add_option "-f|--find-rb" "  Run RB search"              do_search         1
add_option "-g|--build-cfg" "Set build mode"             do_set_build_mode 1
add_option "-r|--resources" "Set resources directory"    do_set_res        1
add_option "-b|--build" "    Build cli binary"           do_build          0
add_option "-i|--input" "    Input CNF path"             do_input          1
add_option "-c|--config" "   Specify config"             do_config         1
add_option "-p|--prop-config" "Specify prop-config"      do_prop_config    1
add_option "-v|--verbose" "  Set verbosity level [=2]"   do_set_verbose    1
add_option "--seed"          "Random seed"               do_seed           1
add_option "--unit" "        Run unit tests"             do_unit           0
add_option "--benchmark" "   Run benchmarks"             do_bench          0
add_option "--pgo-gen" "     Enable PGO generation"      do_pgo_gen        0
add_option "--pgo-use" "     Enable PGO usage"           do_pgo_use        0
add_option "--add-opt" "     Set cmake option to ON"     do_set_cmake      1
add_option "--rm-opt" "      Set cmake option to OFF"    do_unset_cmake    1
add_option "--format" "      Apply clang-format"         do_format         0
add_option "--check-format" "Check format"               do_chk_format     0
add_option "--verify" "      Run verification"           do_verify         0
add_option "--verify-cert" " Verify SAT certificate"     do_verify_cert    0
add_option "--sanitize" "    Enable common sanitizers"   do_sanitize       0
add_option "--t-sanitize" "  Enable thread sanitizer"    do_t_sanitize     0
add_option "--run-pgo" "     Run PGO warmup binary"      do_run_pgo        0
add_option "--run-cmd" "     Specify run prefix"         do_run_cmd        1
add_option "--run-infra" "   Run integration tests"      do_infra          0
add_option "--build-doc" "   Build documentation"        do_doc            0
add_option "--clean" "       Clean build directory"      do_clean          0
add_option "--psql-host" "   Set DB host"                do_set_pg_host    1
add_option "--psql" "        Connect to DB"              do_psql           0
add_option "--sync" "        Sync project to remote"     do_sync           1
add_option "--build-sing" "  Build singularity"          do_singular       1

function main() {
    parse_options "$@"
}

_main "$@"
