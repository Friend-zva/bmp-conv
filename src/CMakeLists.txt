add_library(cbmp cbmp/cbmp.c)
target_include_directories(cbmp PUBLIC cbmp)

add_subdirectory(convolution)

add_library(options options/options.c)
target_include_directories(options PUBLIC options)

add_executable(main main.c)
target_link_libraries(main PRIVATE
        convolution
        options)
target_include_directories(main PRIVATE
        convolution
        options)
target_compile_options(main PRIVATE
        -Wall -Wextra -pedantic -Werror
#        -fsanitize=address
        -pthread)
