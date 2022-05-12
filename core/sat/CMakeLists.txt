add_subdirectory(native/mini)
add_subdirectory(native/painless)

file(
        GLOB SAT_SRC
        "solver/*"
        "solver/sequential/*"
        "solver/service/*"
        "sharing/*"
        "SimpBase.*"
        "Problem.*"
)
set(SAT_SRC ${SAT_SRC} PARENT_SCOPE)

set(
        NATIVE_LIBS_STATIC
        minisat-lib-static
        mapleCOMSPS-lib-static
        lmaple-lib-static
        painless-lib-static
        PARENT_SCOPE
)
