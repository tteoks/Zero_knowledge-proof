#include "crypto.h"

bool simpleSHA256(void *input, unsigned long length, unsigned char *hash) {
	SHA256_CTX sha256;
	if(!SHA256_Init(&sha256)) {
		return false;
	}

	if(!SHA256_Update(&sha256, (unsigned char *)input, length)) {
		return false;
	}

	if(!SHA256_Final(hash, &sha256)) {
		return false;
	}

	return true;
	
}

void trans_string(unsigned char hash[SHA256_DIGEST_LENGTH], char *output)
{
	int i;
	for(i=0; i<SHA256_DIGEST_LENGTH; i++) {
		sprintf(output + (i*2), "%02x", hash[i]);
	}
}
/*
int main()
{
	unsigned char data[256];
	unsigned char md[SHA256_DIGEST_LENGTH];
	unsigned char hashstr[SHA256_DIGEST_LENGTH];
	memset(data, 0x00, sizeof(data));
	memset(md, 0x00, sizeof(md));
	memset(hashstr, 0x00, sizeof(hashstr));

	sprintf(data, "%s", "Hi My name is yujinho\nThis is just test Thank!");

	if(!simpleSHA256(data, strlen(data),  md)) {
		fprintf(stderr, "%s", "ERROR in simpleSHA256\n");
	}

	printf("## Data ##\n%s\n", data);
	printf("=========\n");

	trans_string(md, hashstr);

	printf("HASH: %s\n", hashstr);
}
*/
