#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "Student.h"
#include "Word.h"
#include "UserManagement.h"

#define MAXLINE 100

const char end_message[4] = "bye";
struct Student* attemp = NULL;
   
int main(int argc, char const *argv[])
{
    if(argc != 3)
    {
        printf("Invalid parameter\n");
        exit(1);
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];
    char* username = (char*)calloc(100, sizeof(char));
    char* password = (char*)calloc(100, sizeof(char));
    char* str = (char*)calloc(100, sizeof(char));

    FILE* fp = fopen(saveFile, "r");
    char c;
    c = fgetc(fp);

    if(c == '1')
    {
        login = 1;

        fscanf(fp, "%s", str);
        strcpy(currentUser.username, str);

        fscanf(fp, "%s", str);
        strcpy(currentUser.password, str);
    }

    //create a socket for client
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Problem in creating the socket");
        exit(2);
    }

    //Creation of the remote server socket information structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));

    //Connect the client to the server socket
    if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
    {
        perror("Problem in connecting to the server");
        exit(3);
    }

    currentUser.username = (char*)calloc(100, sizeof(char));
    currentUser.password = (char*)calloc(100, sizeof(char));

    while(1)
    {
        if (login == 1)
        {
            fgets(sendline, sizeof(sendline), stdin);
            if(sendline[0] == '\n')
                exit(1);

            if (strlen(sendline) == 0)
                exit(1);

            if (strcmp(sendline, end_message) == 0)
            {
                memset(sendline, 0, MAXLINE);
                strcat(sendline, "3?");
                strcat(sendline, currentUser.username);

                send(sockfd, sendline, strlen(sendline), 0);

                memset(recvline, 0, MAXLINE);
                int rec = recv(sockfd, recvline, MAXLINE, 0);

                printf("%s %s\n", recvline, currentUser.username);
                login = 0;
            }else{
                char* newPassword = (char*)calloc(MAXLINE, sizeof(char));
                strcpy(newPassword, sendline);

                memset(sendline, 0, MAXLINE);

                strcat(sendline, "1?");
                strcat(sendline, currentUser.username);
                strcat(sendline, "?");
                strcat(sendline, newPassword);

                send(sockfd, sendline, strlen(sendline), 0);

                memset(recvline, 0, MAXLINE);
                int rec = recv(sockfd, recvline, MAXLINE, 0);

                printf("%s\n", recvline);
            }
        }else{
            printf("Username: ");
            fgets(username, sizeof(username), stdin);
            if(username[0] == '\n')
                exit(1);

            if (strlen(username) == 0)
                exit(1);

            printf("Insert password: ");
            fgets(password, sizeof(password), stdin);
            if(password[0] == '\n')
                exit(1);

            if (strlen(password) == 0)
                exit(1);

            memset(sendline, 0, MAXLINE);
            strcat(sendline, "2?");
            strcat(sendline, username);
            strcat(sendline, "?");
            strcat(sendline, password);

            send(sockfd, sendline, strlen(sendline), 0);

            memset(recvline, 0, MAXLINE);
            int rec = recv(sockfd, recvline, MAXLINE, 0);

            if (strcmp(recvline, "OK") == 0)
            {
                login = 1;
                strcpy(currentUser.username, username);
                strcpy(currentUser.password, password);
                
                struct Student* res = studentGetByName(username, &attemp);
                if (res != NULL)
                    res -> block = 0;
            }

            if (strcmp(recvline, "not OK") == 0)
            {
                struct Student* res = studentGetByName(username, &attemp);
                if(res == NULL)
                {
                    addStudent(username, password, -1, "#", &attemp);
                    res = studentGetByName(username, &attemp);
                    res -> block = 1;
                }else{
                    res -> block += 1;
                    res -> state = 0;
                }

                if(res -> block == 3)
                {
                    //block
                    memset(sendline, 0, MAXLINE);
                    strcat(sendline, "0?");
                    strcat(sendline, username);

                    //0?hust
                    send(sockfd, sendline, strlen(sendline), 0);
                }
            }

            printf("%s\n", recvline);
        }
    }

    return 0;
}