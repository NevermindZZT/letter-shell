# esp-idf demo

此目录文件用于在 `esp-idf` 中使用 `letter-shell` 做参考

## 使用

此 demo 已经包含 `CMakeLists.txt`, 可直接作为 `esp-idf` 的模块编译

## 注意

- `esp-idf` 编译系统会忽略 `__attribute__((used))` 声明，所以仅仅作为命令定义的函数不会被包含在编译出来的固件里面，只有被代码引用的函数会被编译进去，参考 ![保留符号](#保留符号)

- 此 demo 包含链接使用的 `.lf` 文件，在使用这个文件的情况下不需要修改 `esp-idf` 中的 `ld` 文件

- 如果使用 overlay 的方式配置 shell，建议在主 CMakeList.txt 后面添加 `idf_build_set_property(COMPILE_OPTIONS "-DSHELL_CFG_USER=\"shell_cfg_user.h\"" APPEND)`

## 保留符号

由于 shell 命令实际上声明的是一个全局变量，并且没有被引用过，所以编译系统编译时会忽略这个变量，导致这个命令不会被编译进去，`__attribute__((used))` 声明就是用来防止编译系统优化的

同时，需要修改 ld 文件，将这些符号编译到一块连续的空间中，这块可以参考项目 readme

对于 esp-idf，还提供了一个 lf 文件的方式，此 demo 就是使用了这种方式，但是 lf 文件仅作用于当前组件，就会导致其他组件声明的命令不会被编译进来，可以在其他组件的 CMakeLists.txt 中添加 `target_link_libraries(${COMPONENT_LIB} INTERFACE "-u shellCommandtest")` 来解决这个问题，其中 `shellCommandtest` 是就是 shell 导出时声明的符号，以 `shellCommand` 开头，拼接命令名即可
