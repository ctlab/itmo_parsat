include_directories(${Protobuf_INCLUDE_DIRS})

protobuf_generate_cpp(LOGGING_PROTO_SRCS LOGGING_PROTO_HDRS logging_config.proto)
message(STATUS "Generated: " ${LOGGING_PROTO_SRCS} ", " ${LOGGING_PROTO_HDRS})
protobuf_generate_cpp(SOLVE_PROTO_SRCS SOLVE_PROTO_HDRS solve_config.proto)
message(STATUS "Generated: " ${SOLVE_PROTO_SRCS} ", " ${SOLVE_PROTO_HDRS})

set(PROTO_INCLUDE "${CMAKE_CURRENT_BINARY_DIR}" PARENT_SCOPE)

add_library(ips-proto STATIC ${LOGGING_PROTO_SRCS} ${SOLVE_PROTO_SRCS})
