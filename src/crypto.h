#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<openssl/sha.h>

bool simpleSHA256(void *input, unsigned long length, 
					unsigned char *hash);
void trans_string(unsigned char hash[SHA256_DIGEST_LENGTH], 
					char *output);
