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

struct pthread_info{
    int m_clinfid;
    mysql_con* mysql;
};

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
        mysql_con mysql;
};

#endif