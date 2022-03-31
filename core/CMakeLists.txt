file(GLOB _DOMAIN_SRC "types.h" "domain/*" "domain/assignment/*")
file(GLOB _EVOL_SRC
        "evol/algorithm/*" "evol/instance/*"
        "evol/limit/*" "evol/method/cross/*"
        "evol/method/mutate/*" "evol/method/select/*" "evol/method/preprocess/*"
        )
file(GLOB _SOLVE_SRC "solve/*" "solve/rbs/*" "solve/reduce/*")
file(GLOB _PROP_SRC "sat/prop/*")
file(GLOB _SOLVER_SRC "sat/solver/*" "sat/solver/sequential/*" "sat/solver/service/*" "sat/sharing/*" "sat/SimpBase.*" "sat/Problem.*")

include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(proto)
add_subdirectory(sat)

set(CORE_LIBS
        ${NATIVE_LIBS_STATIC} core-proto-lib-static ${Boost_LIBRARIES}
        libglog.a libgflags.a libunwind.a liblzma.a libprotobuf.a libz.a)
set(CORE_LIBS ${CORE_LIBS} PARENT_SCOPE)

set(CORE_SRC
        ${_DOMAIN_SRC}
        ${_EVOL_SRC}
        ${_SOLVE_SRC}
        ${_PROP_SRC}
        ${_SOLVER_SRC})
set(CORE_SRC ${CORE_SRC} PARENT_SCOPE)

add_subdirectory(tests)
