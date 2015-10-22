#include "common.h"
#include "index-util.h"

void cp_sha1(void *src, int src_sz, void *dest)
{
	SHA1((uchar*)src, src_sz, (uchar*)dest);
}

void cp_sha1_pr(void *sha1, FILE *fh)
{
	int i;
	for (i = 0; i < SHA1_BYTES_LEN; i++)
		fprintf(fh, "%02x", ((uchar*)sha1)[i]);
}

char *sha1_str(unsigned char *hash)
{
	static char hash_str[SHA1_PRINT_BUF_LEN];
	FILE *dev;
	dev = dev_bind(hash_str, SHA1_PRINT_BUF_LEN);
	cp_sha1_pr(hash, dev);
	fclose(dev);

	if (hash_str[SHA1_PRINT_BUF_LEN - 1] != '\0')
		CP_FATAL;
	return hash_str;
}

#include <stdio.h>

FILE *dev_bind(void *buf, size_t sz)
{
	return fmemopen(buf, sz, "w");
}

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void mkdir_p(const char *path) 
{
	char dir[MAX_DIR_NAME_LEN];
	char *p = NULL;
	size_t len;

	sprintf(dir, "%s", path);
	len = strlen(dir);

	if(dir[len - 1] == '/')
		dir[len - 1] = '\0';

	for(p = dir + 1; *p; p++)
		if(*p == '/') {
			*p = '\0';
			mkdir(dir, S_IRWXU);
			*p = '/';
		}

	mkdir(dir, S_IRWXU);
}

BOOL file_exists(char *path)
{
	return (access(path, F_OK) != -1);
}

/* replace carrige return into space */
BOOL sub_cr_space(char *str)
{
	BOOL ret = 0;
	while (*str != '\0') {
		if (*str == '\r') {
			ret ++;
			*str = ' ';
		}

		str ++;
	}

	return ret;
}
