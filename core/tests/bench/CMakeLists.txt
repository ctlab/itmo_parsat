set(BENCH_SOURCES
        ${CMAKE_SOURCE_DIR}/core/sat/prop/test/bench_propagation.cpp)
message(STATUS "Found benchmark sources: " ${BENCH_SOURCES})

find_package(benchmark REQUIRED)

add_executable(benchmark main.cpp ${BENCH_SOURCES} ${TEST_COMMON} ${UTIL_SRC} ${CORE_SRC})
target_link_libraries(benchmark benchmark::benchmark ${UTIL_LIBS} ${CORE_LIBS})
