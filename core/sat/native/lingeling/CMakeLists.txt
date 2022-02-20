file(GLOB LINGELING_SRC "*.h" "*.c")
list(REMOVE_ITEM LINGELING_SRC "lglmain.c")

include_directories(.)
add_compile_definitions(NDBLSCR NLGLPICOSAT NDEBUG NLGLDRUPLIG NLGLYALSAT NLGLOG NCHKSOL)

add_library(lingeling-lib-static STATIC ${LINGELING_SRC})