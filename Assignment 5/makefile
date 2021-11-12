CC=gcc
CFLAGS=-I
all: server client

server: server.c userManagement.o student.o word.o
	$(CC) -o server server.c UserManagement.c Student.c Word.c

client: client.c userManagement.o student.o word.o
	$(CC) -o client client.c UserManagement.c Student.c Word.c

word.o: Word.c
	$(CC) -c Word.c

userManagement.o: UserManagement.c student.o
	$(CC) -c UserManagement.c Student.c

student.o: Student.c
	$(CC) -c Student.c