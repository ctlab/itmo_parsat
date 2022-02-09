set(PQXX /usr/include/pqxx)
find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)

file(GLOB _INFRA_SRC "domain/*" "testing/*" "tests/*")

add_library(infra STATIC ${_INFRA_SRC} ${CORE_SRC})
target_link_libraries(infra pqxx pq ${CORE_LIBS})
