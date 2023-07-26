server: main.cpp mmysql.cpp webserver.cpp httpconn.cpp
	g++  $^ -o  $@  -lmysqlclient -pthread -g

run: server
	./$<

clean:
	rm -rf *.o
