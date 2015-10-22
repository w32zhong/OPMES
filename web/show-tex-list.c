#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <curl/curl.h>
#include <libgen.h>
#include "search.h"

#define STR(_num) # _num
#define STR_FMT(_num) "%" STR(_num) "s"

static void cat(char *file)
{
	char path[1024];
	size_t len = 0;
	char *line = NULL;
	FILE *f;
	
	sprintf(path, "cat/%s", file);
	f = fopen(path, "r");

	if (f == NULL)
		return;

	while (getline(&line, &len, f) != -1)
		printf("%s", line);

	free(line);
	fclose(f);
}

void echo_li(char *doc_tex, char *url, char *info)
{
	if (doc_tex && url && info)
	printf("<li><table border=\"0\" class=\"rank_item\">"
	       "<tr><td><a target=\"_blank\" href=\"%s\">"
	       "<h3>%s</h3></a>"
	       "<span style=\"color:green;\">%s</span>"
	       "</td><td>[dmath]%s[/dmath]"
	       "<span style=\"color:white;\">"
	       "%s</span></td></tr></table></li>",
	       url, doc_tex, url, doc_tex, info);
}

void echo_tex_li(const char *path)
{
	FILE *fh;
	fh = fopen(path, "r");
	
	if (fh == NULL) {
		printf("<li>specified file does not exist</li>");
		return;
	}

	char buf[4096];
	char url[1024];
	char info[1024];
	url[0] = '\0';
	fgets (buf, sizeof(buf), fh); /* skip the first line */
	int i = 0;
	while (fgets (buf, sizeof(buf), fh)) {
		switch (i) {
		case 0:
			strcpy(info, buf);
			break;
		case 1:
			strcpy(url, buf);
			break;
		case 2:
			echo_li(buf, url, info);
			break;
		default:
			printf("<li>unexpected result file format</li>\n");
			break;
		}

		i = (i + 1) % 3;
	}

	fclose(fh);
}

char *first_line(const char *path)
{
	static char buf[4096];
	FILE *fh;
	fh = fopen(path, "r");
	
	if (fh == NULL) {
		return "<li>specified file does not exist</li>";
	}

	fgets (buf, sizeof(buf), fh);
	fclose(fh);

	return buf;
}

int main()
{
	char     *env_input;
	char     path[4096];

	/* CGI initial print */
	printf("Content-type: text/html\n\n");

	/* create trace/log */
	trace_init("list.log");

	/* get GET content from environment variable */ 
	env_input = getenv("QUERY_STRING");
	if (env_input == NULL) {
		trace(WEB, "No env variable: QUERY_STRING.\n", NULL);
		goto exit;
	}

	/* extract page number from GET content */
	sscanf(env_input, "p=%s", path);

	/* echo HTML */
	cat("head.cat");
	printf("file: %s <br/>", basename(path));
	printf("[dmath]%s[/dmath]", first_line(path));
	cat("neck.cat");

	echo_tex_li(path);
	
	cat("ass.cat");
	cat("tail.cat");

exit:
	/* delete trace/log */
	trace_unfree();
	trace_uninit();
	return 0;
}
