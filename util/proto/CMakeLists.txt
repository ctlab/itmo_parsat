include_directories(${Protobuf_INCLUDE_DIRS})

protobuf_generate_cpp(LOGGING_PROTO_SRCS LOGGING_PROTO_HDRS logging_config.proto)
message(STATUS "Generated: " ${LOGGING_PROTO_SRCS} ", " ${LOGGING_PROTO_HDRS})

set(PROTO_INCLUDE "${CMAKE_CURRENT_BINARY_DIR}" PARENT_SCOPE)

add_library(util-proto-lib-static STATIC ${LOGGING_PROTO_SRCS})
