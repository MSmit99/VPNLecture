#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>

#define PORT_NUMBER 55555
#define BUFF_SIZE 2000

struct sockaddr_in peerAddr;

int createTapDevice() {
   int tapfd;
   struct ifreq ifr;
   memset(&ifr, 0, sizeof(ifr));
   ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
   tapfd = open("/dev/net/tun", O_RDWR);  // /dev/net/tun is correct for both TUN and TAP
   ioctl(tapfd, TUNSETIFF, &ifr);
   return tapfd;
}

int initUDPServer() {
    int sockfd;
    struct sockaddr_in server;
    char buff[100];
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT_NUMBER);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bind(sockfd, (struct sockaddr*) &server, sizeof(server));
    // Wait for the TAP client to "connect".
    bzero(buff, 100);
    int peerAddrLen = sizeof(struct sockaddr_in);
    int len = recvfrom(sockfd, buff, 100, 0,
                (struct sockaddr *) &peerAddr, &peerAddrLen);
    printf("Connected with the client: %s\n", buff);
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
   sockfd = initUDPServer();
   // Enter the main loop
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