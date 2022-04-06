file(GLOB_RECURSE PAINLESS_SRC "painless-src/**/*.h" "painless-src/**/*.cpp")
list(REMOVE_ITEM PAINLESS_SRC "painless-src/painless.cpp")
list(REMOVE_ITEM PAINLESS_SRC "painless-src/painless.h")

add_compile_definitions(__STDC_LIMIT_MACROS __STDC_FORMAT_MACROS)

include_directories(${PROJECT_SOURCE_DIR}/core/sat/native/painless/painless-src)
include_directories(${PROJECT_SOURCE_DIR}/core/sat/native/mapleCOMSPS)
include_directories(${PROJECT_SOURCE_DIR}/core/sat/native/mapleCOMSPS/m4ri-20140914)

add_library(painless-lib-static ${PAINLESS_SRC})
target_link_libraries(
        painless-lib-static
        mapleCOMSPS-lib-static
        z m
)

add_executable(painless-mcomsps ${PAINLESS_SRC} painless.cpp)
target_link_libraries(
        painless-mcomsps
        mapleCOMSPS-lib-static
        pthread z m
)
