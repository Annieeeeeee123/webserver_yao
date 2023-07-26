#include<iostream>
#include"webserver.h"

using namespace std;

int main(){

    webserver ws;
    bool flag = ws.EventListen(); //socket监听
    if(flag) ws.EventLoop(); //监听成功后，socket循环接受消息
    return 0;
}
