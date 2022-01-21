#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFF_SIZE 2048

typedef struct userList
{
    char name[20];
    char nameFile[20];
    int status;
    struct userList *next;
} ul;

ul *head = NULL;

typedef struct message
{
    char type[20];
    char content[255];
};

void readFile();
void writeFile();
void printAllAcc();
void insert(ul **head, char name[20], char nameFile[20], int status);
ul *searchAcc(char name[20]);

int checkPortNumber(char *porNumber);
int removeSpace(char *newpass);
int checknum_str(char *newpass);

// write content to user-'s file
void writeContent(char nameFile[20], char content[255]);

int main(int argc, char *argv[])
{
    int loginStatus = 0;

    int portNumber;
    int sockfd, rcvBytes, sendBytes;
    socklen_t len_serv, len_cli;
    char buff[BUFF_SIZE + 1];
    struct sockaddr_in servaddr, cliaddr;

    if (argc != 2)
    {
        printf("Invalid parameter!\n");
        printf("Input pattern: ./server 5500\n");
        return 0;
    }

    if (!checkPortNumber(argv[1]))
    {
        printf("Parameter 1 is error number!\n");
        return 0;
    }
    portNumber = atoi(argv[1]);
    if (!(portNumber >= 0 && portNumber <= 65535))
    {
        printf("Port number's range is: 0 - 65535");
        return 0;
    }

    //Step 1: Construct socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: Cannot create server socket! \n ");
        return 0;
    }

    //Step 2: Bind address to socket
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(portNumber);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
        perror("Error: Cannot bind this address!\n");
        return 0;
    }
    if (listen(sockfd, 2) < 0)
    {
        perror("Error: Cannot place server socket in state LISTEN");
        return 0;
    }
    printf("Server started.\n");

    //Step 3: Communicate with client
    //accept request
    len_serv = sizeof(servaddr);
    len_cli = sizeof(cliaddr);
    int connSock = accept(sockfd, (struct sockaddr *)&cliaddr, (socklen_t *)&len_serv);
    if (connSock < 0)
    {
        perror("Error: Cannot permit incomming connection");
        return 0;
    }

    redo:
    for(;;) {
        char name[20];
        char content[255];
        char nameFile[20]; 

        //Receive message username
        rcvBytes = recv(connSock, name, sizeof(name), 0);
        if (rcvBytes < 0)
        {
            perror("Error: Cannot receive data ");
            return 0;
        }
        name[rcvBytes] = '\0';

    }
}

int removeSpace(char *newpass)
{
    int len = strlen(newpass);
    int i, count = 0;
    for (i = 0; i < len; i++)
    {
        if (newpass[i] != ' ' && newpass[i] != '\t')
            newpass[count++] = newpass[i];
    }
    newpass[count] = '\0';
    return 1;
}

int checknum_str(char *newpass)
{
    removeSpace(newpass);
    int len = strlen(newpass);
    if (len == 0)
        return 0;
    for (int i = 0; i < len; i++)
    {
        if (!((newpass[i] >= '0' && newpass[i] <= '9') ||
              (newpass[i] >= 'a' && newpass[i] <= 'z') ||
              (newpass[i] >= 'A' && newpass[i] <= 'Z')))
            return 0;
    }
    return 1;
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

void readFile()
{
    char *name = malloc(20 * sizeof(char));
    char *nameFile = malloc(20 * sizeof(char));
    int status;

    FILE *fp;
    fp = fopen("user.txt", "r");
    if (fp == NULL)
    {
        printf("Can't open file!\n");
    }
    else
    {
        while (!feof(fp))
        {
            ul *node = (ul *)malloc(sizeof(ul));
            fscanf(fp, "%s %s %d\n", name, nameFile, &status);
            insert(&head, name, nameFile, status);
        }
    }
    free(name);
    fclose(fp);
}

void writeFile()
{
    FILE *fp;
    ul *node;
    node = head;
    fp = fopen("user.txt", "w");
    while (node != NULL)
    {
        fprintf(fp, "%s %s %d\n", node->name, node->nameFile, node->status);
        node = node->next;
    }
    fclose(fp);
}

void printAllAcc()
{
    ul *node;
    node = head;
    while (node != NULL)
    {
        printf("%20s %d\n", node->name, node->status);
        node = node->next;
    }
}

//push into user list
void insert(ul **head, char name[20], char nameFile[20], int status)
{
    ul *node = (ul *)malloc(sizeof(ul));
    strcpy(node->name, name);
    strcpy(node->nameFile, nameFile);
    node->status = status;
    node->next = *head;
    *head = node;
}

// Search a node
ul *searchAcc(char name[20])
{
    ul *curr = head;
    while (curr != NULL)
    {
        if (strcmp(name, curr->name) == 0)
        {
            return curr;
            break;
        }
        curr = curr->next;
    }
    return NULL;
}

void writeContent(char nameFile[20], char content[255])
{
    FILE *fp;
    // ul *node;
    // node = head;
    fp = fopen(nameFile, "w");
    fprintf(fp, "%s\n", content);
    // node = node->next;
    fclose(fp);
}
