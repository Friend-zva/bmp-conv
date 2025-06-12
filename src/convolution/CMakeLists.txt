file(GLOB_RECURSE C_SOURCES ./*.c)

add_library(convolution ${C_SOURCES})
target_link_libraries(convolution
    PRIVATE "-framework OpenCL"
    PUBLIC cbmp)
target_include_directories(convolution PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
