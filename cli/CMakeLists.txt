add_executable(infra_bin infra/main.cpp)
add_executable(solve_bin solve/main.cpp ${CORE_SRC})

target_link_libraries(infra_bin infra ${Boost_LIBRARIES} ${GTEST_LIBRARIES} pthread glog::glog)
target_link_libraries(solve_bin ${CORE_LIBS})
