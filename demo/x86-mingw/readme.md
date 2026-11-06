# x86-mingw demo

`demo/x86-gcc` 的 Windows 版本，源文件完全相同（`main.c`、`shell_port.c`、`shell_cpp.cpp`、`src/`、`extensions/`），只修正平台差异。

## 编译运行

需要 MinGW-w64（gcc/g++）与 cmake：

```sh
cd demo/x86-mingw/
cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -S . -B build
cmake --build build
./build/LetterShell.exe
```

必须显式指定 `-DCMAKE_C_COMPILER=gcc`，否则 cmake 可能选中 PATH 中未配置完整的 clang。

## 与 x86-gcc 的差异

| 差异 | 原因 |
|---|---|
| 不使用 `-T shell.lds`，改用 `-D_shell_command_start=__start_shellCommand -D_shell_command_end=__stop_shellCommand` | `shell.lds` 是 ELF64 链接脚本，PE/COFF 下不可用；ld 对名字合法的 section 会自动生成 `__start_/__stop_` 边界符号 |
| 不编译 `extensions/telnet/telnetd.c`，由 `telnetd_stub.c` 提供空实现 | 该文件依赖 POSIX `<sys/socket.h>` |
| `-mno-ms-bitfields` | MinGW 默认 MSVC 位域布局，会把 `ShellCommandType` 位域放到偏移 4，而不是 `SHELL_CMD_TYPE(t) = (t&0xF)<<8` 假设的 bit 8..11，导致初始化时在 `shellSeekCommand` 里崩溃 |
| `-std=gnu17 -Wno-incompatible-pointer-types -Wno-int-conversion -Wno-implicit-function-declaration` | GCC 14+ 把这类诊断升级为 error；`2048.c` 里的 `_XOPEN_SOURCE 500` 会隐藏 `snprintf` 声明 |

## 已知限制

- `main.c` 用 `system("stty -echo")` / `system("stty -icanon")` 进入 raw 模式，Windows 控制台没有 `stty`，命令会失败（打印 `Inappropriate ioctl for device`），shell 退化为行模式：整行回车后才处理，逐键编辑、方向键、Tab 补全不可用。若要完整交互，需改用 `SetConsoleMode` 清掉 `ENABLE_LINE_INPUT` 并自行处理按键。
- telnet server 不可用（`telentdInit` 返回 -1）。
- 其余命令（`help`/`keys`/`vars`/`cmds`/`game` 等）均正常。