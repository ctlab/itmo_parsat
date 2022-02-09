file(GLOB _DOMAIN_SRC "domain/*" "domain/assignment/*")
file(GLOB _EVOL_SRC "evol/algorithm/*" "evol/instance/*" "evol/limit/*" "evol/method/cross/*" "evol/method/mutate/*" "evol/method/select/*")
file(GLOB _SOLVE_SRC "solve/*")
file(GLOB _UTIL_SRC "util/*")
file(GLOB _TEST_SRC "tests/*")
file(GLOB _PROP_SRC "sat/prop/*")
file(GLOB _SOLVER_SRC "sat/solver/*" "sat/SimpBase.*")

add_subdirectory(proto)
add_subdirectory(sat)

set(CORE_LIBS
        minisat-lib-static ips-proto glog::glog z
        ${Boost_LIBRARIES} ${MPI_CXX_LIBRARIES} ${Protobuf_LIBRARIES} ${HORDESAT_LIBS})
set(CORE_LIBS ${CORE_LIBS} PARENT_SCOPE)

set(CORE_SRC
        ${_DOMAIN_SRC}
        ${_EVOL_SRC}
        ${_SOLVE_SRC}
        ${_UTIL_SRC}
        ${_PROP_SRC}
        ${_SOLVER_SRC}
        ${LOGGING_PROTO_SRCS}
        ${SOLVE_PROTO_SRCS})
set(CORE_SRC ${CORE_SRC} PARENT_SCOPE)

add_executable(core_unit_tests ${_TEST_SRC} ${CORE_SRC})
target_link_libraries(core_unit_tests ${CORE_LIBS} ${GTEST_LIBRARIES})
