include_directories(${Protobuf_INCLUDE_DIRS})

protobuf_generate_cpp(SOLVE_PROTO_SRC SOLVE_PROTO_HDR solve_config.proto)
message(STATUS "Generated: " ${SOLVE_PROTO_SRC} ", " ${SOLVE_PROTO_HDR})

add_library(core-proto-lib-static STATIC ${SOLVE_PROTO_SRC})