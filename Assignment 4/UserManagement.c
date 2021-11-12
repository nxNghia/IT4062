#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "UserManagement.h"
#include "Student.h"

int login = 0;
const char* fileName = "nguoidung.txt";
struct Student* students = NULL;
struct user currentUser;
char saveFile[20] = "currentAccount.txt";

int isIPAddress(const char* str)
{
	int isNotNumber = 0;
	int dot = 0;
	int values[6] = {-1, -1, -1, -1, -1, -1};
	int temp_value = -1;
	int i = 0;

	for (i = 0; i < strlen(str); ++i)
	{
		if((str[i] < '0' || str[i] > '9') && str[i] != '.')
		{
			return 0;
		}

		if(str[i] >= '0' && str[i] <= '9')
		{
			if(temp_value == -1)
				temp_value = 0;

			temp_value *= 10;
			temp_value += str[i] - '0';
		}

		if(str[i] == '.')
		{
			values[dot++] = temp_value;
			temp_value = -1;
		}
	}

	values[dot] = temp_value;

	if(dot != 3)
		return 0;

	if(i != strlen(str))
		return 0;

	for (i = 0; i < dot + 1; ++i)
	{
		if (values[i] == -1 || values[i] > 255)
			return 0;
	}

	return 1;
}

void Menu()
{
	printf("\n");
	printf("USER MANAGEMENT PROGRAM\n");
	printf("--------------------------------------\n");
	printf("1. Register\n");
	printf("2. Activate\n");
	printf("3. Sign in\n");
	printf("4. Search\n");
	printf("5. Change password\n");
	printf("6. Sign out\n");
	printf("7. HomepageWithDomainName\n");
	printf("8. HomepageWithIPAddress\n");
	printf("Your choice (1-6, other to quit)\n\n");
}

int convertString(char* num)
{
	int sum = 0;
	for (int i = 0; i < strlen(num); ++i)
	{
		sum *= 10;
		sum += num[i] - '0';
	}

	return sum;
}

int findUsername(struct Student* sL, char* username)
{
	while(sL != NULL)
	{
		if(strcmp(sL -> user, username) == 0)
			return 1;

		sL = sL -> next;
	}

	return 0;
}

void Register()
{
	printf("Register\n\n");
	char* username = (char*)calloc(100, sizeof(char));
	char* password = (char*)calloc(100, sizeof(char));
	char* homepage = (char*)calloc(100, sizeof(char));

	printf("Username: ");
	scanf("%s", username);

	if(findUsername(students, username))
		printf("Account existed\n");
	else{
		printf("Password: ");
		scanf("%s", password);

		printf("Homepage: \n");
		scanf("%s", homepage);
		//add into list
		addStudent(username, password, 2, homepage, &students);
		printf("Successful registration. Activated required\n");

		//save into 
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

	free(username);
	free(password);
}

void Activate()
{
	printf("Activate\n\n");
	char* username = (char*)calloc(100, sizeof(char));
	char* password = (char*)calloc(100, sizeof(char));
	char* _activate_code = (char*)calloc(100, sizeof(char));

	printf("Username: ");
	scanf("%s", username);
	printf("Password: ");
	scanf("%s", password);
	printf("Activate code: ");
	scanf("%s", _activate_code);

	struct Student* res = studentGet(username, password, &students);

	if(res == NULL)
	{
		printf("Username or password is not correct\n");
	}else{
		if(res -> state == 2)
		{
			if(strcmp(_activate_code, activate_code) == 0)
			{
				res -> state = 1;
				printf("Account is activated\n");
				//save into 
				res -> block = 0;
				FILE* fp = fopen(fileName, "w");
				struct Student* student = students;

				while(student != NULL)
				{
					fprintf(fp, "%s %s %d %s\n", student -> user,
						student -> password, student -> state, student -> homepage);
					student = student -> next;
				}
				fclose(fp);
			}else{
				res -> block += 1;
				if(res -> block == 4)
				{	
					printf("Acitvation code is incorrect\n");
					printf("Account is blocked\n");
					res -> state = 0;
				}else{
					printf("Account is not activated\n");
				}
			}
		}else{
			if(res -> state == 1)
			{
				printf("Account is already activated\n");
			}else{
				printf("Account is blocked\n");
			}
		}
	}

	free(username);
	free(password);
}

void SignIn()
{
	if(login == 1)
	{
		printf("User already signed in\n");
		return;
	}
	printf("Sign in\n\n");
	char* username = (char*)calloc(100, sizeof(char));
	char* password = (char*)calloc(100, sizeof(char));
	printf("Username: ");
	scanf("%s", username);

	int res = checkStudent(username, students);
	struct Student* res2 = studentGetByName(username, &students);

	switch(res)
	{
		case 1:
		{
			printf("Password: ");
			scanf("%s", password);

			if(strcmp(password, res2 -> password) != 0)
			{
				printf("Password is incorrect\n");
				res2 -> block += 1;
				if(res2 -> block == 4)
				{
					res2 -> state = 0;
					printf("Account is blocked\n");
				}

				break;
			}

			switch(res2 -> state)
			{
				case 0:
				{
					printf("Account is blocked\n");
					break;
				}

				case 1:
				{
					printf("Hello %s\n", username);
					res2 -> block = 0;
					login = 1;
					currentUser.username = (char*)calloc(strlen(username), sizeof(char));
					currentUser.password = (char*)calloc(strlen(password), sizeof(char));
					currentUser.homepage = (char*)calloc(strlen(res2 -> homepage), sizeof(char));
					
					strcpy(currentUser.username, username);
					strcpy(currentUser.password, password);
					strcpy(currentUser.homepage, res2 -> homepage);

					//save into 
					FILE* fp = fopen(saveFile, "w");
					fprintf(fp, "%c\n", '1');
					fprintf(fp, "%s\n", currentUser.username);
					fprintf(fp, "%s\n", currentUser.password);
					fprintf(fp, "%s\n", currentUser.homepage);
					fclose(fp);

					break;
				}

				case 2:
				{
					printf("Account is not activated\n");
					break;
				}
			}
			break;
		}

		case 2:
		{
			printf("Password is incorrect\n");
			res2 -> block += 1;
			if(res2 -> block == 4)
			{
				res2 -> state = 0;
				printf("Account is blocked\n");
			}

			break;
		}

		case 0:
		{
			printf("Cannot find account\n");
			break;
		}
	}

	free(username);
	free(password);
}

void Search()
{
	printf("Search\n\n");
	char* username = (char*)calloc(100, sizeof(char));
	printf("Username: ");
	scanf("%s", username);
	struct Student* res = studentGetByName(username, &students);

	if(res != NULL)
	{
		switch(res -> state)
		{
			case 0:
			{
				printf("Account is blocked\n");
				break;
			}

			case 1:
			{
				printf("Account is active");
				break;
			}

			case 2:
			{
				printf("Account is not activated\n");
				break;
			}
		}
	}else{
		printf("Cannot find account\n");
	}

	free(username);
}

void ChangePassword()
{
	printf("Change password\n\n");
	char* username = (char*)calloc(100, sizeof(char));
	char* password = (char*)calloc(100, sizeof(char));
	char* newPassword = (char*)calloc(100, sizeof(char));
	printf("Username: ");
	scanf("%s", username);
	printf("Password: ");
	scanf("%s", password);
	printf("New password: ");
	scanf("%s", newPassword);

	if(!login)
	{
		printf("Account is not sign in\n");	
	}else{
		if(login && strcmp(username, currentUser.username) == 0 && strcmp(password, currentUser.password) == 0)
		{
			struct Student* res = studentGetByName(username, &students);
			strcpy(res -> password, newPassword);
			strcpy(currentUser.password, newPassword);
			printf("Password changed\n");
			//save into 
			FILE* fp = fopen(fileName, "w");
			struct Student* student = students;

			while(student != NULL)
			{
				fprintf(fp, "%s %s %d %s\n", student -> user,
					student -> password, student -> state, student -> homepage);
				student = student -> next;
			}
			fclose(fp);

			fp = fopen(saveFile, "w");
			fprintf(fp, "%c\n", '1');
			fprintf(fp, "%s\n", currentUser.username);
			fprintf(fp, "%s\n", currentUser.password);
			fprintf(fp, "%s\n", currentUser.homepage);
			fclose(fp);
		}else{
			if(strcmp(username, currentUser.username) == 0 && strcmp(password, currentUser.password) != 0)
			{
				printf("Current password is incorrect. Please try again\n");
			}else{
				printf("Cannot find account\n");
			}
		}
	}

	free(username);
	free(password);
	free(newPassword);
}

void SignOut()
{
	printf("Sign out\n\n");
	char* username = (char*)calloc(100, sizeof(char));
	printf("Username: ");
	scanf("%s", username);

	if(!login)
	{
		printf("Account is not sign in\n");
	}else{
		struct Student* res = studentGetByName(username, &students);

		if(res != NULL)
		{
			printf("Good bye %s\n", username);
			login = 0;
			FILE* fp = fopen(saveFile, "w");
			fprintf(fp, "%c\n", '0');
			fclose(fp);
		}else{
			printf("Cannot find account\n");
		}
	}

	free(username);
}

void HomepageWithIPAddress()
{
	if(!login)
	{
		printf("User hasn't loged in\n");
		return;
	}

	struct hostent* he;
	struct in_addr** addr_list;
	struct in_addr addr;
	int i = 0;

	he = gethostbyname(currentUser.homepage);

	if(he == NULL)
	{
		printf("Not found information\n");
		return;
	}

	printf("Official IP: %s\n", inet_ntoa(*(struct in_addr*) he -> h_addr));

	printf("Alias IP:\n");
	addr_list = (struct in_addr**) he -> h_addr_list;
	for (i = 1; addr_list[i] != NULL; ++i)
		printf("%s\n", inet_ntoa(*addr_list[i]));
}

void HomepageWithDomainName()
{
	if(!login)
	{
		printf("User hasn't loged in\n");
		return;
	}

	if(isIPAddress(currentUser.homepage) == 0)
	{
		printf("Official name: %s\n", currentUser.homepage);
		return;
	}

	struct hostent* he;
	struct in_addr** addr_list;
	struct in_addr addr;
	int i = 0;

	inet_aton(currentUser.homepage, &addr);
	he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
	if(he == NULL)
	{
		printf("Not found information\n");
		return;
	}
	printf("Official name: %s\n", he -> h_name);
	addr_list = (struct in_addr**) he -> h_addr_list;

}