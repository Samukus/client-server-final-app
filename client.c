#include "client.h"

int socket_init_udp(int *sock,int port){

    if ((*sock=socket(AF_INET,SOCK_DGRAM,0))<0){
        //perror("Opening socket");
        return -1;
    }

    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = 500;
    if(setsockopt(*sock, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t)) == -1){
        perror("Setting SO_RCVTIMEO option in UDP socket for destination RX: ");
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    int i = 0;
    while(bind(*sock,(struct sockaddr *)&server, sizeof(server)) < 0){
        //perror("Binding error");
        continue;
    }
    return 0;
};

int create_message(char *message){
    buf_message.sleep_time = rand()%10;
    buf_message.len = rand()%(MSG_BUF_LEN-20)+10;
    int i;

    for(i=0;i<buf_message.len-1;i++)
        buf_message.message_text[i]=(char)(rand()%25+97);

    buf_message.message_text[buf_message.len]='\0';
    buf_message.len = strlen(buf_message.message_text);
    sprintf(message,"%d#%d#%s",buf_message.sleep_time,buf_message.len,buf_message.message_text);
    return 0;
};

int parse_message(char *message){
    int i=0;
    int j=0;
    char buf[10];

    for(j=0;i<strlen(message);i++,j++)
        if(message[i] != '#')
            buf[j] = message[i];
        else{
            buf[j]='\0';
            buf_message.sleep_time = atoi(buf);
            i++;
            break;
        }
    for(j=0;i<strlen(message);i++,j++)
                if(message[i] != '#')
            buf[j] = message[i];
        else{
            buf[j]='\0';
            buf_message.len = atoi(buf);
            i++;
            break;
        }
    strcpy(buf_message.message_text,message+i);

    return 0;
}

int get_server_add(struct sockaddr_in *server_addr,int port){

    int socket_rcv;
    socket_init_udp(&socket_rcv,port);

    if(socket_rcv < 0){
        close(socket_rcv);
        //perror("init");
        return -1;
    }

    int current_port;

    bzero((char *) server_addr, sizeof(struct sockaddr_in));
    int socket_len = sizeof(struct sockaddr_in);

    int n = recvfrom(socket_rcv,&current_port,sizeof(current_port),NULL,server_addr,&socket_len);

    if( n <= 0 ){
        close(socket_rcv);
        //perror("recieve");
        return -1;
    }
    close(socket_rcv);
    return current_port;
};

int send_msg(struct sockaddr_in server_addr,int current_port,char *message){

    server_addr.sin_port = htons(current_port);

    int my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0){
        //perror("ERROR opening socket");
        close(my_sock);
        return -1;
    }

    if (connect(my_sock,(struct sockaddr *)&server_addr,sizeof(struct sockaddr_in)) < 0){
        //perror("ERROR connecting");
        close(my_sock);
        return -1;
    }

    char ipAddress[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server_addr.sin_addr), ipAddress, INET_ADDRSTRLEN);

    int bytes_snd = send(my_sock, &message[0], strlen(&message[0]), 0);
    if(bytes_snd >= 0)
        printf("%-55s --> [%s:%d]\n",message,ipAddress,current_port);
    close(my_sock);
    return 0;

};

int rcv_msg(struct sockaddr_in server_addr,int current_port,char *message){

    server_addr.sin_port = htons(current_port);

    int my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0){
        //perror("ERROR opening socket");
        close(my_sock);
        return -1;
    }

    if (connect(my_sock,(struct sockaddr *)&server_addr,sizeof(struct sockaddr_in)) < 0){
        //perror("ERROR connecting");
        close(my_sock);
        return -1;
    }

    char ipAddress[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server_addr.sin_addr), ipAddress, INET_ADDRSTRLEN);

    bzero(message,sizeof(message));
    int bytes_tcp = recv(my_sock, message, MSG_BUF_LEN, 0);

    if(bytes_tcp >= 0){
        parse_message(message);
        buf_message.message_text[buf_message.len] = '\0';
        printf("%d#%d%-55s <-- [%s:%d]\n",buf_message.sleep_time,
                                          buf_message.len,
                                          buf_message.message_text,
                                          ipAddress,
                                          current_port);
    }

    close(my_sock);
    return 0;

}



