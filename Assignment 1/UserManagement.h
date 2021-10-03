#ifndef _UserManagement_
#define _UserManagement_

#include "Student.h"

extern int login;
extern char* fileName;
extern struct Student* students;

struct user
{
	char* username;
	char* password;
};

struct user currentUser;

void Menu();
int convertString(char* num);
void Register();
void Activate();
void SignIn();
void Search();
void ChangePassword();
void SignOut();

#endif