# esp-idf demo

此目录文件用于在 `esp-idf` 中使用 `letter-shell` 做参考

## 使用

此 demo 已经包含 `CMakeLists.txt`, 可直接作为 `esp-idf` 的模块编译

## 注意

- `esp-idf` 编译系统会忽略 `__attribute__((used))` 声明，所以仅仅作为命令定义的函数不会被包含在编译出来的固件里面，只有被代码引用的函数会被编译进去

- 此 demo 包含链接使用的 `.lf` 文件，在使用这个文件的情况下不需要修改 `esp-idf` 中的 `ld` 文件
