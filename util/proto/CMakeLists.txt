include_directories(${Protobuf_INCLUDE_DIRS})

protobuf_generate_cpp(LOGGING_PROTO_SRC LOGGING_PROTO_HDR logging_config.proto)
message(STATUS "Generated: " ${LOGGING_PROTO_SRC} ", " ${LOGGING_PROTO_HDR})

add_library(util-proto-lib-static STATIC ${LOGGING_PROTO_SRC})