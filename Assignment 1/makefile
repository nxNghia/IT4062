CC=gcc
CFLAGS=-I

main: Main.c userManagement.o student.o word.o
	$(CC) -o main Main.c UserManagement.c Student.c Word.c -I

word.o: Word.c
	$(CC) -o word Word.c -I

userManagement.o: UserManagement.c
	$(CC) -o userManagement UserManagement.c -I

word.o: Word.c
	$(CC) -o word Word.c -I