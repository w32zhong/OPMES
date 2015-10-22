#include <string.h>
#include <unistd.h> /* getopt */
#include "index.h"

static LIST_IT_CALLBK(_posting_write)
{
	LIST_OBJ(struct subpath, p, ln);
	P_CAST(id, CP_ID, pa_extra);
	char  path[MAX_DIR_NAME_LEN];
	FILE *fh;
	struct posting_head hd = {0, 0};
	BOOL  wr_flag = 0;

	/* make directory first */
	sprintf(path, "col/%s", p->dir);
	mkdir_p(path);

	/* wirte posting header */
//	sprintf(path, "col/%s/head.bin", p->dir);
//
//	if(!file_exists(path)) {
//		/* the file does not exist currently. */
//		fh = fopen(path, "w");
//
//		if (fh) {
//			hd.max = *id;
//			hd.min = *id;
//
//			fwrite(&hd, sizeof(struct posting_head), 1, fh);
//			fclose(fh);
//		}
//	} else {
//		fh = fopen(path, "r+");
//		if (fh) {
//			fread(&hd, sizeof(struct posting_head), 1, fh);
//
//			if (hd.max < *id) {
//				hd.max = *id;
//				wr_flag = 1;
//			} 
//			
//			if (*id < hd.min) {
//				hd.min = *id;
//				wr_flag = 1;
//			}
//
//			if (wr_flag) {
//				/* overwrite by new numbers */
//				rewind(fh);
//				fwrite(&hd, sizeof(struct posting_head), 1, fh);
//			}
//			
//			fclose(fh);
//		}
//	}

	/* wirte posting file */
	sprintf(path, "col/%s/posting.bin", p->dir);
	fh = fopen(path, "a");

	if (fh) {
		struct posting_item it;
		it.id = *id;
		it.brw = p->brw;
		fwrite(&it, sizeof(struct posting_item), 1, fh);
		fclose(fh);
	}

	LIST_GO_OVER;
}

void posting_write(struct list_it *li, CP_ID id)
{
	list_foreach(li, &_posting_write, &id);
}

BOOL index_formula(void *db_formula, CP_ID id_new, 
                   char *frml, char *url, CP_NBRW n_brw)
{
	int n_vals;
	BOOL res = 0;

	/* id_new must be new */
	n_vals = db_num_values(db_formula, &id_new, CP_ID_SZ);
	if (n_vals != 0)
		CP_FATAL;

	/* put n_brw */
	res = db_put(db_formula, &id_new, CP_ID_SZ,
	              &n_brw, sizeof(CP_NBRW)); 
	if (0 == res) {
		trace(INDEX, "DB put call failed.\n", NULL);
		return 0;
	} 

	/* put formula string */
	res = db_put(db_formula, &id_new, CP_ID_SZ,
	             frml, strlen(frml) + 1); 
	if (0 == res) {
		trace(INDEX, "DB put call failed.\n", NULL);
		return 0;
	} 

	/* put URL string */
	res = db_put(db_formula, &id_new, CP_ID_SZ,
	             url, strlen(url) + 1); 
	if (0 == res) {
		trace(INDEX, "DB put call failed.\n", NULL);
		return 0;
	} 

	return 1;
}

BOOL index_textree(char *db_textree, CP_ID id_new)
{
	BOOL  res;
	FILE *dev;
	char str_textree[MAX_TEXTREE_STR];

	dev = dev_bind(str_textree, MAX_TEXTREE_STR);
	tex_tr_print(parser_root, dev);
	fclose(dev);

	printf("tree:\n%s\n", str_textree);
	res = db_put(db_textree, &id_new, CP_ID_SZ,
	             str_textree, strlen(str_textree) + 1); 

	if (0 == res) {
		trace(INDEX, "DB put call failed.\n", NULL);
	} 

	return res;
}

BOOL index_webpage(char *db_webpage, char *url, CP_ID id_new)
{
	unsigned char hash_url[SHA1_BYTES_LEN];
	BOOL          res;
	int           n_vals;

	cp_sha1(url, strlen(url), hash_url);
	printf("URL sha-1 = %s \n", sha1_str(hash_url));

	n_vals = db_num_values(db_webpage, hash_url, SHA1_BYTES_LEN);

	if (n_vals != 0) /* just let us know */ 
		printf("same URL exists.\n");

	/* index a reverse look-up from URL */
	res = db_put(db_webpage, hash_url, SHA1_BYTES_LEN, 
	             &id_new, CP_ID_SZ);

	if (0 == res)
		trace(INDEX, "DB put call failed.\n", NULL);

	return res;
}

int main(int argc, char* argv[])
{
	char   *frml = NULL, *url = NULL;
	struct list_it li_subpath;
	void          *db_formula;
	void          *db_webpage;
	void          *db_textree;
	BOOL           bad_parse = 0;
	int            subpath_err;
	uint64_t       records;
	CP_ID          id_new;
	CP_NBRW        n_brw;
	int            c;

	/* log file initialize */
	trace_init("index.log");

	/* open key-value database files */
	db_formula = db_init(CONFIG_FORMULA_DB_NAME, DB_OMOD_WR);
	db_webpage = db_init(CONFIG_WEBPAGE_DB_NAME, DB_OMOD_WR);
	db_textree = db_init(CONFIG_TEXTREE_DB_NAME, DB_OMOD_WR);

	/* set parser_root to NULL initially */
	/* or "go to exit" might free a random address. */
	parser_root = NULL;

	/* handle program arguments */
	while ((c = getopt(argc, argv, "hf:u:")) != -1) {
		switch (c) {
		case 'h':
			printf("DESCRIPTION:\n");
			printf("index raw collection.\n");
			printf("\n");
			printf("SYNOPSIS:\n");
			printf("%s -h | "
			       "-f <formula string> "
			       "-u <url> "
			       "\n",
			       argv[0]);
			goto exit;

		case 'f':
			frml = cp_strdup(optarg);
			break;
		
		case 'u':
			url = cp_strdup(optarg);
			break;

		default:
			goto exit;
		}
	}

	if (frml == NULL || url == NULL) {
		printf("bad argument.\n");
		goto exit;
	}

	printf("index... (`" C_RED "%s" C_RST "' @ %s)\n", frml, url);

	/* before parsing, it is necessary to substitute possible
	 * carriage return(s) with space. Because a Mathjax content
	 * $ a\<^M>b $ will be interpreted as $ a\ b $, we infer a 
	 * line-feed or carriage return should be replaced by a space.
	 */
	sub_cr_space(frml);

	/* parse formula string */
	if (!cp_parse(frml))
		goto exit;

	/* handle parser error */
	if (parser_err_flg) {
		parser_err_flg = 0;
		printf("parser: %s\n", parser_err_msg);

		bad_parse = 1;
		goto exit;
	}

	/* check parser root */
	if (NULL == parser_root) {
		trace(INDEX, "NULL parser root.\n", NULL);

		bad_parse = 1;
		goto exit;
	}

	/* assign new formula ID in order */
	records = (CP_ID)db_num_records(db_formula);

	if (0 != records % 3) 
		trace(INDEX, "bad records number: %u.\n", records);

	if (records / 3 + 1 >= MAX_CP_ID) {
		trace(INDEX, "NO available ID to assign.\n", NULL);
		goto exit;
	} else {
		id_new = records / 3 + 1;
		printf("assign ID = %u\n", id_new);
	}

	/* prepare posting file */
	n_brw = tex_tr_update(parser_root);
	if (n_brw == 0) {
		trace(INDEX, "tex_tr_update() fails.\n", NULL);
		goto exit;
	}

	li_subpath = tex_tr_subpaths(parser_root, &subpath_err);
	if (subpath_err) {
		trace(INDEX, "tex_tr_subpaths() fails.\n", NULL);

		subpaths_free(&li_subpath);
		goto exit;
	}

	/* write posting file */
	posting_write(&li_subpath, id_new);
	subpaths_free(&li_subpath);

	/* index formula */
	if (db_formula) {
		if (!index_formula(db_formula, id_new, frml, url, n_brw))
			goto exit;
	} else {
		trace(INDEX, "fail to open %s for writing.\n", 
		      CONFIG_FORMULA_DB_NAME);
		goto exit;
	}

	/* index URL */
	if (db_webpage) {
		index_webpage(db_webpage, url, id_new);
	}

	/* index tree shape */
	if (db_textree) {
		index_textree(db_textree, id_new);
	}

exit:
	/* releases textree */
	if (parser_root) {
		tex_tr_release(parser_root);
		parser_root = NULL;
	}

	/* free program argument string */
	if (frml)
		cp_free(frml);

	if (url)
		cp_free(url);

	/* database release */
	if (db_formula)
		db_release(db_formula);

	if (db_webpage)
		db_release(db_webpage);

	if (db_textree)
		db_release(db_textree);

	/* unfree trace */
	if (0 != trace_unfree()) {
		for (c = 0; c < argc; c ++) {
			trace(UNFREE, "%s \n", argv[c]);
		}
		
		trace(UNFREE, "\n", NULL);
	}

	/* log release */
	trace_uninit();

	return bad_parse;
}
