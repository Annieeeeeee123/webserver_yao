#ifndef MD5_H
#define MD5_H

#include <iostream>
#include <openssl/md5.h>
#include <string.h>

using namespace std;

class Md5
{
public:
    Md5();
    ~Md5();
    bool SetMd5(string data);
    unsigned char* GetMd5();
private:
    MD5_CTX ctx;
    unsigned char outMd5[16];
};
#endif
