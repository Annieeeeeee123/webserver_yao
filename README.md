# webserver_yao

项目描述：基于C++在Linux下实现多线程Web服务器，支持用户登录功能和会话保持；

         使用多线程机制处理客户端连接，增加并行服务数量；

　　　　　解析get/post请求并返回http响应。get请求返回静态资源，post请求访问mysql数据库返回动态消息；

　　　　　支持application/json格式的数据传输，成功与vue前端实现通信；

　　　　　利用SessionID实现会话保持。　　


环境：WSL Ubuntu18.04  gcc 7.5.0  mysql 5.7.42
