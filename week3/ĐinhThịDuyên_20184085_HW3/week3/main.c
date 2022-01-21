#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct list
{
    char name[20];
    char password[20];
    int status;
    char homepage[100];
    struct list *next;
} ll;

ll *head = NULL;

void readFile();
void insert(ll **head, char name[20], char password[20], int status, char homepage[100]);
ll *searchAcc(char name[20]);
void printAllAcc();

//
void registerAcc();
void active();
int signIn();
void search(int loginStatus);
void changePassword(int loginStatus);
int signout(int loginStatus);

//
// int isValidIpAddress(char *ipAddress);
int hostname_to_ip(int loginStatus);
int ip_to_host(int loginStatus);

int main()
{
    readFile();
    int loginStatus = 0;

    int choice;
    do
    {
        printf("\n");
        printf("\n");
        printf("USER MANAGEMENT PROGRAM\n");
        printf("-------------------------------------\n");
        printf("1. Register\n");
        printf("2. Active\n");
        printf("3. Sign in\n");
        printf("4. Search\n");
        printf("5. Change password\n");
        printf("6. Sign out\n");
        printf("7. Homepage with domain name\n");
        printf("8. Homepage with IP address\n");
        printf("Your choice (1-8, other to quit):\n");
        printAllAcc();
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            registerAcc();
            break;
        case 2:
            active();
            break;
        case 3:
            loginStatus = signIn(loginStatus);
            break;
        case 4:
            search(loginStatus);
            break;
        case 5:
            changePassword(loginStatus);
            break;
        case 6:
            loginStatus = signout(loginStatus);
            break;
        case 7:
            hostname_to_ip(loginStatus);
            break;
        case 8:
            ip_to_host(loginStatus);
            break;
        default:
            printf("----");
            break;
        }
    } while (choice > 0 && choice < 9);

    return 0;
}

// int isValidIpAddress(char *ipAddress)
// {
//     struct sockaddr_in sa;
//     int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
//     return result;
// }

int hostname_to_ip(int loginStatus)
{
    char name[20];
    ll *node;
    if (loginStatus != 0)
    {
        printf("Enter username:\n");
        scanf("%s", name);
        node = searchAcc(name);
        if (node != NULL)
        {
            struct hostent *hostname;
            struct in_addr **addr_list;
            struct in_addr ip;

            if (inet_aton(node->homepage, &ip))
                // printf("Can't parse IP address %s", node->homepage);
                {
                printf("This is an ip address, cannot find anymore\n");
                printf("%s", node->homepage);
                return 1;
            }

            hostname = gethostbyname(node->homepage);
            if (hostname == NULL)
            {
                printf("%s:\t", node->homepage);
                printf("Not found information!\n");
                return 1;
            }
            else
            {
                addr_list = (struct in_addr **)hostname->h_addr_list;

                printf("Official IP: %s\n", inet_ntoa(*(struct in_addr *)hostname->h_name));
                printf("Alias IP:\n");

                for (int i = 0; addr_list[i] != NULL; i++)
                {
                    printf("%s\n", inet_ntoa(*addr_list[i]));
                }
            }
            return 1;
        }
        else
        {
            printf("No information!\n");
            return 1;
        }
    }
    else
        printf("You haven't login yet!\n");
    return 1;
}

int ip_to_host(int loginStatus)
{
    char name[20];
    ll *node;
    if (loginStatus != 0)
    {
        printf("Enter username: \n");
        scanf("%s", name);
        node = searchAcc(name);
        if (node != NULL)
        {

            char *ipstr = node->homepage;
            struct in_addr ip;
            struct hostent *hp;

            if (!inet_aton(ipstr, &ip)) {
                printf("Can't parse IP address %s", ipstr);
                return 0;
            }

            if ((hp = gethostbyaddr((const void *)&ip, sizeof ip, AF_INET)) == NULL) {
                printf("Not found information \n");
                return 0;
            }               

            // printf("name associated with %s is %s\n", ipstr, hp->h_name);
                printf("Official name: %s\n", ipstr);
                printf("Alias name:\n");
                printf("%s\n",hp->h_name );                    
            return 1;
        } else {
            printf("Not existed account!\n");
            return 1;
        }
    }
    else
    {
        printf("You haven't logined yet!\n");
    }
    return 0;
}

void readFile()
{
    char *name = malloc(20 * sizeof(char));
    char *password = malloc(20 * sizeof(char));
    int status;
    char *homepage = malloc(100 * sizeof(char));

    FILE *fp;
    fp = fopen("account.txt", "r");
    if (fp == NULL)
    {
        printf("Can't open file!\n");
    }
    else
    {
        while (!feof(fp))
        {
            ll *node = (ll *)malloc(sizeof(ll));
            fscanf(fp, "%s %s %d %s\n", name, password, &status, homepage);
            insert(&head, name, password, status, homepage);
        }
    }
    free(name);
    free(password);
    free(homepage);
    fclose(fp);
}

void writeFile()
{
    FILE *fp;
    ll *node;
    node = head;
    fp = fopen("account.txt", "w");
    while (node != NULL)
    {
        fprintf(fp, "%s %s %d %s\n", node->name, node->password, node->status, node->homepage);
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
        printf("%20s %20s %d %30s\n", node->name, node->password, node->status, node->homepage);
        node = node->next;
    }
}

//push into ll
void insert(ll **head, char name[20], char password[20], int status, char homepage[100])
{
    ll *node = (ll *)malloc(sizeof(ll));
    strcpy(node->name, name);
    strcpy(node->password, password);
    node->status = status;
    strcpy(node->homepage, homepage);
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

//
void registerAcc()
{
    char name[20];
    char password[20];
    char homepage[100];

    printf("Username: ");
    scanf("%s", name);

    if (searchAcc(name) != NULL)
    {
        printf("Existed account! \n");
        return;
    }
    else
    {
        printf("Password: ");
        scanf("%s", password);
        printf("Homepage: ");
        scanf("%s", homepage);
        insert(&head, name, password, 2, homepage);
        printf("Successfully register.\n");
        writeFile();
    }
}

void active()
{
    const char activeCode[8] = "20184085";
    char name[20];
    char password[20];
    char ac[10]; // user's received active code
    int times = 0;

    ll *node;
    do
    {
        printf("Username:");
        scanf("%s", name);
        printf("Password:");
        scanf("%s", password);
        printf("Your active code:");
        scanf("%s", ac);

        if (strncmp(activeCode, ac, 8) == 0)
        {
            node = searchAcc(name);
            if (node != NULL)
            {
                if (strcmp(node->password, password) == 0)
                {
                    if (node->status == 2)
                    {
                        node->status = 1;
                        printf("Active account successfully ! \n");
                        return;
                    }
                    else
                    {
                        printf("Actived account!\n");
                        return;
                    }
                }
                else
                {
                    printf("Incorect password, try again! \n");
                    times++;
                }
            }
            else
            {
                printf("Not existed account! \n");
                return;
            }
        }
        else
        {
            times++;
            printf("Incorrect active code, try again \n");
        }
    } while (times != 4);

    if (times == 4 && searchAcc(name) != NULL)
    {
        node = searchAcc(name);
        node->status = 0;
        printf("Active failed, account blocked!\n");
    }
    else
    {
        printf("Too much fail request !\n");
    }
}

int signIn(int loginStatus)
{
    char name[20];
    char password[20];
    int times = 0;

    ll *node;
    if (loginStatus == 0)
    {
        do
        {
            printf("Username:");
            scanf("%s", name);
            printf("Password:");
            scanf("%s", password);

            node = searchAcc(name);
            if (node != NULL)
            {
                if (strcmp(node->password, password) == 0 && node->status == 1)
                {
                    loginStatus = 1;
                    printf("Login succesfully!\n");
                    return loginStatus;
                }
                else if (node->status == 0)
                {
                    printf("Accounts is blocked! \n");
                    return 0;
                }
                else if (node->status == 2)
                {
                    printf("Accounts isn't active! \n");
                    return 0;
                }
                else
                {
                    printf("Incorect password, try again! \n");
                    times++;
                }
            }
            else
            {
                printf("Not existed account! \n");
            }
        } while (times != 3);
    }
    else
    {
        printf("Other account has loggin! Login forbitted!\n");
        return 1;
    }

    if (times == 3 && searchAcc(name) != NULL)
    {
        node = searchAcc(name);
        node->status = 0;
        printf("Log in fail, account blocked!\n");
    }
    else
    {
        printf("Too much fail request !\n");
    }
    return loginStatus;
}

void search(int loginStatus)
{
    char name[20];
    if (loginStatus != 0)
    {
        printf("Enter your username you want to find:");
        scanf("%s", name);
        ll *node1 = searchAcc(name);
        if (node1 != NULL)
        {
            printf("Username : %s \n", node1->name);
            switch (node1->status)
            {
            case 0:
                printf("Account is blocked.\n");
                return;
            case 1:
                printf("Account is active.\n");
                return;
            default:
                printf("Account isn't active.\n");
                return;
            }
        }
        else
        {
            printf("Account isn't exited.\n");
            return;
        }
    }
    else
        printf("You haven't log in yet!\n");
    return;
}

void changePassword(int loginStatus)
{
    ll *node;
    char name[20];
    char oldpass[20]; //old password
    char new1[20];    //new password
    char new2[20];    // check new password

    if (loginStatus != 0)
    {
        printf("Username:");
        scanf("%s", name);

        node = searchAcc(name);
        if (node != NULL)
        {
            printf("Enter old password:");
            scanf("%s", oldpass);
            if (strcmp(node->password, oldpass) == 0)
            {
                printf("Enter new password:");
                scanf("%s", new1);
                printf("Enter new password again:");
                scanf("%s", new2);

                if (strcmp(new1, new2) == 0)
                {
                    strcpy(node->password, new1);
                    printf("Change password successfully!\n");
                }
                else
                {
                    printf("Incorrect! Enter your password again!\n");
                }
            }
            else
            {
                printf("Wrong password\n");
                return;
            }
        }
        else
        {
            printf("Not existed account.\n");
            return;
        }
    }
    else
        printf("You haven't login yet.\n");
    return;
}

int signout(int loginStatus)
{
    ll *node;
    char name[20];
    // int st;

    if (loginStatus != 0)
    {
        printf("Username: ");
        scanf("%s", name);

        node = searchAcc(name);
        if (node != NULL)
        {
            loginStatus = 0;
            printf("Log out successfully!\n");
            return 0;
        }
        else
        {
            printf("Not exited account!\n");
            return 1;
        }
    }
    else
        printf("You haven't login yet!\n");
    return loginStatus;
}
