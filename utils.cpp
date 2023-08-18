#include "utils.h"

string stime(tm* t){
    string year = to_string(t->tm_year+1900);
    string mon = t->tm_mon+1<10?  "0"+ to_string(t->tm_mon+1):to_string(t->tm_mon+1);
    string day = t->tm_mday<10?  "0"+ to_string(t->tm_mday):to_string(t->tm_mday);
    string hour = t->tm_hour<10?  "0"+ to_string(t->tm_hour):to_string(t->tm_hour);
    string min = t->tm_min<10?  "0"+ to_string(t->tm_min):to_string(t->tm_min);
    string sec = t->tm_sec<10?  "0"+ to_string(t->tm_sec):to_string(t->tm_sec);
    string stime = year+"-"+mon+"-"+day+" "+hour+":"+min+":"+sec;
    cout<<"formate time:"<<stime<<endl;
    return stime;
}

//设置文件描述符为非阻塞
void nonblocking(int fd){
    int flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}


// 添加文件描述符到epoll
void addfd(int epollfd, int fd, bool one_shot){
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN |  EPOLLRDHUP;
    if(one_shot){
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    nonblocking(fd);
}

// 删除文件描述符到epoll
void remove_fd(int epollfd, int fd){
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}