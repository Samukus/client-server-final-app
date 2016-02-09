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
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    int i =0;
    while(bind(*sock,(struct sockaddr *)&server, sizeof(server)) < 0){
        continue;
    }
    return sizeof(server);
};

int main(){
    int socket_rcv;
    struct sockaddr_in server; // структуры адреса сервера

    while(1){
        int new_port = 0;
        socket_init_udp(&socket_rcv,PORT_SENDERS);
        int n = recvfrom(socket_rcv,&new_port,sizeof(new_port),NULL,NULL,NULL);
        if( n <= 0 ){
            close(socket_rcv);
            continue;
        }
        printf("Port for TCP: %d --- [%d bytes]\n",new_port,n);
        close(socket_rcv);
        usleep(50000);
    }


    return(0);
}
