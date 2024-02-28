cmake_minimum_required(VERSION 3.25)
project(
        DataStructures
        VERSION 1.1.0
        DESCRIPTION "Algorithms and Data Structures"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -fdiagnostics-color=always)

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1 -g -fsanitize=address -fno-omit-frame-pointer")
# set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")

add_subdirectory(complexities)

add_subdirectory(libds)

add_subdirectory(tests)

add_subdirectory(cli)
