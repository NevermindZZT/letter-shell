# segger rtt demo

此目录文件为基于 segger rtt 使用 letter-shell 的实例

## 使用

demo 只给出了 shell 使用 segger rtt 的关键配置，集成编译时，请下载最新的 segger rtt 代码一起集成

demo 中 `delay`, `getTick` 函数需要根据自己使用的平台进行修改

基于 segger rtt 使用时，需要使用 jlink rtt 上位机收发数据

这里推荐一个工具，可以将 rtt 转发为 telnet，[Rtt2Telnet](https://github.com/mcujackson/Rtt2Telnet)
