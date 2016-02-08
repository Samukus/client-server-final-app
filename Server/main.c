#include "server.h"



int main()
{
    //struct thread_broadcasting_argument udp_broadcast_arg;
    //udp_broadcast_arg.number_of_messages = 10;
    //udp_broadcast_arg.port_reciever_current = 27000;
    //udp_broadcast_arg.port_sender_current = 28000;

    pthread_t thread_brcst_snd;
    pthread_t thread_brcst_rcv;
    pthread_t thread_lstn_snd;
    pthread_t thread_lstn_rcv;

    socket_init_broadcast(&udp_broadcast_arg.brcst_senders_sock,PORT_SENDERS);
    socket_init_broadcast(&udp_broadcast_arg.brcst_recievers_sock,PORT_RECIEVERS);

    pthread_create(&thread_lstn_snd, NULL, listner_for_sender, &udp_broadcast_arg);
    pthread_create(&thread_lstn_rcv, NULL, listner_for_reciever, &udp_broadcast_arg);

    pthread_create(&thread_brcst_snd, NULL, udp_broadcast_senders, &udp_broadcast_arg);
    pthread_create(&thread_brcst_rcv, NULL, udp_broadcast_recievers, &udp_broadcast_arg);




    sleep(10);
    return 0;
}
