file(GLOB LINGELING_SRC "*.h" "*.c")
file(GLOB LINGELING_MAIN "*lglmain.c")
list(REMOVE_ITEM LINGELING_SRC "${LINGELING_MAIN}")

include_directories(.)
add_compile_definitions(NDBLSCR NLGLPICOSAT NDEBUG NLGLDRUPLIG NLGLYALSAT NLGLOG NCHKSOL)

add_library(lingeling-lib-static STATIC ${LINGELING_SRC})