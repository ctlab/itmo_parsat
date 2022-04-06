find_package(benchmark REQUIRED)

set(
        BENCH_SOURCES
        ${CMAKE_SOURCE_DIR}/core/sat/prop/test/bench_propagation.cpp
        ${CMAKE_SOURCE_DIR}/core/evol/algorithm/test/bench_algorithm.cpp
)
message(STATUS "Found benchmarks' sources: " ${BENCH_SOURCES})

add_executable(
        benchmark main.cpp
        ${BENCH_SOURCES}
        ${TEST_COMMON}
        ${UTIL_SRC}
        ${CORE_SRC}
)
target_link_libraries(
        benchmark
        benchmark::benchmark
        ${UTIL_LIBS}
        ${CORE_LIBS}
)
