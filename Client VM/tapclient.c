#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>

#define BUFF_SIZE 2000
#define PORT_NUMBER 55555
#define SERVER_IP "10.0.2.4"

struct sockaddr_in peerAddr;

int createTapDevice() {
   int tapfd;
   struct ifreq ifr;
   memset(&ifr, 0, sizeof(ifr));
   ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
   tapfd = open("/dev/net/tun", O_RDWR);
   ioctl(tapfd, TUNSETIFF, &ifr);
   return tapfd;
}

int connectToUDPServer(){
    int sockfd;
    char *hello="Hello";
    struct sockaddr_in clientAddr;  // add this
    
    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(PORT_NUMBER);
    peerAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    // bind client to a fixed port
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddr.sin_port = htons(55556);  // fixed client port
    bind(sockfd, (struct sockaddr *) &clientAddr, sizeof(clientAddr));
    
    sendto(sockfd, hello, strlen(hello), 0,
                (struct sockaddr *) &peerAddr, sizeof(peerAddr));
    return sockfd;
}

void tapSelected(int tapfd, int sockfd){
    int  len;
    char buff[BUFF_SIZE];
    printf("Got a packet from TAP\n");
    bzero(buff, BUFF_SIZE);
    len = read(tapfd, buff, BUFF_SIZE);
    sendto(sockfd, buff, len, 0, (struct sockaddr *) &peerAddr,
                    sizeof(peerAddr));
}

void socketSelected(int tapfd, int sockfd){
    int  len;
    char buff[BUFF_SIZE];
    printf("Got a packet from the tunnel\n");
    bzero(buff, BUFF_SIZE);
    len = recvfrom(sockfd, buff, BUFF_SIZE, 0, NULL, NULL);
    write(tapfd, buff, len);
}

int main (int argc, char * argv[]) {
   int tapfd, sockfd;
   tapfd  = createTapDevice();
   sockfd = connectToUDPServer();
   while (1) {
     fd_set readFDSet;
     FD_ZERO(&readFDSet);
     FD_SET(sockfd, &readFDSet);
     FD_SET(tapfd, &readFDSet);
     select(FD_SETSIZE, &readFDSet, NULL, NULL, NULL);
     if (FD_ISSET(tapfd,  &readFDSet)) tapSelected(tapfd, sockfd);
     if (FD_ISSET(sockfd, &readFDSet)) socketSelected(tapfd, sockfd);
  }
}