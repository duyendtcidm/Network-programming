#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //for exit(0);
#include <sys/socket.h>
#include <errno.h> //For errno - the error number
#include <netdb.h> //hostent
#include <netinet/in.h>
#include <arpa/inet.h>

int isValidIpAddress(char *ipAddress);
int hostname_to_ip(char *hostname);
void ip_to_host(char *ipAddress);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Invalid parameter \n");
        exit(1);
    }

    if (strcmp(argv[1], "1") == 0)
    {
        ip_to_host(argv[2]);
    }
    else if (strcmp(argv[1], "2") == 0)
    {
        hostname_to_ip(argv[2]);
    }
    return 0;
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

    if (isValidIpAddress(hostname))
    {
        printf("Wrong parameter\n");
        exit(1);
    }

    he = gethostbyname(hostname);
    if (he == NULL)
    {
        // herror("gethostbyname");
        printf("Not found information!\n");
        return 1;
    }
    else
    {
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

void ip_to_host(char *ipAddress)
{
    // struct sockaddr_in sa;
    // socklen_t len = sizeof(struct sockaddr_in);
    // char buffer[NI_MAXHOST];

    // if (!isValidIpAddress(ipAddress)) {
    //     if (gethostbyname(ipAddress) != NULL) {
    //         printf("Wrong parameter\n");
    //         exit(1);
    //     } else
    //     printf("Not found information\n");
    //     exit(1);
    // }

    // memset(&sa, 0, sizeof(struct sockaddr_in));

    // sa.sin_family = AF_INET;
    // sa.sin_addr.s_addr = inet_addr(ipAddress);

    // if (getnameinfo((struct sockaddr *) &sa, len, buffer, sizeof(buffer),
    //     NULL, 0, NI_NAMEREQD)) {
    //     printf("Not found information\n");
    // } else {
    //     printf("Official name: %s\n", buffer);
    //     printf("Alias name:\n%s\n", buffer);
    // }
    char *ipstr = ipAddress;
    struct in_addr ip;
    struct hostent *hp;

    if (!inet_aton(ipstr, &ip))
        printf("Can't parse IP address %s", ipstr);

    if ((hp = gethostbyaddr((const void *)&ip, sizeof ip, AF_INET)) == NULL)
    {
        printf("no name associated with %s\n", ipstr);
        return;
    }

    printf("Official name: %s\n", ipstr);
                printf("Alias name:\n");
                printf("%s\n",hp->h_name );      
    return;
}
