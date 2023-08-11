#include "webserver.h"
#include "httpconn.h"
using namespace std;

webserver::webserver(){
    m_port=9000;
    InitSql();
}

void webserver::InitSql(){
    mysql.ConnectDB();
}


webserver::~webserver(){
}

void * working( void *arg){
    struct pthread_info *pinfo = (struct pthread_info *)arg;
    int m_clintfd =pinfo->m_clinfid;
    mysql_con* mysql = pinfo->mysql;
    httpconn user_http(m_clintfd, mysql);
    user_http.handle_client();
    close(m_clintfd);
    pinfo->m_clinfid=-1;
    return NULL;
}

bool webserver:: EventListen()
{
    //create socket for listen
    m_listenfd=socket(AF_INET,SOCK_STREAM,0);// AF_INET协议族 IPv4  SOCK_STREAM流式协议TCP 
    int reuse = 1;
    setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
    if(m_listenfd==-1)
    {
        printf("socket create fail\n");
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serveraddr.sin_port=htons(m_port);// htons // 主机字节序 - 网络字节序
    if(bind(m_listenfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))!=0)
    {
        perror("bind");
        return false;
    }

    if(listen(m_listenfd,5)!=0)
    {
        perror("listen");
        return false;
    }
    return true;
}

void webserver::EventLoop(){
    int p_len = sizeof(pinfos)/sizeof(pinfos[0]);
    memset(pinfos,-1,sizeof(pinfos));
    // loop receive client's request
    while(1){
        char clintIP[16];
        int socklen=sizeof(struct sockaddr_in);
        struct sockaddr_in client_addr; //这是一个传出参数，可以知道客户端的ip和port
        int m_clintfd=accept(m_listenfd,(struct sockaddr*)&client_addr, (socklen_t *)&socklen);  //这是一个阻塞函数,返回用于通信的fd
        inet_ntop(AF_INET, &client_addr.sin_addr, clintIP, sizeof(clintIP));
        unsigned short clintPort = ntohs(client_addr.sin_port); 
        if(m_clintfd==-1){
            printf("connect failed\n");
            break;
        }

        printf("client %s: %d has connnected\n",clintIP, clintPort);
        struct pthread_info *pinfo = NULL; ///这样while退出作用域后内存才不会被回收。
        for(int i=0;i<p_len;i++){
            if(pinfos[i].m_clinfid == -1){
                pinfos[i].m_clinfid = m_clintfd;
                pinfos[i].mysql = &mysql;
                pinfo = &pinfos[i];
                break;
            }
        }
        // 创建线程，处理客户端连接
        if(pinfo!=NULL){
            pthread_t tid;
            pthread_create(&tid, NULL, working, (void*)pinfo);
            pthread_detach(tid); //不能用阻塞的pthread_join,因为while循环要往下进行
        }
    }
    close(m_listenfd);
}
