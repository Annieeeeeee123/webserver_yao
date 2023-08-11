#include "Md5.h"

Md5::Md5()
{

}

Md5::~Md5()
{

}

unsigned char* Md5::GetMd5()
{
    //数组初始化
    memset(outMd5,0x00,sizeof(outMd5));
    int res = MD5_Final(outMd5,&ctx);
    if(res != 1)
    {
        cout<<"Md5_Final is errpr"<<endl;
    }
    return outMd5;
}

bool Md5::SetMd5(string data)
{

    //初始化Md5
    MD5_Init(&ctx);
    //计算Md5
    int res = MD5_Update(&ctx,data.c_str(),5);
    if(res != 1)
    {
        cout<<"Md5_Update is errpr"<<endl;
        return false;
    }
    return true;
}