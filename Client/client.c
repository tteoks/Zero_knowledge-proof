#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
void error_handle(char *msg);

int main(int argc, char *argv[])
{
	int clnt_sock;
	const char *serv_ip = argv[1];
	const char *serv_port = argv[2];
	struct sockaddr_in serv_addr;
	char message[1024] = {0x00, };


	if(argc != 3) {
		error_handle("Invalid arguments!");
	}

	clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(clnt_sock == -1) {
		error_handle("Socket ERROR!");
	}

	memset(&serv_addr, 0x00, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(serv_ip);
	serv_addr.sin_port = htons(atoi(serv_port));

	if(connect(clnt_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		error_handle("Connect ERROR!");
	}

	if(read(clnt_sock, message, sizeof(message)-1) == -1) {
		error_handle("Read ERROR!");
	}
	printf("Message from server: %s\n", message);

	close(clnt_sock);
	return 0;
}

void error_handle(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
