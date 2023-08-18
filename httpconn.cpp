#include "httpconn.h"

using namespace std;

int httpconn::m_user_count = 0;
int httpconn::m_epollfd = -1;

httpconn::httpconn(){
}


void httpconn::Init(int clintfd, const sockaddr_in &addr,  mysql_con *mysqlconn){
    m_clintfd = clintfd;
    mysql = mysqlconn;
    m_address = addr;
    addfd(m_epollfd, m_clintfd, true);
    m_user_count++;
    printf("http_conn init: %d\n", m_clintfd);
}

void httpconn::close_conn(){
    if(m_clintfd!=-1){
        remove_fd(m_epollfd, m_clintfd);
    }
    printf("close_conn: %d\n", m_clintfd);
    m_clintfd = -1;
    m_user_count--;
}

httpconn::~httpconn(){

}

bool httpconn:: check_session_state(string sessionid){
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    string time_now = stime(t);
    vector<vector<string>>  sess = mysql->getDatafromSessionDB("session", 0, sessionid);
    if(sess.size()>0){
        vector<string> ses = sess[0];
        string expire_time = ses[3];
        // cout<<"expire_time:"<<expire_time<<endl;//expire_time:2023-08-10 17:10:42
        // cout<<"time_now:"<<time_now<<endl; //time_now:2023-8-10 17:6:16
        if(expire_time > time_now) return true; 
        else return false;
    }else{
        return false;
    }
}

void  httpconn::bad_request(){
    // 构造响应消息
    string response= "HTTP/1.1 400 Bad Request\r\n"
                    "Content-Type:text/plain \r\n"
                    "\r\n";
    response+= "Bad Request!";        
    char write_buffer[1024]={0};
    printf("%s\n", write_buffer);
    strcpy(write_buffer,response.c_str());
    send(m_clintfd, write_buffer, strlen(write_buffer), 0);
}

void httpconn:: handle_client() {
    char read_buffer[1024];
    // 读取客户端发送的HTTP请求
    ssize_t request_size = recv(m_clintfd, read_buffer, sizeof(read_buffer), 0);
    if (request_size == -1) {
        perror("Receive failed");
        return;
    }
    read_buffer[request_size] = '\0';
    printf("%s\n", read_buffer);
    // 处理HTTP请求
    handle_request(read_buffer);
}

void httpconn:: handle_request(char *request) {
    char method[1024], url[1024], http_version[1024];
    char *header, *body;
    sscanf(request, "%s %s %s", method, url, http_version);
    body = strstr(request, "\r\n\r\n");
    // 处理GET请求
    if (strcmp(method, "GET") == 0) {
        if(strcmp(url, "/query_user") == 0){
            header = strstr(request, "SessionID")+11;
            string str_header = header;
            string session_id = str_header.substr(0,32);
            query_user(session_id);
        }else{
            bad_request();
        }
    }
    // 处理POST请求
    else if (strcmp(method, "POST") == 0) {
        body = strstr(request, "\r\n\r\n");
        if (body == NULL) {
            return;
        }
        body=body+4;
        if (strcmp(url, "/login") == 0) {
            login(body);
        }else{
            bad_request();
        }
    }else if(strcmp(method, "OPTIONS") == 0){
        options_response();
    }
}

void httpconn::options_response(){
    /***
     * 用于处理axios发起的预检options请求
    */
        // 构造响应消息
        string response= "HTTP/1.1 200 OK\r\n"
                         "Server: WebServer Yao\r\n"
                         "Content-Type: application/json\r\n"
                         "Connection: keep-alive\r\n"
                         "Access-Control-Allow-Origin: *\r\n"
                         "Access-Control-Allow-Headers: *\r\n"
                         "Access-Control-Max-Age: 2100\r\n\r\n";
     
        char write_buffer[1024];
        strcpy(write_buffer,response.c_str());
        printf("%s\n", write_buffer);
        send(m_clintfd, write_buffer, strlen(write_buffer), 0);
}


void httpconn::query_user(string params){
    /***
     * 检查用户的登陆状态
    */
        string sessionid = params;
        //检查登陆状态
        if(check_session_state(sessionid)){
            res_pons.res_code = 0;
            res_pons.res_info = "session state!";
        }else{
            res_pons.res_code = 2;
            res_pons.res_info = "session outdated!";
        }
        // 构造响应消息
        string response= "HTTP/1.1 200 OK\r\n"
                         "Server: WebServer Yao\r\n"
                         "Content-Type: application/json\r\n"
                         "Access-Control-Allow-Origin: *\r\n"
                         "Access-Control-Allow-Headers: *\r\n"
                         "Access-Control-Allow-Method: GET, POST\r\n"
                         "Connection: keep-alive\r\n";
     
        string jsonStr;
        JsonHelper::ObjectToJson(res_pons, jsonStr);
        int len = jsonStr.size();
        char con_len[64]={};
        sprintf(con_len,"Content-Length: %d\r\n\r\n", len);
        response+=con_len;
        response+=jsonStr;        
        char write_buffer[1024];
        strcpy(write_buffer,response.c_str());
        printf("%s\n", write_buffer);
        send(m_clintfd, write_buffer, strlen(write_buffer), 0);
}

void httpconn:: login(char *params){
    JsonHelper::JsonToObject(usr, params);
    vector<vector<string>>  users = mysql->getDatafromUserDB("user",usr.username);
    if(users.size()==0){
        login_res.res_code = 1;
        login_res.res_info = "user incorrect!";
        login_res.user_id = 0;
        login_res.session_id = "";
    }else if(users.size()>0){
       vector<string> local_user = users[0];
       if(local_user[2] == usr.password){
            int usr_id = stoi(local_user[0]);
            string sessionid = session_id(usr.username,usr_id);
            login_res.res_code = 0;
            login_res.res_info = "login success";
            login_res.user_id = usr_id;
            login_res.session_id = sessionid;
       }else{
            login_res.res_code = 1;
            login_res.res_info = "passward incorrect!";
            login_res.user_id = 0;
            login_res.session_id = "";
       }
    }
    string jsonStr;
    JsonHelper::ObjectToJson(login_res, jsonStr);
    // 构造响应消息
    string response= "HTTP/1.1 200 OK\r\n"
                         "Server: WebServer Yao\r\n"
                         "Content-Type: application/json\r\n"
                         "Access-Control-Allow-Origin: *\r\n"
                         "Access-Control-Allow-Headers: *\r\n"
                         "Access-Control-Allow-Method: GET, POST\r\n"
                         "Connection: keep-alive\r\n";
    int len = jsonStr.size();
    char con_len[64]={};
    sprintf(con_len,"Content-Length: %d\r\n\r\n", len);
    response+=con_len;
    response+=jsonStr;        
    char write_buffer[1024]={0};
    strcpy(write_buffer,response.c_str());
    printf("%s\n", write_buffer);
    send(m_clintfd, write_buffer, strlen(write_buffer), 0);
}

string httpconn:: session_id(string username, int id){
    /***
     * 生成session_id和expire_time，存入mysql数据库
     * 若该user登陆过，则更新expire_time。会话时间为2分钟
     * 返回session_id
    */
    string session_id="";
    vector<vector<string>>  sess = mysql->getDatafromSessionDB("session", id, "");
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    // 工具类函数stime，将时间格式化
    string time_now = stime(t);
    t->tm_min+=2; //会话时间2分钟
    string expire_time = stime(t);
    if(sess.size()>0){
        vector<string> ses = sess[0];
        session_id =  ses[2];
        mysql->UpdateSessionDB("session",session_id, expire_time);
    }else{
        unsigned char* str = new unsigned char[16];
        Session session(username,id, expire_time);
        session.SetSessionData();
        session.SetSessionId();
        str = session.GetSessionId();
        char session_str[16];
        for(int i=0;i<16;i++)
        {
            sprintf(session_str, "%02X", str[i]);
            session_id+=session_str;
        }   
        mysql->InsertSessionDB("session",id, session_id, expire_time);
    }
    return session_id;
}

