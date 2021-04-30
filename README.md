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
  Setelah itu, membuat thread dari void input dan output chat sampai selesai
```cpp
std::thread thInp(chatInput, sockfd);
std::thread thOut(chatOutput, sockfd, clientName);
thInp.join();
thOut.join();
```
  function mengirim pesan
  mengirim string ke server menggunakan `sendto`
  jika input adalah "LEAVE ROOM" maka akan keluar dari function dan mengirim "&" ke server untuk memberi tahu bahwa client meninggalkan chat
```cpp
void chatOutput(int sockfd, char clientName[MAXLINE]){
    while(1){
        char clientMsg[MAXLINE]="";
        char msg[MAXLINE];
        strcpy(msg,clientName);
        scanf("%[^\n]s", &clientMsg);
        getchar();
        if(!strcmp(clientMsg,"LEAVE ROOM")){
            strcat(msg," &");    
            sendto(sockfd, (const char *)msg, strlen(msg),
            MSG_CONFIRM, (const struct sockaddr *) &servaddr,
                sizeof(servaddr));
            break;
        }
        printf("You: %s\n", clientMsg);
        strcpy(msg,clientName);
        strcat(msg,": ");
        strcat(msg,clientMsg);
        sendto(sockfd, (const char *)msg, strlen(msg),
            MSG_CONFIRM, (const struct sockaddr *) &servaddr,
                sizeof(servaddr));
    }
}
```
  function menerima pesan
  pesan yang diterima akan ada diterima dari server akan dimasukkan ke buffer menggunakan `recvfrom` dan panjang string buffer adalah n
  jika menerima pesan "@" maka function akan selesai dan menampilkan pesan keluar room.
```cpp
void chatInput(int sockfd){
    while(1){
        char buffer[MAXLINE];
        sockaddr servaddr;
        socklen_t len;
        int n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
        buffer[n] = '\0';
        if(!strcmp(buffer,"@")){
            printf("you have left the room\n");
            break;
        }
        printf("%s\n", buffer);
    }
}
```

## chatServer.cpp
  Pada awal program jalan, program server juga membuat file descriptor socket dan mengisi data server.
  Server juga membuat array yang berisi semua alamat client
```cpp
struct sockaddr_in allClients[100];
// Creating socket file descriptor
if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
perror("socket creation failed");
exit(EXIT_FAILURE);
}
```
```cpp
servaddr.sin_family = AF_INET; // IPv4
servaddr.sin_addr.s_addr = INADDR_ANY;
servaddr.sin_port = htons(PORT);
```
  Bind socket ke alamat server
```cpp
if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
{
  perror("bind failed");
  exit(EXIT_FAILURE);
}
```
  Setelah itu, server terus menerus menunggu pesan dari client di dalam while loop
```cpp
n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*) &cliaddr, &len);
```
  Jika menerima tanda "@", client yang menghubungi server adalah client baru
```cpp
if(buffer[n-1]=='@'){
	allClients[groupMembers++]=cliaddr;
	n--;
}
```
  Jika menerima tanda "&", client yang menghubungi server akan leave chat
```cpp
if(buffer[n-1]=='&'){
	sendto(sockfd, "@", 1, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
	buffer[n-1]='\0';
	strcat(buffer,"has left!");
}
```
  Di akhir loop, mengirim buffer ke semua client selain client yang mengirim pesan.
```cpp
printf("%s\n", buffer);
for(int i=0;i<groupMembers;i++)
	if(allClients[i].sin_addr.s_addr!=cliaddr.sin_addr.s_addr || allClients[i].sin_port!=cliaddr.sin_port){
   	sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &allClients[i], len);
  }
```
