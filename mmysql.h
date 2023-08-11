#ifndef _MMYSQL_H_
#define _MMYSQL_H_

#include <mysql/mysql.h>
#include <string.h>
#include <vector>
#include <iostream>
using namespace std;

class mysql_con
{
public:
    string m_host;			 //主机地址
    int m_Port;		 //数据库端口号
    string m_User;		 //登陆数据库用户名
    string m_PassWord;	 //登陆数据库密码
    string m_DatabaseName; //使用数据库名
    MYSQL* mysql;

public:
    bool ConnectDB();
    vector<vector<string>> getDatafromUserDB(string table_name,string user_name);
    vector<vector<string>> getDatafromSessionDB(string table_name,int user_id=0, string session_id="");
    bool InsertUserDB(string table_name, string m_name, string pass);
    bool UpdateUserDB(string table_name, int user_id, string m_name, string pass);
    bool DeleteUserDB(string table_name, string m_name);
    bool InsertSessionDB(string table_name, int user_id, string session_id, string expire_time);
    bool UpdateSessionDB(string table_name, string session_id, string expire_time);
	mysql_con();
	~mysql_con();

};

#endif
