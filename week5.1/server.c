#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFF_SIZE 2048

typedef struct list
{
    char name[20];
    char password[20];
    int status;
    struct list *next;
} ll;

ll *head = NULL;

void readFile();
void writeFile();
void insert(ll **head, char name[20], char password[20], int status);
ll *searchAcc(char name[20]);
void printAllAcc();

int checkPortNumber(char *porNumber);
int removeSpace(char* newpass);
int checknum_str(char *newpass);
int encode_pass(char *newpass, char *number_pass, char *str_pass);

int main(int argc, char *argv[])
{
    readFile();
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
    for (;;)
    {
        char name[20], password[20], newpassword[20], mess[10];
        //Receive message username
        rcvBytes = recv(connSock, name, sizeof(name), 0);
        if (rcvBytes < 0)
        {
            perror("Error: Cannot receive data ");
            return 0;
        }
        name[rcvBytes] = '\0';

        //Send data to client after check username
        if (strcmp(name, "bye") == 0)
        {
            return 0;
        }
        ll *node = searchAcc(name);
        if (node == NULL)
        {
            char not_exit_acc[] = "Not exited account";
            send(connSock, not_exit_acc, strlen(not_exit_acc), 0);
            goto redo;
        }
        else
        {
            if (loginStatus != 0)
            {
                char logined[] = "You've logined";
                send(connSock, logined, strlen(logined), 0);
                goto redo;
            }
            else
            {
                    char insert_pass[] = "Insert password:";
                    send(connSock, insert_pass, strlen(insert_pass), 0);
                    //check password from client
                    int times = 3;
                    while (1)
                {
                    rcvBytes = recv(connSock, password, sizeof(password), 0);
                    if (rcvBytes < 0)
                    {
                        perror("Error: Cannot receive data ");
                        return 0;
                    }
                    password[rcvBytes] = '\0';
                    //passs - insert
                    if (strcmp(password, node->password) == 0)
                    {

                        //Notice login successfully to client
                        if (node->status != 1)
                        {
                            char block_or_unactive[] = "Account is not ready\n";
                            send(connSock, block_or_unactive, strlen(block_or_unactive), 0);
                            goto redo;
                        }
                        else
                        {
                            times = 3;
                            loginStatus = 1;
                            //change password
                            char login_success[] = "OK";
                            send(connSock, login_success, strlen(login_success), 0);
                        changepass:
                            rcvBytes = recv(connSock, newpassword, sizeof(newpassword), 0);
                            if (rcvBytes < 0)
                            {
                                perror("Error: Cannot receive data ");
                                return 0;
                            }
                            newpassword[rcvBytes] = '\0';
                            char number_pass[20], str_pass[20];
                            //check & save new password
                            if (encode_pass(newpassword, number_pass, str_pass) != 1 || strlen(newpassword) == 0)
                            {
                                char new_pass_error[] = "Error";
                                send(connSock, new_pass_error, strlen(new_pass_error), 0);
                                // break;
                                goto changepass;
                            }
                            else
                            {
                                if (strcmp(newpassword, "bye") == 0)
                                {
                                    char message[] = "Goodbye ";
                                    loginStatus = 0;
                                    send(connSock, message, strlen(message), 0);
                                    loginStatus = 0;
                                    goto redo;
                                }
                                else
                                {
                                    strcpy(node->password, newpassword);
                                    writeFile();
                                    encode_pass(newpassword, number_pass, str_pass);
                                    if (strlen(number_pass) == 0) {
                                        send(connSock, str_pass, strlen(str_pass), 0);
                                    } else {
                                        //receive
                                        send(connSock, number_pass, strlen(number_pass), 0);
                                    rcvBytes = recv(connSock, mess, sizeof(mess), 0);
                                    if (rcvBytes < 0)
                                    {
                                        perror("Error: Cannot receive data ");
                                        return 0;
                                    }
                                    mess[rcvBytes] = '\0';
                                    if (strlen(str_pass) != 0){
                                        send(connSock, str_pass, strlen(str_pass), 0);
                                    } else {
                                        char only_num[] = "only_num";
                                        send(connSock, only_num, strlen(only_num), 0);
                                    }
                                    
                                    }                                   
                                    
                                    loginStatus = 0;
                                    goto changepass;
                                }
                            }
                        }
                    }
                    else
                    {
                        times -= 1;
                        if (times == 0)
                        {
                            node->status = 0;
                            writeFile();
                            char false_password[] = "Account is blocked\n";
                            send(connSock, false_password, strlen(false_password), 0);
                            goto redo;
                        }
                        else
                        { //enter pass again
                            char try_pass_again[] = "Not OK";
                            send(connSock, try_pass_again, strlen(try_pass_again), 0);
                        }
                    }
                }
                goto redo;
            }
        }
    }

    return 0;
}

int removeSpace(char* newpass) {
    int len = strlen(newpass);
    int i, count = 0;
     for(i = 0; i < len; i++){  
        if(newpass[i] != ' ' && newpass[i] != '\t') 
            newpass[count++] = newpass[i]   ;    
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

int encode_pass(char *newpass, char *number_pass, char *str_pass)
{
    int m = 0, n = 0;
    removeSpace(newpass);
    int len = strlen(newpass);

    if (len == 0)
    {
        number_pass = "";
        str_pass = "";
        return 0;
    }

    if (checknum_str(newpass) == 0)
        return 0;

    for (int i = 0; i < len; i++)
    {
        if (newpass[i] >= '0' && newpass[i] <= '9')
        {
            number_pass[m++] = newpass[i];
        }
        else
            str_pass[n++] = newpass[i];
    }
    number_pass[m] = '\0';
    str_pass[n] = '\0';
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
    char *password = malloc(20 * sizeof(char));
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
            ll *node = (ll *)malloc(sizeof(ll));
            fscanf(fp, "%s %s %d\n", name, password, &status);
            insert(&head, name, password, status);
        }
    }
    free(name);
    free(password);
    fclose(fp);
}

void writeFile()
{
    FILE *fp;
    ll *node;
    node = head;
    fp = fopen("user.txt", "w");
    while (node != NULL)
    {
        fprintf(fp, "%s %s %d\n", node->name, node->password, node->status);
        node = node->next;
    }
    fclose(fp);
}

void printAllAcc()
{
    ll *node;
    node = head;
    while (node != NULL)
    {
        printf("%20s %20s %d\n", node->name, node->password, node->status);
        node = node->next;
    }
}

//push into ll
void insert(ll **head, char name[20], char password[20], int status)
{
    ll *node = (ll *)malloc(sizeof(ll));
    strcpy(node->name, name);
    strcpy(node->password, password);
    node->status = status;
    node->next = *head;
    *head = node;
}

// Search a node
ll *searchAcc(char name[20])
{
    ll *curr = head;
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
