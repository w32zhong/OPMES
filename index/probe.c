#include <string.h>
#include <unistd.h> /* getopt */
#include "index.h"

int main(int argc, char* argv[])
{
	void *db_formula;
	void *db_webpage;
	void *db_textree;
	int   c, e_option = 0;

	/* log file initialize */
	trace_init("probe.log");

	/* open key-value database files */
	db_formula = db_init(CONFIG_FORMULA_DB_NAME, DB_OMOD_RD);
	db_webpage = db_init(CONFIG_WEBPAGE_DB_NAME, DB_OMOD_RD);
	db_textree = db_init(CONFIG_TEXTREE_DB_NAME, DB_OMOD_RD);

	/* handle program arguments */
	while ((c = getopt(argc, argv, "hnvf:p:i:u:e:")) != -1) {
		switch (c) {
		case 'h':
			printf("DESCRIPTION:\n");
			printf("probe the index files.\n");
			printf("\n");
			printf("SYNOPSIS:\n");
			printf("%s -h | -n (database report) | "
			       "-p <posting dir> | "
			       "-f <posting file> | "
			       "-i <query id> | "
			       "-u <query url> | " 
			       "-v (print db version) | " 
			       "-e <query url>  " /* expanded request */
			       "\n",
			       argv[0]);
			printf("\n");

			goto exit;

		case 'v':
			printf("tokyocabinet version: %s\n", db_version());
			goto exit;
		
		case 'n':
			printf("size of struct brw: %ld\n", 
			       sizeof(struct brw));
			printf("size of posting item: %ld\n", 
			       sizeof(struct posting_item));
			printf("\n");

			probe_records(db_formula, db_textree, db_webpage);
			goto exit;

		case 'p':
		{
			printf("open posting @ %s ...\n", optarg);
			probe_handle_posting(optarg);
			goto exit;
		}
		
		case 'f':
		{
			printf("open posting @ %s ...\n", optarg);
			probe_print_posting(optarg);
			goto exit;
		}

		case 'i':
		{
			CP_ID id = 0;
			sscanf(optarg, "%u", &id);
			printf("query formula ID = %u\n", id);
			probe_handle_id(db_formula, db_textree, id);
			goto exit;
		}
		
		case 'e':
		{
			e_option = 1;
			goto option_u;
		}

		case 'u':
option_u:
		{
			unsigned char hash_url[SHA1_BYTES_LEN];
			char *url = optarg;
			cp_sha1(url, strlen(url), hash_url);
			printf("query URL = %s\n", url);
			printf("hash URL = %s\n", sha1_str(hash_url));
			if (!probe_handle_url(db_formula, db_textree, 
			                      db_webpage, e_option, hash_url))
				printf("this URL is not indexed.\n");
			goto exit;
		}

		default:
			goto exit;
		}
	}

exit:
	/* database release */
	if (db_formula)
		db_release(db_formula);

	if (db_webpage)
		db_release(db_webpage);

	if (db_textree)
		db_release(db_textree);

	/* log release */
	trace_uninit();

	return 0;
}
