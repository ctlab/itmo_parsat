set(
        UNIT_SOURCES
        ${CMAKE_SOURCE_DIR}/core/evol/algorithm/test/unit_algorithm.cpp
        ${CMAKE_SOURCE_DIR}/core/sat/prop/test/unit_propagation.cpp
        ${CMAKE_SOURCE_DIR}/util/test/unit_random.cpp
        ${CMAKE_SOURCE_DIR}/core/sat/solver/sequential/test/unit_sequential_solver.cpp
        ${CMAKE_SOURCE_DIR}/core/sat/solver/service/test/unit_seq_solver_service.cpp
        ${CMAKE_SOURCE_DIR}/core/search/test/unit_search.cpp
)
message(STATUS "Found unit test sources: " ${UNIT_SOURCES})

add_executable(
        unit_tests main.cpp
        ${UNIT_SOURCES}
        ${TEST_COMMON}
        ${UTIL_SRC}
        ${CORE_SRC}
)
target_link_libraries(
        unit_tests
        ${GTEST_LIBRARIES}
        ${UTIL_LIBS}
        ${CORE_LIBS}
)
