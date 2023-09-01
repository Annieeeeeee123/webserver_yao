# webserver_yao

## 项目描述

基于C++在Linux下实现多线程Web服务器，支持用户登录功能和会话保持；

使用epoll多路IO复用监听socket, 利用多线程机制处理客户端连接，增加并行服务数量；

解析get/post请求，访问mysql数据库，返回json格式的响应消息；

支持application/json格式的数据传输，设置响应头实现跨域， 成功与vue前端实现通信；

利用SessionID实现会话保持。　　


环境：WSL Ubuntu18.04  gcc 7.5.0  mysql 5.7.42

## 运行方式
sudo service start mysql  //启动mysql

make server  //编译

make run  //运行


## 功能介绍
用户登陆界面，根据用户名和密码验证登录信息（login），并返回user_id和session_id。
登录后维护用户会话30分钟，超时请求服务（query）会自动转入login页面。

支持login（POST）, query_user（GET）接口

