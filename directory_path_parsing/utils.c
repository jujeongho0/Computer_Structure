#define _CRT_SECURE_NO_WARNINGS
#include "dir_file.h"
#include "utils.h"


void open_file(char* fname, FILE** input) {
	assert(fname != NULL);

	if (access(fname, 00) == -1) {
		fprintf(stderr, "The '%s' file does not exists\n",  fname);
		exit(1);
	}

	*input = fopen(fname, "r");

	if (input == NULL) {
		fprintf(stderr, "The '%s' file did not open\n", fname);
		exit(1);
	}

}

// This parse_str_to_list() split string to the tokens, and put the tokens in token_list.
// The value of return is the number of tokens.

int parse_str_to_list(const char* str, char*** token_list) {

	
	
	char** arr;
	arr = (char**)malloc(sizeof(char*) * 1024);
	memset(arr, 0, sizeof(char*) * 1024);

	
	int a = -1;
	int b = 0;
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] == '/')
		{
			a++;
			b = 0;
			arr[a] = (char*)malloc(sizeof(char) * 1024);
			memset(arr[a], 0, sizeof(char) * 1024);
		}
		if(str[i] == '\n')
		{
			break;
		}
		if(str[i] != '/')
		{
			arr[a][b] = str[i];
			b++;
		}
	}
	if (strlen(str) > 2)
	{
		if ((str[strlen(str) - 2] != '/') && (str[strlen(str) - 1] == '\n'))
			a += 1;
	}
	
	
	
	

	*token_list = arr;
	for(int i=0;i<a;i++)
		

	return a;
}

void free_token_list(char** token_list, int num_token) {
	int index;

	for (index = 0; index < num_token; index++) {
		free(token_list[index]);
	}

	free(token_list);
}

