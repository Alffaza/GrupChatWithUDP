// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>

#define PORT	8080
#define MAXLINE 1024

// Driver code
int main() {
	int sockfd;
	char buffer[MAXLINE],clientName[MAXLINE];
	struct sockaddr_in	 servaddr;
    int n;
    socklen_t len;
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY;
	
    printf("Please enter your name:\n");
    scanf("%[^\n]s", clientName);
    getchar();
    char joinMsg[MAXLINE];
    strcpy(joinMsg,clientName);
    strcat(joinMsg," has joined!");
    sendto(sockfd, (const char *)joinMsg, strlen(joinMsg),
            MSG_CONFIRM, (const struct sockaddr *) &servaddr,
                sizeof(servaddr));

    while(1){
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
        buffer[n] = '\0';

        printf("%s\n", buffer);
        char clientMsg[MAXLINE];
        scanf("%[^\n]s", &clientMsg);
        getchar();
        char msg[MAXLINE];
        strcpy(msg,clientName);
        strcat(msg,": ");
        strcat(msg,clientMsg);
        sendto(sockfd, (const char *)msg, strlen(msg),
            MSG_CONFIRM, (const struct sockaddr *) &servaddr,
                sizeof(servaddr));

    }
	close(sockfd);
	return 0;
}
