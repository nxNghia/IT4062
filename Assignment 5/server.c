#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "Student.h"
#include "Word.h"
#include "UserManagement.h"

#define MAXLINE 100
#define LISTENQ 1

const char end_message[9] = "Good bye";

void init()
{
	FILE* fp = fopen(fileName, "r");

	if (fp == NULL)
	{
		printf("File is not exist\n");
		exit(0);
	}

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
			if(sizeOfWords(_Word) == 4)
			{
				addStudent(getAt(0, _Word) -> word, getAt(1, _Word) -> word, 
					convertString(getAt(2, _Word) -> word), getAt(3, _Word) -> word, &students);
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

	fp = fopen(saveFile, "r");
	c = fgetc(fp);

	if(c == '1')
	{
		login = 1;
	
		fscanf(fp, "%s", str);
		currentUser.username = (char*)calloc(strlen(str), sizeof(char));
		strcpy(currentUser.username, str);
		fscanf(fp, "%s", str);
		currentUser.password = (char*)calloc(strlen(str), sizeof(char));
		strcpy(currentUser.password, str);
		fscanf(fp, "%s", str);
		fscanf(fp, "%s", str);
		currentUser.homepage = (char*)calloc(strlen(str), sizeof(char));
		strcpy(currentUser.homepage, str);
	}else{
		login = 0;
	}
}

struct Word* validate(char* str)
{
	int i = 0;
	char alphabet[100];
	char numbers[100];
	int a_num = 0;
	int n_num = 0;

	for (i = 0; i < strlen(str); ++i)
	{
		if(str[i] >= '0' && str[i] <= '9')
			numbers[n_num++] = str[i];
		else
			if((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z'))
				alphabet[a_num++] = str[i];
			else
				return NULL;
	}

	alphabet[a_num] = '\0';
	numbers[n_num] = '\0';

	struct Word* res = NULL;
	addWord(numbers, &res);
	addWord(alphabet, &res);

	return res;
}

int main(int argc, const char* argv[])
{
	int listenfd, connfd, n;
	pid_t childpid;
	socklen_t clilen;
	char buf[MAXLINE];
	struct sockaddr_in cliaddr, servaddr;

	if (argc != 2)
	{
		printf("Invalid parameter\n");
		exit(0);
	}

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);
	printf("Server running... Waiting for connections.\n");

	init();

	for (;;)
	{
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);
		printf("Received request...\n");

		while((n = recv(connfd, buf, MAXLINE, 0)) > 0)
		{
			struct Word* params = split(buf);

			switch(buf[0])
			{
				case '0':
				{
					struct Student* res = studentGetByName(getAt(0, params) -> word, &students);

					if(res != NULL)
					{
						res -> state = 0;

						FILE* fp = fopen(fileName, "w");
						struct Student* student = students;

						while(student != NULL)
						{
							fprintf(fp, "%s %s %d %s\n", student -> user,
								student -> password, student -> state, student -> homepage);
							student = student -> next;
						}
						fclose(fp);
					}

					break;
				}

				case '1':
				{
					struct Student* res = studentGetByName(getAt(1, params) -> word, &students);
					
					if (res != NULL)
					{
						struct Word* split = validate(getAt(0, params) -> word);

						if(split == NULL)
						{
							send(connfd, "Error", strlen("Error"), 0);
						}else{
							strcpy(res -> password, getAt(0, params) -> word);

							FILE* fp = fopen(fileName, "w");
							struct Student* student = students;

							while(student != NULL)
							{
								fprintf(fp, "%s %s %d %s\n", student -> user,
									student -> password, student -> state, student -> homepage);
								student = student -> next;
							}
							fclose(fp);

							memset(buf, 0, MAXLINE);
							while(split != NULL)
							{
								split -> word[strlen(split -> word) + 1] = '\0';
								split -> word[strlen(split -> word)] = '\n';
								strcat(buf, split -> word);

								split = split -> next;
							}

							send(connfd, buf, strlen(buf), 0);
						}
					}

					break;
				}

				case '2':
				{
					struct Student* res = studentGetByName(getAt(1, params) -> word, &students);

					if (res != NULL)
					{
						if (strcmp(getAt(0, params) -> word, res -> password) == 0)
						{
							if(res -> state == 1)
							{
								FILE* fp = fopen(saveFile, "w");
								fprintf(fp, "%d\n%s\n%s\n", 1, getAt(1, params) -> word, getAt(0, params) -> word);
								fclose(fp);
								printf("Hello %s\n", getAt(1, params) -> word);
								send(connfd, "OK", strlen("OK"), 0);
							}

							if(res -> state == 2)
								send(connfd, "Account not ready", strlen("Account not ready"), 0);
						}else{
							if(res -> state == 0)
								send(connfd, "Account is blocked", strlen("Account is blocked"), 0);
							else
								send(connfd, "not OK", strlen("not OK"), 0);
						}
					}else{
						send(connfd, "Error", strlen("Error"), 0);
					}

					break;
				}

				case '3':
				{
					FILE* fp = fopen(saveFile, "w");
					fprintf(fp, "%d\n", 0);
					fclose(fp);

					printf("Good bye %s\n", getAt(0, params) -> word);

					send(connfd, "Good bye", strlen("Good bye"), 0);


					break;
				}
			}

			memset(buf, 0, MAXLINE);
		}

		if (n < 0)
		{
			perror("Read error");
			exit(1);
		}

		close(connfd);
	}
	close(listenfd);

	return 0;
}