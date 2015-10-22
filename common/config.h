/* trace */
#define TRACE_ENABLE                  1
#define TRACE_ENABLE_STDERR           1
#define TRACE_ENABLE_LOG              1
#define TRACE_STDERR_ALL              0
#define TRACE_LOG_ALL                 0

#define TRACE_STDERR_FATAL            1
#define TRACE_LOG_FATAL               1

#define TRACE_STDERR_UNFREE           1
#define TRACE_LOG_UNFREE              1

#define TRACE_STDERR_TEST             1
#define TRACE_LOG_TEST                1

#define TRACE_STDERR_YYERR            1
#define TRACE_LOG_YYERR               1

#define TRACE_STDERR_LIMIT            1
#define TRACE_LOG_LIMIT               1

#define TRACE_STDERR_YYDEBUG          0
#define TRACE_LOG_YYDEBUG             0

#define TRACE_STDERR_INDEX            1
#define TRACE_LOG_INDEX               1

#define TRACE_STDERR_MERGE_SEARCH     0
#define TRACE_LOG_MERGE_SEARCH        0

#define TRACE_STDERR_RANK_ERR         0
#define TRACE_LOG_RANK_ERR            1

#define TRACE_STDERR_WEB              0
#define TRACE_LOG_WEB                 1

#define TRACE_STDERR_SEARCH_TIME      1
#define TRACE_LOG_SEARCH_TIME         1

/* limits */
#define MAX_CP_ID                     UINT32_MAX
#define MAX_TRACE_FILE_NAME           32
#define MAX_TEX_TR_DEPTH              21
#define MAX_TEX_TR_LEAVES             256
#define MAX_TEX_TR_VARS               MAX_TEX_TR_LEAVES
#define MAX_SUBPATH_DIR_NAME          (64 * MAX_TEX_TR_DEPTH)
#define MAX_COL_PATH_NAME_LEN         256
#define MAX_BRW_SYMBOL_ID_SZ          UINT16_MAX
#define MAX_BRW_PIN_SZ                UINT8_MAX
#define MAX_BRW_FAN_SZ                UINT8_MAX
#define MAX_FLOAT_NUM_LEN             24
#define MAX_PARSER_ERR_STR            1024
#define MAX_DIR_NAME_LEN              MAX_SUBPATH_DIR_NAME 
#define MAX_TEXTREE_STR               65536
#define MAX_SEARCH_ITEMS              650000 //1054321
#define MAX_WEB_QUERY_STR_LEN         4096 

/* debug */
#define DEBUG_TEX_TR_PRINT_ID         0
#define DEBUG_POS_MERGE               0
#define DEBUG_SCORE_BRW               0
#define DEBUG_SCORING                 0
#define DEBUG_HEADPRUNE_PRINT         1
#define DEBUG_HEAP_WHEN_SORT          0

/* other configurations */
#define CONFIG_FORMULA_DB_NAME        "formula.db"
#define CONFIG_WEBPAGE_DB_NAME        "webpage.db"
#define CONFIG_TEXTREE_DB_NAME        "textree.db"
#define CONFIG_NO_SCORE_PROCESS       0

#define BRWBLK_RD_NUM                 4096
#define STOP_MERG_ND_RK_ITEMS         30
#define DEFAULT_RES_PER_PAGE          30

#define DEFAULT_COL_PATH              "./col"
#define DEFAULT_RANKING_ITEMS         200
#define DEFAULT_EN_MERGES_ND_SE       1
#define DEFAULT_EN_HEADPRUNE          0
#define DEFAULT_EN_THOROUGH_SE        1
