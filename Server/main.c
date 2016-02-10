#include "server.h"

int main(){

    printf("This is Server\n");
    /*      Initialisation of IPC_QUEUE for messages                           */
    /*      queue id in server.h int msg_queue                                 */

    queue_init();

    /*      Initialisation of UDP sockets for broadcasting                     */
    /*      ports: PORT_SENDERS & PORT_RECIEVERS defines in defs.h             */

    socket_init_broadcast(&udp_broadcast_arg.brcst_senders_sock,PORT_SENDERS);
    socket_init_broadcast(&udp_broadcast_arg.brcst_recievers_sock,PORT_RECIEVERS);

    /* View HELP  */
    print_help();

    /*      Creating threads:                                               */
    /*     thread_lstn_snd: listner for connection of client sender  */
    pthread_t thread_lstn_snd;
    int code_sender = 0;
    pthread_create(&thread_lstn_snd, NULL, listner, &code_sender);

    /*     thread_lstn_rcv: listner for connection of client reciever  */
    pthread_t thread_lstn_rcv;
    int code_reciever = 1;
    pthread_create(&thread_lstn_rcv, NULL, listner, &code_reciever);

    /*     thread_brcst_snd: broadcast current port for the clients senders*/
    pthread_t thread_brcst_snd;
    pthread_create(&thread_brcst_snd, NULL, udp_broadcast_senders, &udp_broadcast_arg);

    /*     thread_brcst_rcv: broadcast current port for the clients recievers*/
    pthread_t thread_brcst_rcv;
    pthread_create(&thread_brcst_rcv, NULL, udp_broadcast_recievers, &udp_broadcast_arg);


    /* View ports for recievers & senders in console                */
    sleep(1);
    print_port();

    /*      thread_console: thread for the control of the server (use 'c' ENTER)*/
    /*      comands 'c': q - shut down the server,
                         p - print ports for the senders and recievers,
                         l - print number of messages in queue,
                         c - clear console;                                     */
    console();

    return 0;
}
