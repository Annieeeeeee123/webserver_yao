server: main.cpp mmysql.cpp webserver.cpp httpconn.cpp Md5.cpp Session.cpp utils.cpp
	g++  $^ -o  $@ -lmysqlclient -pthread -g -lssl -lcrypto

run: server
	./$<

clean:
	rm -rf *.o
