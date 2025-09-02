cmake_minimum_required(VERSION 3.17)
project(parallel C)
include(CMakePrintHelpers)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/cmocka")

find_package(CMocka)
if (CMocka_FOUND AND UNIT_TESTING)
    cmake_print_variables(CMOCKA_LIBRARIES)
    cmake_print_variables(CMOCKA_INCLUDE_DIRS)

    include(AddCMockaTest)
    include(AddMockedTest)
    add_subdirectory(test)
    enable_testing()
endif (CMocka_FOUND AND UNIT_TESTING)
