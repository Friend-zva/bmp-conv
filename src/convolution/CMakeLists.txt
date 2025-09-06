file(GLOB_RECURSE C_SOURCES ./*.c)

if(NOT OpenCL_SUPPORT)
    list(REMOVE_ITEM C_SOURCES ./gpu/*.c)
endif()

add_library(convolution ${C_SOURCES})
target_link_libraries(convolution PUBLIC cbmp)

if(OpenCL_SUPPORT)
    if(APPLE)
        target_link_libraries(convolution PRIVATE "-framework OpenCL")
    else()
        find_package(OpenCL REQUIRED)
        target_link_libraries(convolution PRIVATE OpenCL)
    endif()
endif()

target_include_directories(convolution PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
