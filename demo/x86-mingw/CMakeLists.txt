# letter shell demo for Windows (MinGW-w64)
#
# 与 demo/x86-gcc 使用相同的源文件，仅修正平台差异：
#   * 不使用 ELF 链接脚本 shell.lds，改用 ld 自动生成的
#     __start_shellCommand / __stop_shellCommand 作为命令表边界
#   * 不编译 extensions/telnet/telnetd.c（依赖 POSIX <sys/socket.h>），
#     由 telnetd_stub.c 提供空实现
#   * -mno-ms-bitfields：MinGW 默认使用 MSVC 位域布局，会把
#     ShellCommandType 位域挤到偏移 4，导致 SHELL_CMD_TYPE() 失效
#   * -std=gnu17 并放宽 GCC 14+ 升级为 error 的诊断
#
# 编译：cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -S . -B build
#       cmake --build build
# 运行：build/LetterShell.exe
cmake_minimum_required(VERSION 3.10)
project(LetterShellWin C CXX)

set(LS "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(DEMO "${LS}/demo/x86-gcc")

add_definitions(-DSHELL_CFG_USER="shell_cfg_user.h")
add_definitions(-D_shell_command_start=__start_shellCommand)
add_definitions(-D_shell_command_end=__stop_shellCommand)

add_executable(LetterShell
               "${DEMO}/main.c"
               "${DEMO}/shell_port.c"
               "${DEMO}/shell_cpp.cpp"
               telnetd_stub.c
               "${LS}/src/shell.c"
               "${LS}/src/shell_companion.c"
               "${LS}/src/shell_ext.c"
               "${LS}/src/shell_cmd_list.c"
               "${LS}/extensions/fs_support/shell_fs.c"
               "${LS}/extensions/log/log.c"
               "${LS}/extensions/shell_enhance/shell_passthrough.c"
               "${LS}/extensions/shell_enhance/shell_cmd_group.c"
               "${LS}/extensions/shell_enhance/shell_secure_user.c"
               "${LS}/extensions/game/game.c"
               "${LS}/extensions/game/2048/2048.c"
               "${LS}/extensions/game/pushbox/pushbox.c"
               )

target_include_directories(LetterShell PUBLIC
                           "${DEMO}"
                           "${LS}/src"
                           "${LS}/extensions/fs_support"
                           "${LS}/extensions/cpp_support"
                           "${LS}/extensions/log"
                           "${LS}/extensions/shell_enhance"
                           "${LS}/extensions/telnet"
                           )

target_compile_options(LetterShell PRIVATE
                       -mno-ms-bitfields
                       -std=gnu17
                       -Wno-incompatible-pointer-types
                       -Wno-int-conversion
                       -Wno-implicit-function-declaration
                       )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-ms-bitfields -std=c++11")