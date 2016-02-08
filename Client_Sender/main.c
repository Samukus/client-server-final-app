#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <defs.h>

#define PORT 2501


size_t socket_init_udp(int *sock,int port){

    if ((*sock=socket(AF_INET,SOCK_DGRAM,0))<0){
        perror("Opening socket");
        return -1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(*sock,(struct sockaddr *)&server, sizeof(server))){
        perror("Binding");
        return -1;
    }
    return sizeof(server);
};

int main(){
    int socket_rcv;
    struct sockaddr_in server; // структуры адреса сервера

    socket_init_udp(&socket_rcv,PORT_SENDERS);



    while(1){
        int new_port;
        char buff[64]="";
        int n = recvfrom(socket_rcv,&buff,sizeof(buff),NULL,NULL,NULL);
        printf("Port for TCP: %s --- [%d bytes]\n",buff,n);
    }


    return(0);
}
