#include <tcutil.h>
#include <tcbdb.h>
#include "index.h"

void *db_init(const char *name, int mod)
{
	TCBDB *bdb = tcbdbnew();

	if (mod == DB_OMOD_WR) {
		if (!tcbdbopen(bdb, name, BDBOCREAT | BDBOWRITER)) 
			return NULL;
	} else if (mod == DB_OMOD_RD) {
		if (!tcbdbopen(bdb, name, BDBOREADER)) 
			return NULL;
	}

	return bdb;
}

void db_release(void *db)
{
	tcbdbclose((TCBDB*)db);
	tcbdbdel((TCBDB*)db);
}

uint64_t db_num_records(void *db)
{
	return tcbdbrnum((TCBDB*)db);
}

void db_sync(void *db)
{
	tcbdbsync((TCBDB*)db);
}

const char *db_last_err(void *db)
{
	return tcbdberrmsg(tcbdbecode(db));
}

BOOL 
db_put(void *db, void *key, int key_sz, void *val, int val_sz) 
{
	return tcbdbputdup((TCBDB*)db, key, key_sz, val, val_sz);
}

int db_num_values(void *db, void *key, int key_sz)
{
	return tcbdbvnum((TCBDB*)db, key, key_sz);
}

#include <string.h>

BOOL db_get_formula_record(void *db_formula, CP_ID id, 
                           struct frml_rec *frml_rec)
{
	BOOL ret;
	if (db_formula == NULL)
		return 1;
	
	TCLIST *li;
	li = tcbdbget4((TCBDB*)db_formula, &id, sizeof(CP_ID));

	if (li) {
		if (tclistnum(li) == 3) {
			int sz;
			int *val = (int *)tclistval(li, 0, &sz);

			frml_rec->n_brw = *val;
			frml_rec->frml_str = cp_strdup(tclistval2(li, 1));
			frml_rec->url_str = cp_strdup(tclistval2(li, 2));

			ret = 0;
		} else {
			ret = 1;
		}

		tclistdel(li);
		return ret;
	} else {
		return 1;
	}
}

void probe_print_frml_textree(void *db_textree, CP_ID id)
{
	int   sz;
	char *str;
	if (db_textree == NULL)
		return;

	str = tcbdbget((TCBDB*)db_textree, &id, sizeof(CP_ID), 
	               &sz);
	if (str) {
		fprintf(stderr, "tree:\n%s\n", str);
	} else {
		fprintf(stderr, "No tree string with ID=%u found.\n", 
				id);
	}
}

#include <stdio.h>

void probe_handle_id(void *db_formula, void *db_textree, CP_ID id)
{
	if (db_formula != NULL) {
		TCLIST *li;
		li = tcbdbget4((TCBDB*)db_formula, &id, sizeof(CP_ID));

		if (li) {
			if (tclistnum(li) == 3) {
				int sz;
				int *val = (int *)tclistval(li, 0, &sz);
				printf("brw number: %d\n", *val);
				printf("formula: \n%s\n", tclistval2(li, 1));
				printf("url: %s\n", tclistval2(li, 2));
			} else {
				printf("unexpected records associated.\n");
			}

			tclistdel(li);
		} else {
			printf("No formula with ID=%u found.\n", id);
		}
	}
	
	probe_print_frml_textree(db_textree, id);
}

BOOL probe_handle_url(void *db_formula, void *db_textree, 
                      void *db_webpage, BOOL expand,
                      unsigned char hash_url[])
{
	TCLIST *li;
	int i;

	if (NULL == db_webpage) {
		printf("cannot open %s.\n", CONFIG_WEBPAGE_DB_NAME);
		return 0;
	}

	li = tcbdbget4((TCBDB*)db_webpage, hash_url, SHA1_BYTES_LEN);

	if (li) {
		printf("all indexed on this URL:\n");
		for (i = 0; i < tclistnum(li); i++) {
			int sz;
			int *val = (int *)tclistval(li, i, &sz);
			printf("id = %d\n", *val);

			if (expand) {
				probe_handle_id(db_formula, db_textree, *val);
			}
		}

		tclistdel(li);
		return 1;
	} else {
		return 0;
	}
}

void probe_print_posting(char *path)
{
	FILE *fh;
	struct posting_item it;
	fh = fopen(path, "r");

	if (NULL == fh) {
		printf("posting file not found.\n");
		return;
	}

	while (fread(&it, sizeof(struct posting_item), 1, fh) == 1) {
		printf("ID = %u; ", it.id);
		print_brw(&it.brw, stdout);
		printf("\n");
	}

	fclose(fh);
}

void probe_handle_posting(char *dir)
{
	struct posting_head hd;
	char path[MAX_DIR_NAME_LEN];
	FILE *fh;

	sprintf(path, "%s/head.bin", dir);
	fh = fopen(path, "r");

	if (NULL == fh) {
		printf("posting head not found.\n");
		goto probe_posting;
	}

	fread(&hd, sizeof(struct posting_head), 1, fh);
	printf("max: %u, min: %u.\n", hd.max, hd.min);

	fclose(fh);

probe_posting:
	sprintf(path, "%s/posting.bin", dir);
	probe_print_posting(path);
	
}

#include <stdint.h>
#include <inttypes.h>

void probe_records(void *db_formula, void *db_textree, 
                   void *db_webpage)
{
	if (db_formula)
		trace(INDEX, "%" PRIu64 " records in %s.\n", 
		      db_num_records(db_formula), CONFIG_FORMULA_DB_NAME);

	if (db_webpage)
		trace(INDEX, "%" PRIu64 " records in %s.\n", 
		      db_num_records(db_webpage), CONFIG_WEBPAGE_DB_NAME);

	if (db_textree)
		trace(INDEX, "%" PRIu64 " records in %s.\n", 
		      db_num_records(db_textree), CONFIG_TEXTREE_DB_NAME);
}

const char *db_version()
{
	return tcversion;
}
