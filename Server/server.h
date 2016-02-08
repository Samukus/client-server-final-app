#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include "defs.h"
#include <pthread.h>
#include <errno.h>
#include <netinet/in.h>


size_t socket_init_broadcast(int *sock,int port);

size_t send_msg(int sock,struct sockaddr_in addr,void *msg,size_t len);

void *udp_broadcast_recievers(void *arg);

void *udp_broadcast_senders(void *arg);

void *listner_for_sender(void *arg);

void *listner_for_reciever(void *arg);

void *client_sender(void *arg);

void *client_reciever(void *arg);

pthread_mutex_t shared_mutex;

static struct thread_broadcasting_argument{
    int port_sender_current;
    int port_reciever_current;
    int number_of_messages;
    int brcst_senders_sock;
    int brcst_recievers_sock;
} udp_broadcast_arg;

#endif // SERVER_H_INCLUDED
