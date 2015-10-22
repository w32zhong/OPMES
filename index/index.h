#include "common.h"
#include "parser.h"
#include "index-util.h"
#include "db-wraper.h"

struct posting_head {
	CP_ID max, min;
};

struct posting_item {
	CP_ID       id;
	struct brw brw;
};
