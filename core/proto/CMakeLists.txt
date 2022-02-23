include_directories(${Protobuf_INCLUDE_DIRS})

protobuf_generate_cpp(SOLVE_PROTO_SRCS SOLVE_PROTO_HDRS solve_config.proto)
message(STATUS "Generated: " ${SOLVE_PROTO_SRCS} ", " ${SOLVE_PROTO_HDRS})

set(PROTO_INCLUDE "${CMAKE_CURRENT_BINARY_DIR}" PARENT_SCOPE)
add_library(core-proto-lib-static STATIC ${SOLVE_PROTO_SRCS})
