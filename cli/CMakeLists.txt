add_executable(infra_bin infra/main.cpp ${UTIL_SRC} ${INFRA_SRC})
add_executable(solve_bin solve/main.cpp ${UTIL_SRC} ${CORE_SRC})

include_directories(${CMAKE_BINARY_DIR})
include_directories(../core/sat/native/minisat)

target_link_libraries(infra_bin ${INFRA_LIBS} ${UTIL_LIBS})
target_link_libraries(solve_bin ${CORE_LIBS} ${UTIL_LIBS})
