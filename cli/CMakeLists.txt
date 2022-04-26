add_executable(
        infra_bin infra/main.cpp
        ${UTIL_SRC}
        ${INFRA_SRC}
)
target_link_libraries(infra_bin ${UTIL_LIBS} ${INFRA_LIBS})

add_executable(
        solve_bin solve/main.cpp
        ${UTIL_SRC}
        ${CORE_SRC}
)
target_link_libraries(solve_bin ${UTIL_LIBS} ${CORE_LIBS})

add_executable(
        verify_bin verify/main.cpp
        ${UTIL_SRC}
        ${CORE_SRC}
)
target_link_libraries(verify_bin ${UTIL_LIBS} ${CORE_LIBS})
