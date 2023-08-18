#ifndef WEBSERVER_H
#define WEBSERVER_H

#include<stdio.h>
#include<iostream>
#include<cstring>
#include<stdlib.h>
#include<sys/fcntl.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<errno.h>
#include<sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include"mmysql.h"
#include <sys/epoll.h>
#include "httpconn.h"
#include "utils.h"

struct pthread_info{
    int m_clinfid;
    httpconn *user;
};

const int MAX_FD = 65536;           //最大文件描述符
const int MAX_EVENT_NUMBER = 128; //最大事件数
const int TIMESLOT = 5;             //最小超时单位

class webserver{
    public: 
        webserver();
        ~webserver();
        void InitSql();
        bool EventListen();
        void EventLoop();
     
    public: 
        struct pthread_info pinfos[128]; 
        unsigned int  m_port;
        int m_listenfd;
        int m_epollfd;
        //epoll_event相关
        epoll_event events[MAX_EVENT_NUMBER];
        mysql_con mysql;
        httpconn *users;

    private:
    /*工作线程运行的函数，它不断从工作队列中取出任务并执行*/
    static void *working(void *arg);
    void run(int m_clintfd);
};

#endif