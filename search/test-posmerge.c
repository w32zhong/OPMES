#include "search.h"
#define PS_IT_SZ sizeof(struct posting_item)

void test_write_rand_posting(const char *fname)
{
	FILE *fh = fopen(fname, "w");
	struct posting_item item;
	int i, delta;

	if (fh == NULL)
		return;

	item.id = 0;
	delta = rand() % 50000;
	for (i = 0; i < 20000 + delta; i++) {
		item.id += rand() % 50;
		fwrite(&item, PS_IT_SZ, 1, fh);
	}
	
	fclose(fh);
}

void test_write_posting0()
{
	FILE *fh = fopen("test-posting-0.bin", "w");
	struct posting_item item;
	if (fh == NULL)
		return;

	item.id = 1; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 2; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 3; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 4; fwrite(&item, PS_IT_SZ, 1, fh);

	item.id = 5; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 6; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 7; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 8; fwrite(&item, PS_IT_SZ, 1, fh);

	item.id = 9; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 9; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 9; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 10; fwrite(&item, PS_IT_SZ, 1, fh);
	
	item.id = 12; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 15; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 18; fwrite(&item, PS_IT_SZ, 1, fh);
	
	fclose(fh);
}

void test_write_posting1()
{
	FILE *fh = fopen("test-posting-1.bin", "w");
	struct posting_item item;
	if (fh == NULL)
		return;

	item.id = 1; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 2; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 9; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 14; fwrite(&item, PS_IT_SZ, 1, fh);

	item.id = 15; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 15; fwrite(&item, PS_IT_SZ, 1, fh);
	item.id = 18; fwrite(&item, PS_IT_SZ, 1, fh);
	fclose(fh);
}

BOOL test_callbk(uint32_t pipe, CP_ID id, 
                 struct brw *brw, void* args)
{
	if (brw != NULL) {
		printf(C_GRAY "[%d] @ pipe %d calls" C_RST "\n", id, pipe);
		printf("{ ");
		print_brw(brw, stdout);
		printf(" } arg=`%s'", (char*)args);
		printf("\n");
	}

	return 0; /* continue to search */
}

void test_rand_data()
{
	char fname[][MAX_SUBPATH_DIR_NAME] = {
	                    "test-posting-0.bin", 
	                    "test-posting-1.bin", 
	                    "test-posting-2.bin"};
	trace_init("test-posmerge.log");

	srand(3875);
	test_write_rand_posting("test-posting-0.bin");
	test_write_rand_posting("test-posting-1.bin");
	test_write_rand_posting("test-posting-2.bin");

	posmerge(fname, 3, &test_callbk, "test");
}

void test_fixed_data()
{
	char fname[][MAX_SUBPATH_DIR_NAME] = {
	                    "test-posting-0.bin", 
	                    "test-posting-1.bin"};

	test_write_posting0();
	test_write_posting1();

	probe_print_posting("test-posting-0.bin");
	printf("-------\n");
	probe_print_posting("test-posting-1.bin");

	posmerge(fname, 2, &test_callbk, "test");
}

int main()
{
	trace_init("test-posmerge.log");

	posmerge_print_rd_num();

#if 0
	test_rand_data();
#else
	test_fixed_data();
#endif

	trace_unfree();
	trace_uninit();
	return 0;
}
