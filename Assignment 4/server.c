#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "Student.h"
#include "Word.h"
#include "UserManagement.h"

const char end_message[9] = "Good bye";

void init()
{
	FILE* fp = fopen(fileName, "r");
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
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	char *hello = "Hello from server";

	if (argc != 2)
	{
		printf("Invalid parameter\n");
		exit(1);
	}

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(atoi(argv[1]));

	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		perror("bind fail");
		exit(EXIT_FAILURE);
	}

	if(listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	if((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}

	init();

	while(1)
	{
		memset(buffer, 0, 1024);
		valread = read(new_socket, buffer, 1024);

		if(valread == 0)
		{
			send(new_socket, end_message, strlen(end_message), 0);
			printf("Good bye\n");
		}else{
			struct Word* param = split(buffer);

			switch(buffer[0])
			{
				//block
				//0?hust
				case '0':
				{
					struct Student* res = studentGetByName(getAt(0, param) -> word, &students);

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

				//reset password
				//1?hust?hust321
				case '1':
				{
					struct Student* res = studentGetByName(getAt(1, param) -> word, &students);
					
					if (res != NULL)
					{
						struct Word* split = validate(getAt(0, param) -> word);

						if(split == NULL)
						{
							send(new_socket, "Error", strlen("Error"), 0);
						}else{
							strcpy(res -> password, getAt(0, param) -> word);

							FILE* fp = fopen(fileName, "w");
							struct Student* student = students;

							while(student != NULL)
							{
								fprintf(fp, "%s %s %d %s\n", student -> user,
									student -> password, student -> state, student -> homepage);
								student = student -> next;
							}
							fclose(fp);

							char message[100] = "";
							while(split != NULL)
							{
								split -> word[strlen(split -> word) + 1] = '\0';
								split -> word[strlen(split -> word)] = '\n';
								strcat(message, split -> word);

								split = split -> next;
							}

							send(new_socket, message, strlen(message), 0);
						}
					}

					break;
				}

				//sign in
				case '2':
				{
					struct Student* res = studentGetByName(getAt(1, param) -> word, &students);

					if (res != NULL)
					{
						if (strcmp(getAt(0, param) -> word, res -> password) == 0)
						{
							if(res -> state == 1)
							{
								FILE* fp = fopen(saveFile, "w");
								fprintf(fp, "%d\n%s\n%s\n", 1, getAt(1, param) -> word, getAt(0, param) -> word);
								fclose(fp);
								printf("Hello %s\n", getAt(1, param) -> word);
								send(new_socket, "OK", strlen("OK"), 0);
							}

							if(res -> state == 2)
								send(new_socket, "Account not ready", strlen("Account not ready"), 0);
						}else{
							if(res -> state == 0)
								send(new_socket, "Account is blocked", strlen("Account is blocked"), 0);
							else
								send(new_socket, "not OK", strlen("not OK"), 0);
						}
					}else{
						send(new_socket, "Error", strlen("Error"), 0);
					}

					break;
				}

				//log out
				case '3':
				{
					FILE* fp = fopen(saveFile, "w");
					fprintf(fp, "%d\n", 0);
					fclose(fp);

					printf("Good bye %s\n", getAt(0, param) -> word);

					send(new_socket, "Good bye", strlen("Good bye"), 0);
					break;
				}
			}
		}
	}
	return 0;
}