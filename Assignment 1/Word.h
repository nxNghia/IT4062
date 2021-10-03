#ifndef _Word_
#define _Word_

struct Word
{
	char* word;
	struct Word* next;
};

void addWord(char* word, struct Word** wL);
int sizeOfWords(struct Word* wL);
void printWords(struct Word* wL);
struct Word* getAt(int index, struct Word* wL);

#endif