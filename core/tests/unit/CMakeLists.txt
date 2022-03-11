file(GLOB _UNIT_SRC "*.cpp" "*.h")

add_executable(core_unit_tests ${_UNIT_SRC} ${TEST_COMMON} ${UTIL_SRC} ${CORE_SRC})
target_link_libraries(core_unit_tests ${GTEST_LIBRARIES} ${UTIL_LIBS} ${CORE_LIBS})
