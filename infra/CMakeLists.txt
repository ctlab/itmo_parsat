set(PQXX /usr/include/pqxx)
find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)

include_directories(${CMAKE_BINARY_DIR})
file(
        GLOB INFRA_SRC
        "domain/*"
        "fixture/*"
        "tests/*"
        "execution/*"
)
set(INFRA_SRC ${INFRA_SRC} PARENT_SCOPE)

set(
        INFRA_LIBS
        ${CORE_LIBS}
        ${GTEST_LIBRARIES}
        pqxx pq
        PARENT_SCOPE
)
