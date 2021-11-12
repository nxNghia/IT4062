#ifndef _StudenList_
#define _StudenList_

extern const char* activate_code;

struct Student
{
	char* user;
	char* password;
	int state;
	char* homepage;
	int block;

	struct Student* next;
};

void addStudent(char* _user, char* _password, int _state, char* homepage, struct Student** sL);
void printStudents(struct Student* sL);
struct Student* studentGet(char* _username, char* _password, struct Student** sL);
struct Student* studentGetByName(char* _username, struct Student** sL);
int checkStudent(char* username, struct Student* sL);

#endif