#include<iostream>
#include<cstring>
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include<unistd.h>
#include<sys/fcntl.h>
#include<sys/socket.h>
#include <sys/epoll.h>
using namespace std;

//返回格式化时间：2023-08-10 17:10:42
string stime(tm* t);


// 添加文件描述符到epoll
void addfd(int epollfd, int fd, bool one_shot);

// 删除文件描述符到epoll
void remove_fd(int epollfd, int fd);

// 修改文件描述符，重置socket上的EPOLLONESHOT事件，以确保下一次可读时，EPOLLIN事件能被触发
void modfd(int epollfd, int fd, int ev);