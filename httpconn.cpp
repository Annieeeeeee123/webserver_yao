#include "httpconn.h"

using namespace std;

httpconn::httpconn(int clintfd, mysql_con* mysqlconn){
    m_clintfd = clintfd;
    mysql = mysqlconn;
}


httpconn::~httpconn(){}

char *get_header_value(char *header, const char *key) {
    // 查找key
    char *key_pos = strstr(header, key);
    if (key_pos == NULL) { return NULL; }
    // 查找value
    char *value_pos = strchr(key_pos, ':');
    if (value_pos == NULL) {
        return NULL;
    }
    value_pos++;

    // 跳过前面的空格
    while (*value_pos == ' ') {
        value_pos++;
    }

    // 查找结尾的\r\n
    char *end_pos = strstr(value_pos, "\r\n");
    if (end_pos == NULL) {
        return NULL;
    }

    // 截取value
    *end_pos = '\0';

    return value_pos;
}

void httpconn::get_response(){
        //查询所有表数据
        vector<vector<string>>  users = mysql->getDatafromDB("user","");
        string table = "<html><body><table border='1'><tr><th>id</th><th>user_name</th> <th>passward</th></tr>";
        for(int i=0;i<users.size();i++){
            vector<string> line=users[i];
            table+="<tr>";
            table+="<td>"+line[0]+"</td>";
            table+="<td>"+line[1]+"</td>";
            table+="<td>"+line[2]+"</td>";
            table+="</tr>";
        }        
        table+= "</table></body></html>";
        // 构造响应消息
        string response= "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/html\r\n"
                         "\r\n";
        response+= table;        
        char buffer[1024];
        strcpy(buffer,response.c_str());
        send(m_clintfd, buffer, strlen(buffer), 0);
}

void  httpconn::post_response(char *request){
    // 处理POST请求体
    char *header, *body;
    // header = strstr(request, "Content-Length");
    // if (header == NULL) {
    //     return;
    // }
    // // 解析Content-Length头部的值
    // int content_length = atoi(get_header_value(header, "Content-Length"));

    // 查找请求体
    body = strstr(request, "\r\n\r\n");
    if (body == NULL) {
        return;
    }
    body=body+4;
    // 处理POST请求体
    printf("POST request body: %s\n", body);

    char *name = strstr(body,"=")+1;
    string sname(name);
    vector<vector<string>>  users = mysql->getDatafromDB("user",sname);
    string res;
    if(users.size()>0){
        res+="user_name: ";
        res+=name;
        res+=" passward: ";
        res+=users[0][2];
    }else{
        res+="find not!";
    }

    // 构造响应消息
    string response= "HTTP/1.1 200 OK\r\n"
                    "Content-Type:text/plain \r\n"
                    "\r\n";
    response+= res;        
    char buffer[1024];
    strcpy(buffer,response.c_str());
    send(m_clintfd, buffer, strlen(buffer), 0);
}

void httpconn:: handle_client() {
    char buffer[1024];
    // 读取客户端发送的HTTP请求
    ssize_t request_size = recv(m_clintfd, buffer, sizeof(buffer), 0);
    if (request_size == -1) {
        perror("Receive failed");
        return;
    }
    
    buffer[request_size] = '\0';
    printf("%s\n", buffer);
    // 处理HTTP请求
    handle_request(buffer);
}

void * httpconn:: handle_request(char *request) {
    char method[1024], url[1024], http_version[1024];
    char *header, *body;
    sscanf(request, "%s %s %s", method, url, http_version);
    // 处理GET请求
    if (strcmp(method, "GET") == 0) {
        get_response();
    }
    // 处理POST请求
    else if (strcmp(method, "POST") == 0) {
        post_response(request);
    }
}
