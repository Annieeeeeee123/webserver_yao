#ifndef HTTPCONN_H
#define HTTPCONN_H

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

class httpconn
{
    public:
        httpconn(int m_clintfd, mysql_con* mysql);
        ~httpconn();
        void handle_client();
    private:
        void *handle_request(char *request);
        void get_response();
        void post_response(char * request);

    private:
        int m_clintfd;
        mysql_con* mysql;
};
#endif
