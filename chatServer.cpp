// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
using namespace std;

#define PORT	8080
#define MAXLINE 1024

// Driver code
int main() {
	int sockfd,groupMembers=0;
	char buffer[MAXLINE];
	char *hello ;
	struct sockaddr_in servaddr, cliaddr;
	struct sockaddr_in allClients[100];
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);
	
	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	int n;
    socklen_t len= sizeof(cliaddr);
    while(1){
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*) &cliaddr, &len);
		if(buffer[n-1]=='@'){
			allClients[groupMembers++]=cliaddr;
			n--;
		}
        buffer[n] = '\0';
        printf("%s\n", buffer);
        for(int i=0;i<groupMembers;i++)
			if(allClients[i].sin_addr.s_addr!=cliaddr.sin_addr.s_addr || allClients[i].sin_port!=cliaddr.sin_port){
            	sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &allClients[i], len);
        }
    }
	return 0;
}
