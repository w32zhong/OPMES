#include <stdio.h>

int main(void)
{
	printf("Content-type: text/html\n\n");
	printf("hello CGI! <br/>");
	printf("<img src=\"/opmes/mathman.png\"/>");
	printf("\n");
	return 0;
}
