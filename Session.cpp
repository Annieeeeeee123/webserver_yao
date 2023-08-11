#include "Session.h"

Session::Session()
{

}
Session::~Session()
{

}

Session::Session(string UserName,int ID, string time)
{
    this->id = ID;
    this->name = UserName;
    this->expire_time = time;
}

int Session::GetId()
{
    return this->id;
}

string Session:: GetExpireTime()
{
    return this->expire_time;
}

string Session::GetUserName()
{
    return this->name;
}

bool Session::SetSessionData()
{
    char str[20];
    memset(str,0,sizeof(str));
    //这里使用name+id的方式，生成最终的sessionid
    sprintf(str,"%d",GetId());
    SessionData = GetUserName()+str;
    return true;

}

string Session::GetSessionData()
{
    if(!SessionData.empty())
        return SessionData;
}

unsigned char* Session::GetSessionId()
{
    return md5.GetMd5();
}

bool Session::SetSessionId()
{
    bool res = md5.SetMd5(GetSessionData());
    if(!res)
        return false;
    return true;
}
