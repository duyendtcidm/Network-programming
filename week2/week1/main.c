#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<errno.h> 
#include<netdb.h>	
#include<netinet/in.h>
#include<arpa/inet.h>

typedef struct list{
    char name[20];
    char password[20];
    int status;
    int loginStatus;
    char homepage[100];
    struct list* next;
}ll;

ll* head = NULL;

void readFile();
void insert(ll** head, char name[20], char password[20], int status, char homepage[100]);
ll* searchAcc(char name[20]);
void printAllAcc();
void registerAcc();
void active();
void signIn();
void search();
void changePassword();
void signout();
//
int isValidIpAddress(char *ipAddress);
int hostname_to_ip(char *hostname);
void ip_to_host(char *ipAddress);

int main() {
    readFile();
    printAllAcc();

    int choice;
    do {
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
        scanf("%d", &choice);

        switch(choice) {
            case 1:
             registerAcc();
                break;
            case 2:
               active();
                break;
            case 3:
                signIn();
                break;
            case 4:
                search();
                break;
            case 5:
                changePassword();
                break;
            case 6:
                signout();
                break;
            case 7:
            

                break;
            case 8:
                break;
            default:
                printf("----");
                break;

        }
    }while(choice>0 && choice < 7);   

    return 0;
}

void readFile(){
    char* name = malloc(20*sizeof(char));
    char* password = malloc(20*sizeof(char));
    int status;

    FILE* fp;
    fp = fopen("account.txt", "r");
    if( fp == NULL) {
        printf("Can't open file!\n");
    } else {
         while(!feof(fp)) {
            ll* node = (ll*)malloc(sizeof(ll));
            fscanf(fp, "%s %s %d\n", name, password, &status);
            insert(&head, name, password, status);
        }  
    }
    free(name);
    free(password);
    fclose(fp);   
}

void writeFile(){
    FILE *fp;
    ll *node;
    node = head;
    fp = fopen("account.txt", "wb");
    while (node != NULL)
    {
        fprintf(fp, "%s %s %d\n", node->name, node->password, node->status);
        node = node->next;
    }
    fclose(fp);
}

void printAllAcc(){
    ll *node;
    node = head;
    while (node != NULL)
    {
        printf("%20s %20s %d\n", node->name, node->password, node->status);
        node = node->next;
    }
}

//push into ll
void insert(ll** head, char name[20], char password[20], int status, char homepage[100]) {
    ll* node = (ll*)malloc(sizeof(ll));
    strcpy(node->name, name);
    strcpy(node->password, password);
    node->status = status;
    strcpy(node->homepage, homepage);
    node->next = *head;
    *head = node;
}

// Search a node
ll* searchAcc(char name[20]) {
    ll* curr = head; 
    while (curr != NULL)
    {
        if (!strcmp(name, curr->name))
        {
            return curr;
            break;
        }
        curr = curr->next;
    }
    return NULL;
}

//
void registerAcc(){
    char name[20];
    char password[20];
    char homepage[100];

    printf("Username: ");
    scanf("%s", trim(name));
    // check tab, space

    if (searchAcc(name) != NULL) {
        printf("Existed account! \n");
        return;
    } else {
        printf("Password: ");
        scanf("%s", trim(password));
        printf("Homepage: ");
        scanf("%s", homepage);
        insert(&head, name, password, 2);
        printf("Successfully register.\n");
        writeFile();
    }
}

void active() {
    const char activeCode[8] = "20184085";
    char name[20];
    char password[20];
    char ac[8];         // user's received active code
    int times = 0;

    ll* node;   
    do{
        printf("Username:");
        scanf("%s", name);
        printf("Password:");
        scanf("%s", password);
        printf("Your active code:");
        scanf("%s", ac);

        if (strcmp(activeCode,ac) == 0){
            node = searchAcc(name);
            if (node != NULL){
                if (strcmp(node->password, password) == 0){
                    if (node->status == 2) {
                        node->status = 1;
                        printf("Active account successfully ! \n");
                        return;
                    } else {
                        printf("Actived account!\n");
                        return;
                    }
                } else {
                    printf("Incorect password, try again! \n");
                    times++;
                }
            } else {
                printf("Not existed account! \n");
            }
        }else {
            times++;
            printf("Incorrect active code, try again \n");
        }
    } while (times != 3);

    if (times == 3 && searchAcc(name) != NULL ){
        node =  searchAcc(name);
        node->status = 0;
        printf("Active failed, account blocked!\n");
    } else {
        printf("Too much fail request !\n");
    }
}

void signIn() {
    char name[20];
    char password[20];
    int times = 0;

    ll* node;   
    do{
        printf("Username:");
        scanf("%s", name);
        printf("Password:");
        scanf("%s", password);

            node = searchAcc(name);
            if (node != NULL){
                if (strcmp(node->password, password) == 0 && node->status == 1 && node->loginStatus != 1){
                        node->loginStatus = 1;
                        printf("Log in successfully! \n");
                        return;
                } else if (node->status != 0 ){
                            printf("accounts blocked or not active ! \n");
                            return;
                            }else if (node->loginStatus == 1) {
                                printf("This account is already logged in \n");
                                return;
                            } 
                else {
                    printf("Incorect password, try again! \n");
                    times++;
                }
            } else {
                printf("Not existed account! \n");
            }
    } while (times != 3);

    if (times == 3 && searchAcc(name) != NULL ){
        node =  searchAcc(name);
        node->status = 0;
        printf("Log in fail, account blocked!\n");
    } else {
        printf("Too much fail request !\n");
    }
}

void search() {
    // if (node->loginStatus != 1) {
    //     printf("You haven't signed in yet! \n");
    //     return;
    // }
    char name[20];
    printf("Enter your username you want to find:");
    scanf("%s", name);
    ll* node1 = searchAcc(name);
    if (node1 != NULL) {
        printf("Username : %s \n", node1->name);
        }
    switch(node1->status) {
            case 0:
                printf("Account is blocked.\n");
                return;
            case 1:
                printf("Account is active.\n");
                return;
            default:
                printf("Account isn't active or exited.\n");
    }
    return;
}

void changePassword() {
    ll* node;
    char name[20];
    char oldpass[20];           //old password
    char new1[20];              //new password
    char new2[20];              // check new password

    printf("Username:");
    scanf("%s", name);

    node = searchAcc(name);
    if (node != NULL) {
        if (node->loginStatus == 1) {
        printf("Enter old password:");
        scanf("%s", oldpass);
        if (strcmp(node->password, oldpass) == 0) {
            printf("Enter new password:");
            scanf("%s", new1);
            printf("Enter new password again:");
            scanf("%s", new2);
            
            if(strcmp(new1, new2) == 0) {
                strcpy(node->password, new1);
                printf("Change password successfully!\n");
            } else {
                printf("Incorrect! Enter your password again!\n");
            }
        } else {
            printf("Wrong password");
        }
    }else 
        printf("Account is not loged in");
    } else 
        printf("Not existed account.\n");
    
}

void signout() {
    ll* node;
    char name[20];

    node = searchAcc(name);
    if (node != NULL) {
        if (node->loginStatus == 1) {
            node->loginStatus = 0;
            printf("Log out successfully!\n");
            return;
        } else {
            printf("You haven't log in yet!\n");
            return;
        }
    } else 
        printf("Not existed account.\n");
    return;
}

int isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result;
}

int hostname_to_ip(char *hostname)
{
    struct hostent *he;
    struct in_addr **addr_list;

    if (isValidIpAddress(hostname)) {
        printf("Wrong parameter\n");
        exit(1);
    }

    he = gethostbyname(hostname);
    if (he == NULL)
    {
        // herror("gethostbyname");
        printf("Not found information!\n");
        return 1;
    } else {
        addr_list = (struct in_addr **)he->h_addr_list;

        printf("Official IP: %s\n", inet_ntoa(*(struct in_addr *)he->h_name));
        printf("Alias IP:\n");

        for (int i = 0; addr_list[i] != NULL; i++)
        {
            printf("%s\n", inet_ntoa(*addr_list[i]));
        }
    }
    return 1;
}

void ip_to_host(char *ipAddress) {
    struct sockaddr_in sa;    
    socklen_t len = sizeof(struct sockaddr_in);         
    char buffer[NI_MAXHOST];

    if (!isValidIpAddress(ipAddress)) {
        if (gethostbyname(ipAddress) != NULL) {
            printf("Wrong parameter\n");
            exit(1);
        } else
        printf("Not found information\n");
        exit(1);
    }

    memset(&sa, 0, sizeof(struct sockaddr_in));

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(ipAddress);
    
    if (getnameinfo((struct sockaddr *) &sa, len, buffer, sizeof(buffer), 
        NULL, 0, NI_NAMEREQD)) {
        printf("Not found information\n");
    } else {
        printf("Official name: %s\n", buffer);
        printf("Alias name:\n%s\n", buffer);
    }                           
}








