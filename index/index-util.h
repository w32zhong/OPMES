#include <stdio.h>

void cp_sha1(void*, int, void*);
void cp_sha1_pr(void*, FILE*);

FILE *dev_bind(void*, size_t);
char *sha1_str(unsigned char*);

void mkdir_p(const char*); 
BOOL file_exists(char *path);

#include <openssl/sha.h>

#define SHA1_BYTES_LEN SHA_DIGEST_LENGTH
#define SHA1_PRINT_BUF_LEN (SHA1_BYTES_LEN * 2 + 1)

BOOL sub_cr_space(char*);
