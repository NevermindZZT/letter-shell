cmake_minimum_required(VERSION 3.0.0)
project(LetterShell VERSION 0.1.0)

add_executable(LetterShell
               main.c
               shell_port.c
               shell_cpp.cpp
               ../../src/shell.c
               ../../src/shell_companion.c
               ../../src/shell_ext.c
               ../../extensions/fs_support/shell_fs.c
               ../../extensions/log/log.c
               ../../extensions/shell_enhance/shell_passthrough.c
               )

target_include_directories(LetterShell PUBLIC 
                           "${PROJECT_BINARY_DIR}" 
                           ../../src
                           ../../extensions/fs_support
                           ../../extensions/cpp_support
                           ../../extensions/log
                           ../../extensions/shell_enhance
                           ) 

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T shell.lds")
