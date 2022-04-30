file(
        GLOB TEST_COMMON
        "common/*"
)

set(TEST_COMMON ${TEST_COMMON} PARENT_SCOPE)

add_subdirectory(bench)
add_subdirectory(unit)
