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
	// int nRcv;
	const char *serv_ip = argv[1];
	const char *serv_port = argv[2];
	struct sockaddr_in serv_addr;
	char recv_msg[1024] = {0x00, };
	char send_msg[1024] = {0x00, };

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
	} else {
		printf("%s Connection Complete!\n", inet_ntoa(serv_addr.sin_addr));
	}


/*	while(1) {
		printf("Input Your command: ");
		// scanf("%s", send_msg);
		fgets(send_msg, 1024, stdin);
		send_msg[strlen(send_msg) - 1] = '\0';

		if (strcmp(send_msg, "exit") == 0) {
			write(clnt_sock, send_msg, (int)strlen(send_msg));
			break;
		}

		write(clnt_sock, send_msg, (int)strlen(send_msg));
		printf("Message Receives...\n");

		nRcv = read(clnt_sock, recv_msg, sizeof(recv_msg) - 1);
		if (nRcv < 0) {
			printf("Receive Error..\n");
			break;
		}
		recv_msg[nRcv] = '\0';

		if (strcmp(recv_msg, "exit") == 0) {
			printf("Close Server Connection...\n");
			break;
		}

		printf("Receive Message: %s\n", recv_msg);
	}

*/
	printf("Input you Command: ");
	fgets(send_msg, 1024, stdin);
	send_msg[strlen(send_msg) - 1] = '\0';

	write(clnt_sock, send_msg, (int)strlen(send_msg));

	close(clnt_sock);
	printf("Close Connection...\n");
	return 0;
}

void error_handle(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
