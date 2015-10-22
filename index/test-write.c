#include "index.h"
#include <string.h>

int main(int argc, char* argv[])
{
	void *db_test = db_init("test.db", DB_OMOD_WR);
	CP_ID id_test = 123;
	char test_str[] = "if you see this, then the db works.";
	BOOL res;

	if (NULL == db_test) {
		fprintf(stderr, "cannot open test.db!\n");
		return 0;
	}

	res = db_put(db_test, &id_test, CP_ID_SZ, test_str,
	             strlen(test_str) + 1); 

	if (0 == res)
		fprintf(stderr, "db_put() fails.\n");
	else
		fprintf(stderr, "db_put() successful.\n");

	if (db_test)
		db_release(db_test);

	return 0;
}
