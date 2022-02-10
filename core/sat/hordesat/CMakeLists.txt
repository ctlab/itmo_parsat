file(GLOB HORDESAT_SRC "hordesat-src/sharing/*" "hordesat-src/solvers/*" "hordesat-src/utilities/*")

include_directories(../lingeling)
include_directories(../minisat)

add_compile_definitions(__STDC_LIMIT_MACROS)
add_compile_definitions(__STDC_FORMAT_MACROS)

add_library(hordesat-lib-static STATIC ${HORDESAT_SRC})
target_link_libraries(hordesat-lib-static minisat-lib-static lingeling-lib-static)