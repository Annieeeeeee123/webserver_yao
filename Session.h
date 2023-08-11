#ifndef SESSION_H
#define SESSION_H
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "Md5.h"


using namespace std;

class Session
{
public:
    Session();
    ~Session();
    Session(string UserName,int ID,string time);
    bool SetId();
    int GetId();
    string GetExpireTime();
    bool SetUserName();
    string GetUserName();
    bool SetSessionId();
    bool SetSessionData();
    string GetSessionData();
    unsigned char* GetSessionId();
private:
    string name;
    int id;
    string SessionData;
    Md5 md5;
    string expire_time;
};
#endif