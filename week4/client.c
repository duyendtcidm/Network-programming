#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFF_SIZE 2048

int checkPortNumber(char *porNumber);

int main(int argc, char *argv[])
{
    int sockfd, portNumber, rcvBytes, sendBytes;
    // only need server address
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
        printf("Port number's range is: 0 - 65535");
        return 0;
    }

    //Step 1: Construct socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Error: Cannot create server socket. \n ");
        return 0;
    }
    printf("Client started!\n");

    //Step 2: Define the address of the server
    char *ip_addr = argv[1];
    if (!inet_aton(ip_addr, &servaddr.sin_addr))
    {
        printf("Invalid IP address %s", ip_addr);
        return 0;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_aton(ip_addr, &servaddr.sin_addr);
    servaddr.sin_port = htons(portNumber);

    //Step 3: Communicate with server
    len = sizeof(servaddr);
redo:
    while (1)
    {
        char buff[BUFF_SIZE], name[20], password[20], newpass[20];
        printf("Username: ");
        gets(name);
        // hanle bye-Bye ma chua login

        if (strlen(name) == 0)
        {
            // Empty string -> exit
            printf("You've typed nothing. Exit.\n");
            return 0;
        }
        sendBytes = sendto(sockfd, name, strlen(name), 0,
                           (struct sockaddr *)&servaddr, len);
        if (sendBytes < 0)
        {
            perror("Error: Cannot send username to server\n ");
            break;
        }
        if (strcmp(name, "bye") == 0)
        {
            printf("Exit.\n");
            return 0;
        }

        /// input handle
        // receive notice after insert username
        rcvBytes = recvfrom(sockfd, buff, BUFF_SIZE, 0,
                            (struct sockaddr *)&servaddr, &len);
        if (rcvBytes < 0)
        {
            perror("Error: Cannot receive from server\n");
            break;
        }
        buff[rcvBytes] = '\0';
        //insert pass and send to server
        if (strcmp(buff, "Insert password:") != 0)
        {
            printf("%s\n", buff);
            goto redo;
        }
        else
        {
            printf("%s", buff);

                gets(password);
                if (strlen(password) == 0)
                {
                    printf("You've just inserted nothing. Exit.\n");
                    sendBytes = sendto(sockfd, password, strlen(password), 0,
                                       (struct sockaddr *)&servaddr, len);
                    if (sendBytes < 0)
                    {
                        perror("Error: Cannot send password to server\n ");
                        break;
                    }
                    return 0;
                }
                sendBytes = sendto(sockfd, password, strlen(password), 0,
                                   (struct sockaddr *)&servaddr, len);
                if (sendBytes < 0)
                {
                    perror("Error: Cannot send password to server\n ");
                    break;
                }        
            
            checkpass: while (1)
            { 
                rcvBytes = recvfrom(sockfd, buff, BUFF_SIZE, 0,
                                    (struct sockaddr *)&servaddr, &len);
                if (rcvBytes < 0)
                {
                    perror("Error: Cannot receive information after insert password\n");
                    break;
                }
                buff[rcvBytes] = '\0';

                if (strcmp(buff, "OK") != 0 && strcmp(buff, "Not OK") != 0)
                {
                    printf("%s", buff);
                    // goto redo;
                    break;
                }

                //change password

                else if (strcmp(buff, "OK") == 0)
                {
                    printf("%s\n", buff);
                changepass:
                    printf("Enter new password:");
                    gets(newpass);
                    if (strlen(newpass) == 0)
                    {
                        printf("You've just inserted nothing. Exit.\n");
                        return 0;
                    }
                    sendBytes = sendto(sockfd, newpass, strlen(newpass), 0,
                                       (struct sockaddr *)&servaddr, len);
                    if (sendBytes < 0)
                    {
                        perror("Error: Cannot send newpass to server\n ");
                        break;
                    }
                    char number_pass[10], str_pass[10];
                    rcvBytes = recvfrom(sockfd, number_pass, sizeof(number_pass), 0,
                                        (struct sockaddr *)&servaddr, &len);
                    if (rcvBytes < 0)
                    {
                        perror("Error: Cannot receive information after insert password\n");
                        break;
                    }
                    number_pass[rcvBytes] = '\0';
                    if (strcmp(number_pass, "Goodbye ") == 0)
                    {
                        printf("%s %s\n", number_pass, name);
                    }
                    else if (strlen(number_pass) == 0 || checkPortNumber(number_pass) == 1)
                    {
                        if (strlen(number_pass) != 0)
                            printf("%s\n", number_pass);
                        
                        rcvBytes = recvfrom(sockfd, str_pass, sizeof(str_pass), 0,
                                            (struct sockaddr *)&servaddr, &len);
                        if (rcvBytes < 0)
                        {
                            perror("Error: Cannot receive information after insert password\n");
                            break;
                        }
                        str_pass[rcvBytes] = '\0';
                        if (strlen(str_pass) != 0) {
                            printf("%s\n", str_pass);
                        }
                        goto changepass;
                    }
                    else
                    { // error
                        printf("%s\n", number_pass);
                        goto changepass;
                    }
                    break;
                }
                else if (strcmp(buff, "Not OK") == 0)
                {
                    printf("%s\n", buff);
                    char passn[20];
                    gets(passn);
                    if (strlen(passn) == 0)
                    {
                        printf("You've just inserted nothing. Exit.\n");
                        sendBytes = sendto(sockfd, passn, strlen(passn), 0,
                                           (struct sockaddr *)&servaddr, len);
                        if (sendBytes < 0)
                        {
                            perror("Error: Cannot send passn to server\n ");
                            break;
                        }
                        break;
                    }
                    sendBytes = sendto(sockfd, passn, strlen(passn), 0,
                                       (struct sockaddr *)&servaddr, len);
                    if (sendBytes < 0)
                    {
                        perror("Error: Cannot send password to server\n ");
                        break;
                    }
                    goto checkpass;
                }
            }
        }
    }
    close(sockfd);
    return 0;
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