include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(proto)
add_subdirectory(sat)

file(
        GLOB DOMAIN_SRC
        "domain/*"
)
file(
        GLOB SEARCH_SRC
        "search/*"
)
file(
        GLOB EVOL_SRC
        "evol/limit/*"
        "evol/algorithm/*"
        "evol/instance/*"
        "evol/method/select/*"
        "evol/method/mutate/*"
        "evol/method/cross/*"
        "evol/method/preprocess/*"
)
file(
        GLOB SOLVE_SRC
        "solve/*"
        "solve/rbs/*"
        "solve/reduce/*"
)
file(
        GLOB PROP_SRC
        "sat/prop/*"
)

set(
        CORE_LIBS
        ${NATIVE_LIBS_STATIC}
        ${Boost_LIBRARIES}
        core-proto-lib-static
        libglog.a
        libgflags.a
        libunwind.a
        liblzma.a
        libprotobuf.a
        libz.a
)
set(CORE_LIBS ${CORE_LIBS} PARENT_SCOPE)

set(
        CORE_SRC
        ${DOMAIN_SRC}
        ${SEARCH_SRC}
        ${PROP_SRC}
        ${EVOL_SRC}
        ${SAT_SRC}
        ${SOLVE_SRC}
)
set(CORE_SRC ${CORE_SRC} PARENT_SCOPE)

add_subdirectory(tests)

set(TEST_COMMON ${TEST_COMMON} PARENT_SCOPE)

