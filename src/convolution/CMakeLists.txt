file(GLOB_RECURSE SOURCES_C *.c)

if(NOT OpenCL_SUPPORT)
    file(GLOB_RECURSE SOURCES_GPU gpu/*.c)
    list(REMOVE_ITEM SOURCES_C ${SOURCES_GPU})
endif()

add_library(convolution ${SOURCES_C})
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
