#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<net/if.h>
#include<netinet/in.h>
#include<sys/ioctl.h>


void error_handle(char *msg);
void display_info(const char *serv_port);
void getIP(char *ip_buff);
void display_usage() {
	printf("Usage\n");
	printf("./server <serv_port>\n");
}


int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;
	int nRcv;
	int end;
	const char *serv_port = argv[1];
	char send_msg[1024] = { 0x00, };
	char recv_msg[1024] = { 0x00, };
	char *start, *temp;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1) {
		error_handle("Socket ERROR!");
	}

	if(argc != 2) {
		display_usage();
		exit(1);
	}

	memset(&serv_addr, 0x00, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(serv_port));
	

	if(bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		error_handle("Bind ERROR!");
	}

	if(listen(serv_sock, 5) == -1) {
		error_handle("Listen ERROR!");
	}

	display_info(serv_port);

	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_size);
	if (clnt_sock == -1) {
		error_handle("Accept ERROR");
	} else {
		printf("%s Connection Complete!\n", inet_ntoa(clnt_addr.sin_addr));
	}

/*
	while(1) {
		printf("Message Receives...\n");
		nRcv = read(clnt_sock, recv_msg, sizeof(recv_msg) - 1);

		if (nRcv < 0 ) {
			printf("Receive ERROR...\n");
			break;
		}

		recv_msg[nRcv] = '\0';

		if (strcmp(recv_msg, "exit") == 0) {
			printf("Close Clinet Connection...\n");
			break;
		}
		
		printf("Receive Message: %s\n", recv_msg);
		
		printf("Send your Message: ");
		// scanf("%s", send_msg);
		fgets(send_msg, 1024, stdin);
		send_msg[strlen(send_msg) - 1] = '\0';

		if (strcmp(send_msg, "exit") == 0) {
			write(clnt_sock, send_msg, (int)strlen(send_msg));
			break;
		}

		write(clnt_sock, send_msg, (int)strlen(send_msg));
	}
*/
	printf("Message Receives...\n");
	nRcv = read(clnt_sock, recv_msg, sizeof(recv_msg) - 1);

	if (nRcv < 0) {
		error_handle("Receive ERROR...\n");
	}
	printf("Receive Message: %d: %s\n", nRcv, recv_msg);

	// split data set ", "
	temp = strtok(recv_msg, ", ");
	while (temp != NULL) {
	 	printf("%s\n", temp);
		temp = strtok(NULL, ", ");
	}

	close(clnt_sock);
	close(serv_sock);
	return 0;
}

void error_handle(char *msg) 
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void display_info(const char *port)
{
	char ip_addr[32];
	getIP(ip_addr);
	printf("==========================\n");
	printf("Socket Info\n");
	printf("Addr: %s\n", ip_addr);
	printf("Port: %s\n", port);
	printf("==========================\n");
}

void getIP(char *ip_buff)
{
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "ens33", IF_NAMESIZE - 1);

	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);

	sprintf(ip_buff, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}
