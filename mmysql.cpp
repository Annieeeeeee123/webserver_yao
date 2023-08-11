#include "mmysql.h"

using namespace std;

mysql_con:: mysql_con()
{
	m_host = "localhost";
	m_Port = 3306;
	m_User = "root";
	m_PassWord = "Raner123!";
	m_DatabaseName = "databaseyao";
    mysql = NULL;
}

mysql_con::	~mysql_con(){
    if(!mysql)
    {
        mysql_close(mysql);
    }
}

bool mysql_con:: ConnectDB()
{
	// MYSQL* mysql = NULL;
	mysql = mysql_init(mysql);
	if (!mysql) {
		cout<<"Mysql error"<<endl;//返回并打印错误信息函数
		return false;
	}
	mysql_options(mysql, MYSQL_SET_CHARSET_NAME, "gbk");//连接设置
	mysql = mysql_real_connect(mysql, m_host.c_str(), m_User.c_str(), m_PassWord.c_str(), m_DatabaseName.c_str(), m_Port, NULL, 0);
	//中间分别是主机，用户名，密码，数据库名，端口号（可以写默认0或者3306等），可以先写成参数再传进去
	if (mysql == NULL) {
		cout<<"Mysql error"<<endl;
		return false;
	}
	cout<<"Database connected successful"<<endl;//连接成功反馈
    return true;
}

vector<vector<string>> mysql_con::  getDatafromUserDB(string table_name, string user_name) 
{
	std::vector<std::vector<std::string> > data;
	std::string queryStr = "select * from "+ table_name;
	if(user_name==""){
		queryStr+=";";
	}else{
		string con = " where user_name = '"+user_name+"' ;";
		queryStr+=con;
	}

	if (0 != mysql_query(mysql, queryStr.c_str())) {
		cout<<"mysql select error"<<endl;
		return data;
	}
	MYSQL_RES* result = mysql_store_result(mysql);//获得数据库结果集
	int row = mysql_num_rows(result);//获得结果集中的行数
	int field = mysql_num_fields(result);//获得结果集中的列数

	MYSQL_ROW line = NULL;
	line = mysql_fetch_row(result);
	string temp;
	while (NULL != line) {
		vector<string> linedata;
		for (int i = 0; i < field; i++) {//获取每一行的内容
			if (line[i]) {
				temp = line[i];
				linedata.push_back(temp);
			}
			else {
				temp = "NULL";
				linedata.push_back(temp);
			}
		}
		line = mysql_fetch_row(result);
		data.push_back(linedata);
	}
	return data;
}

bool mysql_con::InsertUserDB(string table_name, string m_name, string pass) 
{
	if (m_name.length() == 0) {
		cout<<"人名不能为空!"<<endl;
		return false;
	}
	std::string str = "insert into "+ table_name + "(user_name,passward ) values('" + m_name + "', '" + pass + "');";
	if (mysql_query(mysql, str.c_str())) {
		cout<<"mysql user insert error"<<endl;
		return false;
	}
	return true;
}


vector<vector<string>> mysql_con:: getDatafromSessionDB(string table_name, int user_id, string sessionid) 
{
	std::vector<std::vector<std::string> > data;
	string queryStr;
	if(user_id !=0){
	    queryStr= "select * from "+ table_name + " where user_id = '"+ to_string(user_id)+"';";
	}
	if(sessionid!=""){
		 queryStr= "select * from "+ table_name + " where session_id = '"+ sessionid+"';";
	}
	// cout<<queryStr<<endl;

	if (0 != mysql_query(mysql, queryStr.c_str())) {
		cout<<"mysql session select error"<<endl;
		return data;
	}
	MYSQL_RES* result = mysql_store_result(mysql);//获得数据库结果集
	int row = mysql_num_rows(result);//获得结果集中的行数
	int field = mysql_num_fields(result);//获得结果集中的列数

	MYSQL_ROW line = NULL;
	line = mysql_fetch_row(result);
	string temp;
	while (NULL != line) {
		vector<string> linedata;
		for (int i = 0; i < field; i++) {//获取每一行的内容
			if (line[i]) {
				temp = line[i];
				linedata.push_back(temp);
			}
			else {
				temp = "NULL";
				linedata.push_back(temp);
			}
		}
		line = mysql_fetch_row(result);
		data.push_back(linedata);
	}
	return data;
}


bool mysql_con::InsertSessionDB(string table_name, int user_id, string session_id, string expire_time) 
{
	string str = "insert into "+ table_name + "(user_id,session_id,expire_time) values('" + to_string(user_id) + "', '" + session_id 
	+"', '"+ expire_time + "');";
	cout<<str<<endl;
	if (mysql_query(mysql, str.c_str())) {
		cout<<"mysql session insert error"<<endl;
		return false;
	}
	return true;
}

bool mysql_con::UpdateSessionDB(string table_name, string session_id, string expire_time) 
{
	string str = "update `"+ table_name + "` set expire_time= '" + expire_time+"' where session_id = '"+ session_id+ "';";
	cout<<str<<endl;
	if (mysql_query(mysql, str.c_str())) {
		cout<<"mysql session update error"<<endl;
		return false;
	}
	return true;
}



bool mysql_con::DeleteUserDB(string table_name, string m_name) {
	if (m_name.length() == 0) {
		cout<<"人名不能为空!"<<endl;
		return false;
	}
	std::string query = "delete from "+ table_name +" where user_name = '" + m_name + "';";
	if (mysql_query(mysql, query.c_str())) {
		cout<<"mysql user delete error"<<endl;
		return false;
	}
	return true;
}

bool mysql_con::UpdateUserDB(string table_name, int user_id, string m_name, string pass) {
	if (m_name.length() == 0) {
		cout<<"人名不能为空!"<<endl;
		return false;
	}
	std::string query = "update "+ table_name +" set user_name = '" + m_name + "', passward = '" + pass + "' where user_id = '" + to_string(user_id) + "';";
	if (mysql_query(mysql, query.c_str())) {
		cout<<"mysql user update error"<<endl;
		return false;
	}
	return true;
}

