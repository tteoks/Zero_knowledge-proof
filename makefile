CC=gcc -Wall                                                                                    
INCLUDE=-I/usr/include/mysql -I./src 
LIBS=-L/usr/libs/mysql
DEFS=-lmysqlclient -lssl -lcrypto

all: database.o crypto.o server.o client.o
	${CC} -g -o client database.o crypto.o client.o ${INCLUDE} ${LIBS} ${DEFS}
	${CC} -g -o server database.o crypto.o server.o ${INCLUDE} ${LIBS} ${DEFS}

client: database.o crypto.o client.o
	${CC} -g -o client database.o crypto.o client.o ${INCLUDE} ${LIBS} ${DEFS}

server: database.o crypto.o server.o
	${CC} -g -o server database.o crypto.o server.o ${INCLUDE} ${LIBS} ${DEFS}

database.o : ./src/database.c
	${CC} ./src/database.c -c -o database.o ${INCLUDE} ${LIBS} ${DEFS} 

crypto.o : ./src/crypto.c	
	${CC} ./src/crypto.c -c -o crypto.o ${INCLUDE} ${DEFS}

client.o : client.c
	${CC} ./client.c -c -o client.o ${INCLUDE} ${DEFS}

server.o : server.c
	${CC} ./server.c -c -o server.o ${INCLUDE} ${DEFS}

clean:
	rm *.o client server
