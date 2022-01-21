#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFF_SIZE 2048

typedef struct message
{
    char type[20];
    char content[255];    
}message;

int checkPortNumber(char *porNumber);


int main(int argc, char *argv[]) {
    int sockfd, portNumber, rcvBytes, sendBytes;
    socklen_t len;
    struct sockaddr_in servaddr;

    if (argc != 3)
    {
        printf("Invalid parameter!\n");
        printf("Input pattern: ./client 127.0.0.1 5500\n");
        return 0;
    }

    if (!checkPortNumber(argv[2]))
    {
        printf("Parameter 2 is error number!\n");
        return 0;
    }
    portNumber = atoi(argv[2]);
    if (!(portNumber > 0 && portNumber < 65535))
    {
        printf("Port number's range is: 0 - 65535\n");
        return 0;
    }

    //Step 1: Construct socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: Cannot create server socket. \n ");
        return 0;
    }

    //Step 2: Define the address of the server
    char *ip_addr = argv[1];
    if (!inet_aton(ip_addr, &servaddr.sin_addr))
    {
        printf("Invalid IP address %s\n", ip_addr);
        return 0;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_aton(ip_addr, &servaddr.sin_addr);
    servaddr.sin_port = htons(portNumber);

    //Step 3: Communicate with server
    //request
    len = sizeof(servaddr);
    if (connect(sockfd, (struct sockaddr *)&servaddr, len) < 0)
    {
        perror("Error: Cannot connect to server!");
        return 0;
    }
    printf("Connected to server!\n");

redo: 
    for(;;) {
        char buff[BUFF_SIZE], type[20], content[255];
        struct message msg; 
        message *msg = (message *)malloc(sizeof(message));
               
        printf('Type of content: ');
        gets(type);
        // handle type 
        if (strlen(type) == 0)
        {
            printf("You've typed nothing. Exit.\n");
            return 0;
        }
        if (strcmp(type, "name") == 0) {
            printf("Username:");
        } else if (strcmp(type, "text") == 0) {
            printf("Text message:");
        } else if (strcmp(type, "bye") == 0)
        {   // Exit when bye
            printf("Exit.\n");
            return 0;
        } else {
            printf("Wrong type of message.\n");
            return 0;
        }
        gets(content);   
        strcpy(msg.type, type);     
        strcpy(msg.content, content);  
        sendBytes = send(sockfd, msg, strlen(msg), 0);
        if (sendBytes < 0)
        {
            perror("Error: Cannot send username to server\n ");
            break;
        }
    }
    

}

int checkPortNumber(char *porNumber)
{
    if (strlen(porNumber) == 0)
        return 0;
    for (int i = 0; i < strlen(porNumber); i++)
    {
        if (porNumber[i] >= '0' && porNumber[i] <= '9')
            return 1;
    }
    return 0;
}