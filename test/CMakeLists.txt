file(GLOB_RECURSE SOURCES_C ./*.c)
list(REMOVE_ITEM SOURCES_C ./main.c)

add_library(utils ${SOURCES_C})
target_link_libraries(utils PUBLIC
        convolution
        options
        "${CMOCKA_LIBRARIES}")
target_include_directories(utils PUBLIC "${CMOCKA_INCLUDE_DIRS}")

add_cmocka_test(test_main
        SOURCES main.c
        LINK_LIBRARIES utils)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h")
