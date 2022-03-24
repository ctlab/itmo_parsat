add_subdirectory(native/mini)
add_subdirectory(native/painless)

set(
        NATIVE_LIBS_STATIC
        minisat-lib-static mapleCOMSPS-lib-static painless-lib-static
        PARENT_SCOPE
)