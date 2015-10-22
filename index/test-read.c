#include "index.h"

int main(int argc, char* argv[])
{
	void *db_test = db_init("test.db", DB_OMOD_RD);
	CP_ID id_test = 123;

	if (NULL == db_test) {
		fprintf(stderr, "cannot open test.db!\n");
		return 0;
	}

	probe_print_frml_textree(db_test, id_test);

	if (db_test)
		db_release(db_test);

	return 0;
}
