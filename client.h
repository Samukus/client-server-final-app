#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <defs.h>

struct message{
    int len;
    int sleep_time;
    char message_text[MSG_BUF_LEN-1];
} buf_message;

int current_port;

int socket_init_udp(int *sock,int port);

int create_message(char *message);

int send_msg(struct sockaddr_in server_addr,int current_port,char *message);

int get_server_add(struct sockaddr_in *server_addr,int port);

int parse_message(char *message);


