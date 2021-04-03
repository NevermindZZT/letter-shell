# cpp suport

![standard](https://img.shields.io/badge/standard-c99-brightgreen.svg)
![build](https://img.shields.io/badge/build-2021.01.17-brightgreen.svg)
![license](https://img.shields.io/badge/license-MIT-brightgreen.svg)

`letter shell`cpp支持

## 简介

`cpp support`是用于扩展`letter shell`，使其可以在cpp环境中生效的组件，`cpp support`仅仅包含一个头文件

## 使用

`cpp support`仅仅提供了一个`shell_cpp.h`的头文件，用于在cpp文件中使用`letter shell`的命令导出，在使用时，需要在`.cpp`文件中包含`shell_cpp.h`，而不是`shell.h`，如果一个工程同时包含`.c`和`.cpp`文件，只需要分别包含对应的头文件即可

注意，对于cpp，`letter shell`也仅支持函数的导出，不适用于类的成员函数
