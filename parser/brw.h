struct brw {
	uint16_t  symbol_id;
	uint8_t   pin[MAX_TEX_TR_DEPTH];
	/* first element fan[0] is group members */
	uint8_t   fan[MAX_TEX_TR_DEPTH]; 
};

struct subpath {
	char             dir[MAX_SUBPATH_DIR_NAME];
	struct brw       brw;
	struct list_node ln;
};
