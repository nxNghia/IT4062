#ifndef _UserManagement_
#define _UserManagement_

#include "Student.h"

extern int login;
extern char* fileName;
extern struct Student* students;
extern char saveFile[20];

struct user
{
	char* username;
	char* password;
	char* homepage;
};

extern struct user currentUser;

void Menu();
int convertString(char* num);
void Register();
void Activate();
void SignIn();
void Search();
void ChangePassword();
void SignOut();
void HomepageWithDomainName();
void HomepageWithIPAddress();

#endif