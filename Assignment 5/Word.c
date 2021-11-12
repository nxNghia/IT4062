#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Word.h"

void addWord(char* _word, struct Word** wL)
{
	struct Word* new_word = (struct Word*)calloc(1, sizeof(struct Word));
	new_word -> word = (char*)calloc(strlen(_word) + 1, sizeof(char));
	new_word -> next = NULL;
	strcpy(new_word -> word, _word);
	if(*wL == NULL)
	{
		*wL = new_word;
	}else{
		struct Word* cur = *wL;
		struct Word* previous = NULL;
		while(cur != NULL)
		{
			previous = cur;
			cur = cur -> next;
		}

		previous -> next = new_word;
	}
}

int sizeOfWords(struct Word* wL)
{
	int size = 0;
	while(wL != NULL)
	{
		++size;
		wL = wL -> next;
	}

	return size;
}

void printWords(struct Word* wL)
{
	int i = 0;
	while(wL != NULL)
	{
		printf("%d. %s\n", i++,  wL -> word);
		wL = wL -> next;
	}
}

struct Word* getAt(int index, struct Word* wL)
{
	int _index = 0;
	struct Word* cur = wL;

	while(1)
	{
		if(index == _index || cur == NULL)
			break;
		++_index;
		cur = cur -> next;
	}

	if(cur == NULL)
	{
		struct Word* temp = (struct Word*)malloc(1 * sizeof(struct Word*));
		temp -> word = (char*)calloc(2, sizeof(char));
		strcpy(temp -> word, "NA");
		return temp;
	}

	return cur;
}

struct Word* split(char* str)
{
	struct Word* res = NULL;

	int i = 0;

	for (i = strlen(str) - 1; i >= 0; --i)
	{
		if(str[i] == '?')
		{
			addWord(str + i + 1, &res);
			str[i] = '\0';
		}
	}

	return res;
}