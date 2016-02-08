#include "server.h"

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
    printf("To port(%d) Message:%d --- bytes[%d]\n",(int)ntohs(addr.sin_port),(int)msg,send_bytes);
    return send_bytes;
};

void *udp_broadcast_senders(void *arg){
    struct thread_broadcasting_argument *thread_brdcast_arg = (struct thread_broadcasting_argument*)arg;

    /*Struct addres for broadcast to senders*/
    struct sockaddr_in broadcast_senders;
    broadcast_senders.sin_family = AF_INET;
    broadcast_senders.sin_port = htons(PORT_SENDERS);
    broadcast_senders.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    while(1){
        pthread_mutex_lock(&shared_mutex);

        send_msg(thread_brdcast_arg->brcst_senders_sock,
                 broadcast_senders,
                 udp_broadcast_arg.port_sender_current,
                 sizeof(udp_broadcast_arg.port_sender_current) );

        pthread_mutex_unlock(&shared_mutex);

        sleep(BROADCAST_TIMEOUT_SENDERS);
    }
};

void *udp_broadcast_recievers(void *arg){
    struct thread_broadcasting_argument *thread_brdcast_arg = (struct thread_broadcasting_argument*)arg;
    /*Struct addres for broadcast to recievers*/
    struct sockaddr_in broadcast_recievers;
    broadcast_recievers.sin_family = AF_INET;
    broadcast_recievers.sin_port = htons(PORT_RECIEVERS);
    broadcast_recievers.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    while(1){
        pthread_mutex_lock(&shared_mutex);

        send_msg(thread_brdcast_arg->brcst_recievers_sock,
             broadcast_recievers,
             thread_brdcast_arg->port_reciever_current,
             sizeof(thread_brdcast_arg->port_reciever_current) );

        pthread_mutex_unlock(&shared_mutex);

        sleep(BROADCAST_TIMEOUT_RECIEVERS);
    }
};

void *listner_for_sender(void *arg){

    struct thread_broadcasting_argument *thread_brdcast_arg = (struct thread_broadcasting_argument*)arg;

    struct sockaddr_in serv_addr,cli_addr; // структура сокета сервера и клиента
    int	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
	   perror("ERROR opening socket");
	   return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY; // сервер принимает подключения на все IP-адреса
	serv_addr.sin_port = 0;

    int new_port_senders;

    bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

	if ( new_port_senders < 0 ){
        perror("ERROR on binding");
        return -1;
    }

    socklen_t len = sizeof(serv_addr);
    if (getsockname(sockfd, (struct sockaddr *)&serv_addr, &len) == -1) {
        perror("Getsockname");
        return -1;
    }

    printf("Port number %d\n", ntohs(serv_addr.sin_port));
    //thread_brdcast_arg->port_sender_current = (int) ntohs(serv_addr.sin_port);
    udp_broadcast_arg.port_sender_current = (int) ntohs(serv_addr.sin_port);

	listen(sockfd,5);
	int clilen;
	int newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		perror("ERROR on accept");
    printf("Client connected!\n");

    return 0;
};

void *listner_for_reciever(void *arg){
    return NULL;
}
