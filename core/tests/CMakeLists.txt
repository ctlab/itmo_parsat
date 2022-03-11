add_compile_definitions(IPS_PROJECT_ROOT="${PROJECT_SOURCE_DIR}")
file(GLOB TEST_COMMON "common/*")

add_subdirectory(bench)
add_subdirectory(unit)
