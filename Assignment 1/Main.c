#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Student.h"
#include "Word.h"
#include "UserManagement.h"

void init(const char* f)
{	
	fileName = (char*)calloc(strlen(f) + 1, sizeof(char));
	strcpy(fileName, f);
	FILE* fp = fopen(f, "r");
	char* str = (char*)calloc(100, sizeof(char));
	struct Word* _Word = NULL;
	char c;
	int pos = 0;
	while(!feof(fp))
	{
		c = fgetc(fp);
		if(c == ' ' || c == '\n' || c == EOF)
		{
			str[pos] = '\0';
			if(strlen(str) != 0)
			{
				addWord(str, &_Word);
			}
			if(sizeOfWords(_Word) == 3)
			{
				addStudent(getAt(0, _Word) -> word, getAt(1, _Word) -> word, 
					convertString(getAt(2, _Word) -> word), &students);
				_Word = NULL;
			}
			pos = 0;
			str[0] = '\0';
		}else{
			str[pos++] = c;
		}
	}

	free(str);
	free(_Word);
	fclose(fp);
}

int main(int argc, const char** argv)
{
	if(argc != 2)
	{
		printf("File input is missing\n");
	}else{
		int option = 0;
		int off = 0;

		init(argv[1]);

		while(1)
		{
			Menu();

			scanf("%d", &option);
			
			switch(option)
			{
				case 1:
				{
					Register();
					break;
				}

				case 2:
				{
					Activate();
					break;
				}

				case 3:
				{
					SignIn();
					break;
				}

				case 4:
				{
					Search();
					break;
				}

				case 5:
				{
					ChangePassword();
					break;
				}

				case 6:
				{
					SignOut();
					break;
				}

				default:
				{
					off = 1;
					break;
				}
			}

			option = 0;

			if(off == 1)
			{
				//save into file
				FILE* fp = fopen(argv[1], "w");
				struct Student* student = students;

				while(student != NULL)
				{
					fprintf(fp, "%s %s %d\n", student -> user,
						student -> password, student -> state);
					student = student -> next;
				}

				fclose(fp);

				printf("Turn off\n");
				break;
			}
		}
	}
}