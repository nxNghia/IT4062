#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "Student.h"
#include "Word.h"
#include "UserManagement.h"

const char end_message[4] = "bye";
struct Student* attemp = NULL;
   
int main(int argc, char const *argv[])
{
    if(argc != 3)
    {
        printf("Invalid parameter\n");
        exit(1);
    }
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char mes[100];
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }


    currentUser.username = (char*)calloc(100, sizeof(char));
    currentUser.password = (char*)calloc(100, sizeof(char));
    
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

    int i = 0;
    char* option = (char*)calloc(100, sizeof(char));

    while(1)
    {
        if(login == 1)
        {
            scanf("%s", option);
            if(strcmp(option, end_message) == 0)
            {
                char message[20] = "3?";
                strcat(message, currentUser.username);
                send(sock, message, strlen(message), 0);

                memset(buffer, 0, 1024);
                valread = read(sock, buffer, 1024);
                printf("%s %s\n", buffer, currentUser.username);
                login = 0;
            }else{
                char message[100];

                strcpy(message, "1?");
                strcat(message, currentUser.username);
                strcat(message, "?");
                strcat(message, option);
                
                //1?hust123?123
                send(sock, message, strlen(message), 0);
                memset(buffer, 0, 1024);
                valread = read(sock, buffer, 1024);

                printf("%s\n", buffer);
            }
        }else{
            printf("Username: ");
            scanf("%s", username);

            printf("Insert password\n");
            printf("Password: ");
            scanf("%s", password);
            
            char message[100];
            strcpy(message, "2?");
            strcat(message, username);
            strcat(message, "?");
            strcat(message, password);

            //2?hust?hust123
            send(sock, message, strlen(message), 0);

            memset(buffer, 0, 1024);
            valread = read(sock, buffer, 1024);
            
            if(strcmp(buffer, "OK") == 0)
            {
                login = 1;
                strcpy(currentUser.username, username);
                strcpy(currentUser.password, password);
                
                struct Student* res = studentGetByName(username, &attemp);
                if (res != NULL)
                    res -> block = 0;
                printf("%s\n", buffer);
            }else{
                if(strcmp(buffer, "not OK") == 0)
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
                        char message[100];
                        strcpy(message, "0?");
                        strcat(message, username);

                        //0?hust
                        send(sock, message, strlen(message), 0);
                    }
                }
                printf("%s\n", buffer);
            }
        }
    }
    return 0;
}