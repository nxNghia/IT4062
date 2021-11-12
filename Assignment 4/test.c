#include <stdio.h>
#include <string.h>

int main()
{
	char str[50] = "";

	str[strlen(str)] = '\n';
	str[strlen(str) + 1] = '\0';

	printf("%s|\n", str);
}