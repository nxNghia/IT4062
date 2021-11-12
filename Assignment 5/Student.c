#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Student.h"

const char* activate_code = "20184166";

void addStudent(char* _user, char* _password, int _state, char* _homepage, struct Student** sL)
{
	struct Student* new_student = (struct Student*)calloc(1, sizeof(struct Student));
	new_student -> user = (char*)calloc(strlen(_user), sizeof(char));
	new_student -> password = (char*)calloc(strlen(_password), sizeof(char));
	new_student -> homepage = (char*)calloc(strlen(_homepage), sizeof(char));
	new_student -> next = NULL;
	new_student -> block = 0;
	strcpy(new_student -> user, _user);
	strcpy(new_student -> password, _password);
	new_student -> state = _state;
	strcpy(new_student -> homepage, _homepage);

	if(*sL == NULL)
	{
		*sL = new_student;
	}else{
		struct Student* cur = *sL;
		struct Student* previous = NULL;

		while(cur != NULL)
		{
			previous = cur;
			cur = cur -> next;
		}

		previous -> next = new_student;
	}
}

void printStudents(struct Student* sL)
{
	while(sL != NULL)
	{
		printf("%s\n%s\n%d\n%s\n\n", sL -> user, sL -> password, sL -> block, sL -> homepage);
		sL = sL -> next;
	}
}

struct Student* studentGet(char* _username, char* _password, struct Student** sL)
{
	struct Student* cur = *sL;

	while(cur != NULL)
	{
		if(strcmp(cur -> user, _username) == 0 && strcmp(cur -> password, _password) == 0)
			return cur;

		cur = cur -> next;
	}

	return NULL;
}

int checkStudent(char* _username, struct Student* sL)
{
	while(sL != NULL)
	{
		if(strcmp(sL -> user, _username) == 0)
		{
			return 1;
		}

		sL = sL -> next;
	}

	return 0;
}

struct Student* studentGetByName(char* _username, struct Student** sL)
{
	struct Student* cur = *sL;

	while(cur != NULL)
	{
		if(strcmp(cur -> user, _username) == 0)
			return cur;

		cur = cur -> next;
	}

	return NULL;
}