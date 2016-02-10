#include "server.h"

void print_help(){
    printf("Comands: q - shut down the server\n");
    printf("         p - print ports for the senders and recievers\n");
    printf("         l - print number of messages in queue\n");
    printf("         c - clear console\n");
    printf("         h - print this help\n\n");
    return;
};

void print_port(){
    printf("Port for senders:     %d\n",udp_broadcast_arg.port_sender_current);
    printf("Port for recievers:   %d\n",udp_broadcast_arg.port_reciever_current);
};

void print_queue(){
    printf("Number of messages in queue: %d of %d\n",udp_broadcast_arg.number_of_messages,MAX_LEN_QUEUE);
}

void console(){
    int c;
    while(1){
        c=getc(stdin);
        switch(c) {
            case 'p'    :
                print_port();
            break;
            case 'q'    :
                close(udp_broadcast_arg.brcst_recievers_sock);
                close(udp_broadcast_arg.brcst_senders_sock);
                pthread_mutex_unlock(&shared_mutex);
                msgctl(msg_queue, IPC_RMID, NULL);
                printf("Server shut down\n");
                return;
            break;
            case 'c'    :
                printf("\033[2J"); // Clear screen
                printf("\033[0;0H"); //Go to 0:0
            break;
            case 'l'    :
                print_queue();
            break;
            case 'h'    :
                print_help();
            break;
        }
    }
};

size_t socket_init_broadcast(int *sock,int port){

    int BroadcastPermission = 1;

    if ( (*sock = socket(AF_INET,SOCK_DGRAM,0)) < 0){
        perror("Error opening socket");
        return -1;
    }

    if ( setsockopt(*sock, SOL_SOCKET, SO_BROADCAST, (char *)&BroadcastPermission,sizeof(BroadcastPermission)) < 0 ) {
        perror("Error setsockopt to broadcast");
        return -1;
    }

    return 0;
};

size_t send_msg(int sock,struct sockaddr_in addr,void *msg,size_t len){
    size_t send_bytes = sendto (sock,&msg, len, 0,(struct sockaddr *)&addr, sizeof (addr));
    if( (int)send_bytes < 0 ){
        perror("Sending\n");
    }
    //printf("To port(%d) Message:%d --- bytes[%d]\n",(int)ntohs(addr.sin_port),(int)msg,send_bytes);
    return send_bytes;
};

void *udp_broadcast_senders(void *arg){

    /*Struct addres for broadcast to senders*/
    struct sockaddr_in broadcast_senders;
    broadcast_senders.sin_family = AF_INET;
    broadcast_senders.sin_port = htons(PORT_SENDERS);
    broadcast_senders.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    while(1){

        if( udp_broadcast_arg.number_of_messages >= MAX_LEN_QUEUE )
            continue;


        send_msg(udp_broadcast_arg.brcst_senders_sock,
                 broadcast_senders,
                 udp_broadcast_arg.port_sender_current,
                 sizeof(udp_broadcast_arg.port_sender_current) );

        usleep(BROADCAST_TIMEOUT_SENDERS);
    }
};

void *udp_broadcast_recievers(void *arg){


    /*Struct addres for broadcast to recievers*/
    struct sockaddr_in broadcast_recievers;
    broadcast_recievers.sin_family = AF_INET;
    broadcast_recievers.sin_port = htons(PORT_RECIEVERS);
    broadcast_recievers.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    while(1){

        if( udp_broadcast_arg.number_of_messages == 0 )
           continue;

        send_msg(udp_broadcast_arg.brcst_recievers_sock,
             broadcast_recievers,
             udp_broadcast_arg.port_reciever_current,
             sizeof(udp_broadcast_arg.port_reciever_current) );

        usleep(BROADCAST_TIMEOUT_RECIEVERS);
    }
};

void *listner(void *arg){
    int code = *(int *)arg;
    struct sockaddr_in serv_addr,cli_addr; // структура сокета сервера и клиента

    while(1){
        int	sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
           perror("ERROR opening socket");
           return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY; // сервер принимает подключения на все IP-адреса
        serv_addr.sin_port = 0;

        bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

        socklen_t len = sizeof(serv_addr);
        if (getsockname(sockfd, (struct sockaddr *)&serv_addr, &len) == -1) {
            perror("Getsockname");
            continue;
        }

        if(code == 0){   // True -> Client sender
            udp_broadcast_arg.port_sender_current = (int) ntohs(serv_addr.sin_port);
            //printf("Port for senders:     %d\n", ntohs(serv_addr.sin_port));
        }
        else{
            udp_broadcast_arg.port_reciever_current = (int) ntohs(serv_addr.sin_port);
            //printf("Port for recievers:   %d\n", ntohs(serv_addr.sin_port));
        }

        listen(sockfd,1);
        int clilen;
        int newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0){
            perror("ERROR on accept");
            continue;
        }

        pthread_t client_thr;
        if(code == 0) {    // True -> Client sender
            //printf("Sender connected!\n");
            pthread_create(&client_thr, NULL, client_sender, &newsockfd);
        }
        else{
            //printf("Sender connected!\n");
            pthread_create(&client_thr, NULL, client_reciever, &newsockfd);
        }


        close(sockfd);
    }
    return 0;

};

void *client_sender(void *arg){
    int sock = *(int *)arg;
    int bytes_recv;
    char message[MSG_BUF_LEN]="";

	if ( (bytes_recv = read(sock,&message[0],sizeof(message))) < 0 )
        //perror("ERROR reading from socket");
    message[bytes_recv] = '\0';
    add_message(message);
    //printf("Server <- %s\n",message);
	close(sock);
};

void *client_reciever(void *arg){
    int sock = *(int *)arg;
    int bytes_snd;
    char message[MSG_BUF_LEN]="";

    if(get_message(message) < 0){
    	close(sock);
    	return;
    }

	if ( bytes_snd = write(sock, message, strlen(message)) < 0 )
        perror("ERROR sending to socket");
    //printf("Server -> %s\n",message);
	close(sock);
};

void queue_init(){
    key_t msgkey = ftok(".", 'm');
    msgctl(msgget(msgkey,IPC_CREAT | 0660), IPC_RMID, NULL);

    if (( msg_queue = msgget(msgkey,IPC_CREAT | 0660)) < 0) {
      perror("open_queue");
      exit(1);
    }

    struct msqid_ds queue_qurent_status;

    if( msgctl(msg_queue,IPC_STAT,&queue_qurent_status) < 0 ){
        perror("msgctl IPC_STAT failed");
        exit(1);
    }

    queue_qurent_status.msg_qbytes = MAX_LEN_QUEUE * MSG_BUF_LEN;

    if( msgctl(msg_queue,IPC_SET,&queue_qurent_status) < 0 ){
        perror("msgctl IPC_SET failed");
        exit(1);
    }

    return;
};

void add_message(char *message){
    struct msgbuf client_server_message_snd;
    strcpy(client_server_message_snd.mtext,message);
    int len = sizeof(struct msgbuf) - sizeof(long);

    if(udp_broadcast_arg.number_of_messages == MAX_LEN_QUEUE)
        return;

    client_server_message_snd.mtype = MSG_TYPE;  /* тип сообщения должен быть положительным! */
    if( msgsnd( msg_queue, &client_server_message_snd,len, IPC_NOWAIT ) < 0) {
        perror("add_message");
        return;
    }

    pthread_mutex_lock(&shared_mutex);

    udp_broadcast_arg.number_of_messages++;

    pthread_mutex_unlock(&shared_mutex);

    printf("Add to queue:   %s\n",message);

    return;
};

int get_message(char *message){
    struct msgbuf client_server_message_rcv;
    int len = sizeof(struct msgbuf) - sizeof(long); /* тип сообщения должен быть положительным! */


    if( msgrcv( msg_queue, &client_server_message_rcv, len, MSG_TYPE, IPC_NOWAIT ) < 0 ) {
        perror("get_message");
        pthread_mutex_unlock(&shared_mutex);
        return -1;
    }

    strcpy(message,client_server_message_rcv.mtext);

    printf("Get from queue: %s\n",message);

    pthread_mutex_lock(&shared_mutex);

    udp_broadcast_arg.number_of_messages--;

    pthread_mutex_unlock(&shared_mutex);

    return 0;
};
