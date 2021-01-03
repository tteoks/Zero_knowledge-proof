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

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;
	const char *serv_port = argv[1];

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1) {
		error_handle("Socket ERROR!");
	}

	if(argc != 2) {
		error_handle("Invalid arguments!");
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
	if(clnt_sock == -1) {
		error_handle("Accept ERROR!");
	}

	char msg[] = "Hello This is TEST\n";
	write(clnt_sock, msg, sizeof(msg));

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
	strncpy(ifr.ifr_name, "eth0", IF_NAMESIZE - 1);

	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);

	sprintf(ip_buff, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}
