#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include"database.h"
#include"crypto.h"

void error_handle(char *msg);
void Usage();

int main(int argc, char *argv[])
{
	int clnt_sock;
	int i = 0, j = 0, num = 0, nRcv = 0;
	const char *serv_ip = argv[1];
	const char *serv_port = argv[2];
	struct sockaddr_in serv_addr;
	char recv_msg[65535] = {0x00, };
	char send_msg[16384] = {0x00, };
	char infectees_sha[4096][SHA256_DIGEST_LENGTH*2 + 1] = {0x00, };
	char client_result[4096][256] = {0x00, };
	char *temp;
	char select;

	// For database
	MYSQL *conn = NULL;
	MYSQL_RES *sql_res = NULL;
	MYSQL_ROW  sql_row = NULL;
	char query[1024];

	// For crypto
	unsigned char md_ary[4096][SHA256_DIGEST_LENGTH] = {0x00};
	char hashstr_ary[4096][SHA256_DIGEST_LENGTH*2 + 1] = {0x00};

	if(argc != 3) {
		Usage();
		error_handle("");
	}
	
	// Create Client socket
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
		printf("%s Connection Complete!\n\n", inet_ntoa(serv_addr.sin_addr));
	}

	printf("Do you want send data to server?[Y/N]: ");
	scanf("%c", &select); 

	if (select == 'Y' || select == 'y') {
		printf("Start..\n");
	} else if (select == 'N' || select == 'n') {
		error_handle("Your answer is NO\n Exist");
	} else {
		error_handle("Unkown selection\n Exist");
	}

	// Database logic
	printf("Connect Database...\n");		
	db_connect(&conn, HOST, USER, PASS, NAME);
	printf("Success database connection!\n\n");
	
	sprintf(query, "SELECT VALUE FROM %s", TABLE);
	
	printf("Try query!: %s\n", query);
	db_select(&conn, &sql_res, query);
	printf("Success query!\n\n");

	// SHA256 logic
	printf("Start Hashing...\n");
	while (sql_row = mysql_fetch_row(sql_res)) {
		snprintf(client_result[num], strlen(sql_row[0]), "%s", sql_row[0]);
		if(!simpleSHA256(sql_row[0], strlen(sql_row[0]), 
					md_ary[num++])) {
		error_handle("ERROR in simpleSHA256");	
		}
	}

	if(client_result[0][0] == '\0') {
		printf("Query Result is Zero\nExit...\n");
		write(clnt_sock, "bye", 3);
		close(clnt_sock);
		return 0;
	}

	for(i=0; i<num; i++) {
		trans_string(md_ary[i], hashstr_ary[i]);
	}
	printf("Success hashing\n\n");
	

	printf("Waiting Server ready\n...");
	nRcv = read(clnt_sock, recv_msg, sizeof(recv_msg) - 1);
	if (nRcv < 0) {
		printf("Receive ERROR!\n");
		exit(1);
	}
	recv_msg[nRcv] = '\0';

	if(strcmp(recv_msg, "ready ok") == 0) {
		printf("Server get ready, Start!\n");
	} else {
		printf("Unknown command from Server!\n");
		close(clnt_sock);
		printf("Close connection...\n");
		exit(1);
	}
	
	i = 0, j = 0;
	// Sending hashstr, each num of sending data is 250
	for (j=0; j<num/200; j++) {
		memset(send_msg, 0x00, sizeof(send_msg)*sizeof(char));
		nRcv = 0;
		strncat(send_msg, hashstr_ary[200*j], strlen(hashstr_ary[200*j]));
		for(i=200 * j + 1; i<200 * (j+1); i++) {
			strcat(send_msg, ", ");
			strncat(send_msg, hashstr_ary[i], strlen(hashstr_ary[i]));
		}
		
		write(clnt_sock, send_msg, (int)strlen(send_msg));
		printf("Send Hash: %d\n", i);
		nRcv = read(clnt_sock, recv_msg, sizeof(recv_msg) - 1);
		if (nRcv < 0) {
			printf("Receive ERROR!\n");
			break;
		}
		recv_msg[nRcv] = '\0';

		if(strcmp(recv_msg, "ok") == 0) {
			printf("Continue...\n");
		} else {
			printf("Unknown command from Server!\n");
			close(clnt_sock);
			printf("Close connection...\n");
			exit(1);
		}
	}
	
	// Sending rest hash str
	memset(send_msg, 0x00, sizeof(send_msg));
	strncat(send_msg, hashstr_ary[200*j], strlen(hashstr_ary[200*j]));
	for (j=1; j<num%200; j++) {
		strcat(send_msg, ", ");
		strncat(send_msg, hashstr_ary[i + j], strlen(hashstr_ary[i+j]));
	}

	if(send_msg[0] != 0x00) {
		write(clnt_sock, send_msg, (int)strlen(send_msg));
		nRcv = read(clnt_sock, recv_msg, sizeof(recv_msg) - 1);
		recv_msg[nRcv] = '\0';
	}
	
	if(strcmp(recv_msg, "ok") == 0) {
		write(clnt_sock, "end", 3);
	} else {
		printf("Unknown command from Server!\n");
		close(clnt_sock);
		printf("Close connection...\n");
		exit(1);
	}
	printf("Success Send all data\n\n");

	printf("Waiting result...\n");

	int idx = 0;
	while(1) {
		nRcv = read(clnt_sock, recv_msg, sizeof(recv_msg) - 1);
		recv_msg[nRcv] = '\0';
		printf("Receive Infectee data...\n");

		if (nRcv < 0) {
			printf("NO Infectee!!\n");
			close(clnt_sock);
			return 0;
		}
		if(strcmp(recv_msg, "end") == 0) {
			printf("Success get result\n");
			break;
		}

		temp = strtok(recv_msg, ", ");
		while(temp != NULL) {
			snprintf(infectees_sha[idx], 65, "%s", temp);
			infectees_sha[idx++][65] = '\0';
			temp = strtok(NULL, ", ");
		}
		
		printf("Send OK\n");
		write(clnt_sock, "ok", 2);
	}
	
	if(recv_msg[0] == '\0') {
			printf("No infectee!\n");
			return 0;
	}


	printf("-------------------------------------\n");
	printf("#         Infectee's values         #\n");
	printf("-------------------------------------\n");

	// Compare hash
	for (i=0; i<num; i++) {
		for (j=0; j<idx; j++) {
			if(strcmp(hashstr_ary[i], infectees_sha[j]) == 0) {
				printf("no.%04d %s\n", i+1, client_result[i]);
			} 
		}
	}
	
	printf("-------------------------------------\n");

	printf("Total Infectees: %d\n\n\n", idx);
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

void Usage() {
	printf("Usage: \n");
	printf("./client <serv_ip> <serv_port>\n");
}
