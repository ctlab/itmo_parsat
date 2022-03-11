file(GLOB _BENCH_SRC "*.cpp" "*.h")

find_package(benchmark REQUIRED)

add_executable(core_benchmark ${_BENCH_SRC} ${TEST_COMMON} ${UTIL_SRC} ${CORE_SRC})
target_link_libraries(core_benchmark benchmark::benchmark ${UTIL_LIBS} ${CORE_LIBS})
