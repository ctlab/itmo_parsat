add_subdirectory(proto)

file(
        GLOB UTIL_SRC
        "*.cpp"
)

set(UTIL_SRC ${UTIL_SRC} PARENT_SCOPE)
set(UTIL_LIBS util-proto-lib-static PARENT_SCOPE)
