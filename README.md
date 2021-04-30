# GrupChatWithUDP
Tugas3 ichiro

Pengguna (client) mengirim pesan ke server lalu server mengirim pesan yang dikirim ke semua client lain.
Program memakai library socket.
```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
```
## chatClient.cpp
  Client memakai library threading untuk dapat mengambil input dan output sekaligus
```cpp
#include <thread>
```
  Pada awal program jalan, program membuat file descriptor socket dan mengisi data server
```cpp
if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
  perror("socket creation failed");
  exit(EXIT_FAILURE);
}
```
```cpp
servaddr.sin_family = AF_INET;
servaddr.sin_port = htons(PORT);
servaddr.sin_addr.s_addr = INADDR_ANY
```
  Meminta nama client, dan mengirim pesan join ke server
```cpp
printf("Please enter your name: ");
scanf("%[^\n]s", clientName);
getchar();
printf("welcome to the room!\n");
char joinMsg[MAXLINE];
strcpy(joinMsg,clientName);
strcat(joinMsg," has joined!");
strcat(joinMsg,"@");
sendto(sockfd, (const char *)joinMsg, strlen(joinMsg), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
```
  Setelah itu, membuat thread dari void input dan output chat
```cpp
std::thread thInp(chatInput, sockfd);
std::thread thOut(chatOutput, sockfd, clientName);
```
## chatServer.cpp
