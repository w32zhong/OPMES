enum {
	DB_OMOD_WR,
	DB_OMOD_RD
};

void *db_init(const char*, int);
void  db_release(void*);
void  db_sync(void*);

BOOL  db_put(void*, void*, int, void*, int); 

uint64_t    db_num_records(void*);
int         db_num_values(void*, void*, int);
const char *db_last_err(void*);

void probe_print_frml_textree(void*, CP_ID);
void probe_handle_id(void*, void*, CP_ID);
BOOL probe_handle_url(void*, void*, void*, 
                      BOOL, unsigned char hash_url[]);
void probe_handle_posting(char*);
void probe_records(void*, void*, void*);
void probe_print_posting(char *);

/* formula record */
struct frml_rec {
	int   n_brw;
	char *frml_str;
	char *url_str;
};

BOOL db_get_formula_record(void*, CP_ID, struct frml_rec*);

const char *db_version();
