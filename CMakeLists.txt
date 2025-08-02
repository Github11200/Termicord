cmake_minimum_required(VERSION 3.10.0)

# Enable address sanitizer if using GCC or Clang
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Enabling AddressSanitizer")
    add_compile_options(-fsanitize=address -g)
    add_link_options(-fsanitize=address)
endif()


project(TERMICORD VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(http)
