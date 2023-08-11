# webserver_yao
在Linux下利用C++实现web服务器，基于socket网络编程，支持http的get/post请求。
使用多线程机制处理客户端连接，增加并行服务数量；
解析get/post请求。get请求返回静态HTML资源，post请求访问mysql数据库返回动态消息。

环境：WSL Ubuntu18.04  gcc 7.5.0  mysql 5.7.42
