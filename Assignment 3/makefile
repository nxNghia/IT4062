CC=gcc
CFLAGS=-I

main: Main.c userManagement.o student.o word.o
	$(CC) -o main Main.c UserManagement.c Student.c Word.c

word.o: Word.c
	$(CC) -c Word.c

userManagement.o: UserManagement.c student.o
	$(CC) -c UserManagement.c Student.c

student.o: Student.c
	$(CC) -c Student.c