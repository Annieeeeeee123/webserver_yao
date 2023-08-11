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
#include "./include/AIGCJson.hpp"
#include "Session.h"
#include "utils.h"
using namespace std;
using namespace aigc;

struct user{
    string username;
    string password;
    AIGC_JSON_HELPER(username, password)
};
/**
 * res_code = 0 请求成功，返回成功状态
 * res_code = 1 请求成功，一般情况的失败消息
 * res_code = 2 请求成功，用户会话超时
*/


struct response{
    int res_code;
    string res_info;
    AIGC_JSON_HELPER(res_code, res_info)
};

struct login_response{
    int res_code;
    string res_info;
    int user_id;
    string session_id;
    AIGC_JSON_HELPER(res_code, res_info,user_id,session_id)
};

class httpconn
{
    public:
        httpconn(int m_clintfd, mysql_con* mysql);
        ~httpconn();
        void handle_client();
    private:
        string session_id(string username, int id);
        bool check_session_state(string sessionid);
        void handle_request(char *request);
        void options_response();
        void query_user(string params);
        void bad_request();
        void login(char *params);

    private:
        int m_clintfd;
        sockaddr_in m_address;
        mysql_con* mysql;
        user usr;
        response res_pons;
        login_response login_res;
        
};
#endif
