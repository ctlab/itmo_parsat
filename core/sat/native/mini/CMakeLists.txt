set(MINI_LIB_SRC "utils/Options.cc" "utils/System.cc")
add_library(mini-lib-static STATIC ${MINI_LIB_SRC})

add_subdirectory(minisat)
add_subdirectory(mapleCOMSPS)
