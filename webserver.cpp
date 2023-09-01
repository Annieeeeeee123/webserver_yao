#include "webserver.h"
using namespace std;

webserver::webserver(){
    m_port=9000;
    users = new httpconn[MAX_FD];
    InitSql();
}

void webserver::InitSql(){
    mysql.ConnectDB();
}


webserver::~webserver(){
    remove_fd(m_epollfd, m_listenfd);
    close(m_epollfd);
    delete [] users;
}
void * webserver:: working( void *arg){
    struct pthread_info *pinfo = (struct pthread_info *)arg;
    int m_clintfd =pinfo->m_clinfid;
    httpconn *user = pinfo->user;
    user->handle_client();
    user->close_conn();
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

    if(listen(m_listenfd,8)!=0)
    {
        perror("listen");
        return false;
    }

    int epollfd = epoll_create(5);
    m_epollfd = epollfd;
    httpconn:: m_epollfd = epollfd;
    addfd(epollfd, m_listenfd, false);
    return true;
}

void webserver::EventLoop(){
    int p_len = sizeof(pinfos)/sizeof(pinfos[0]);
    memset(pinfos,-1,sizeof(pinfos));
    // loop receive client's request
    while(1){
        int number =  epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno != EINTR)
        {
            printf("epoll wait failed\n");
            break;
        }
        for(int i=0; i<number; i++){
            int sockfd = events[i].data.fd;
            if(sockfd == m_listenfd){
                //有客户端连接
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
                users[m_clintfd].Init(m_clintfd, client_addr, &mysql);
                printf("client %s: %d has connnected\n",clintIP, clintPort);
            }
            else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                //服务器端关闭连接
                printf("close client connect\n");
                users[sockfd].close_conn();
            }       
            else{
                // 读取数据，处理
                struct pthread_info *pinfo = NULL; ///这样while退出作用域后内存才不会被回收。
                for(int i=0;i<p_len;i++){
                    if(pinfos[i].m_clinfid == -1){
                        pinfos[i].m_clinfid = sockfd;
                        pinfos[i].user = &users[sockfd];
                        pinfo = &pinfos[i];
                        break;
                    }
                }
                // 创建线程，working函数处理客户端连接
                if(pinfo!=NULL){
                    pthread_t tid;
                    pthread_create(&tid, NULL, working, (void*)pinfo);
                    pthread_detach(tid); //不能用阻塞的pthread_join,因为while循环要往下进行
                }
            }
        }
    }
}
