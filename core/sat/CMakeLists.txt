set(HORDESAT_LIBS
        ${CMAKE_SOURCE_DIR}/core/sat/hordesat/libhordesat.a
        ${CMAKE_SOURCE_DIR}/core/sat/hordesat/lingeling/liblgl.a
        PARENT_SCOPE)

add_subdirectory(minisat)