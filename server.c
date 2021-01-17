#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<net/if.h>
#include<netinet/in.h>
#include<sys/ioctl.h>
#include"database.h"
#include"crypto.h"



void error_handle(char *msg);
void display_info(const char *serv_port);
void getIP(char *ip_buff);
void display_usage() {
	printf("Usage\n");
	printf("./server <serv_port>\n");
}


int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock, nRcv, end;
	int i=0, j=0, num=0, idx=0;
	const char *serv_port = argv[1];
	char send_msg[65535] = { 0x00, };
	char recv_msg[65535] = { 0x00, };

	char get_hashstr_ary[4096][SHA256_DIGEST_LENGTH*2 + 1] = {0x00, };
	char result_list[4096][SHA256_DIGEST_LENGTH*2 + 1] = {0x00, };
	char *start, *temp;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	
	// Start data Compare
	MYSQL *conn = NULL;
	MYSQL_RES *sql_res = NULL;
	MYSQL_ROW sql_row = NULL;
	char query[1024];

	unsigned char md_ary[4096][SHA256_DIGEST_LENGTH] = {0x00};
	char cmp_hashstr_ary[4096][SHA256_DIGEST_LENGTH*2 + 1] = {0x00};


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

	// DATABASE
	printf("Connect Database...\n");
	db_connect(&conn, HOST, USER, PASS, NAME);
	printf("Success database connection!\n\n");

	sprintf(query, "SELECT VALUE FROM %s", TABLE);

	printf("Try query!: %s\n", query);
	db_select(&conn, &sql_res, query);
	printf("Success query!\n\n");


	// SHA256
	printf("Start Hashing...\n");
	while (sql_row = mysql_fetch_row(sql_res)) {
		if (!simpleSHA256(sql_row[0], strlen(sql_row[0]),
					md_ary[num++])) {
			error_handle("ERROR in simpleSHA256");
		}
	}
	
	for(i=0; i<num; i++) {
		trans_string(md_ary[i], cmp_hashstr_ary[i]);
	}
	printf("Success Hashing\n\n");

	write(clnt_sock, "ready ok", 8);
	printf("Sending Read ok\n");


	printf("Message Receives...\n");
	idx = 0;
	memset(get_hashstr_ary, 0x00, sizeof(get_hashstr_ary));
	while(1) {
		nRcv = read(clnt_sock, recv_msg, sizeof(recv_msg) - 1);

		if (nRcv < 0) {
			error_handle("Receive ERROR...\n");
		}
		recv_msg[nRcv] = '\0';
		if(strcmp(recv_msg, "end") == 0) {
			printf("END of data recieve...\n");
			break;
		}
		
		// split data set ", "
		temp = strtok(recv_msg, ", ");
		while (temp != NULL) {
//			printf("[%d] %s\n", idx, temp);
			snprintf(get_hashstr_ary[idx], 65, "%s", temp);
			get_hashstr_ary[idx++][65] = '\0';
			temp = strtok(NULL, ", ");
		}
		
		
		printf("Sending ok\n\n");
		write(clnt_sock, "ok", 2);
	}

	printf("total %d\n", idx-1);
	printf("Success recieve all data set!\n");
	
	printf("Find Infectee...\n");
	int find = 0;;
	memset(send_msg, 0x00, sizeof(send_msg));
	for(i=0; i<idx; i++) {
		for(j=0; j<num; j++) {
			if(strcmp(get_hashstr_ary[i], cmp_hashstr_ary[j]) == 0) {
				if (find == 0) {
					strncat(send_msg, get_hashstr_ary[i], strlen(get_hashstr_ary[i]));
				} else {
					strcat(send_msg, ", ");	
					strncat(send_msg, get_hashstr_ary[i], strlen(get_hashstr_ary[i]));
				}
				find++;
			}
		}
	}
	if(send_msg[0] == '\0') {
		printf("No Infectee in airline\n");
		send_msg[0] = '\0';
	} else {
		printf("Find Infectee: %d\n", find);
		send_msg[64 + (find-1) * 66] = '\0';
		
	}
	write(clnt_sock, send_msg, strlen(send_msg));
	printf("Sending result and close... \n\n");

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
