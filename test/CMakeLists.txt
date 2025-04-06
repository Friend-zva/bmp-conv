file(GLOB_RECURSE C_SOURCES *.c)
list(REMOVE_ITEM C_SOURCES main.c)

add_library(utils ${C_SOURCES})
target_link_libraries(utils PUBLIC
        convolution
        options
        "${CMOCKA_LIBRARIES}")
target_include_directories(utils PUBLIC "${CMOCKA_INCLUDE_DIRS}")

add_cmocka_test(test_main
        SOURCES main.c
        LINK_LIBRARIES utils)
target_include_directories(test_main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h")
