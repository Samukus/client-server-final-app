#include "client.h"

int main(){
    struct timeval time;
    gettimeofday(&time, NULL);
    time.tv_usec;
    srand(time.tv_usec);

    char message[MSG_BUF_LEN]="";

    struct sockaddr_in server_addr;

    while(1){

        current_port = get_server_add(&server_addr,PORT_SENDERS);

        if(current_port == -1)
            continue;

        create_message(message);

        send_msg(server_addr,current_port,message);

        sleep(buf_message.sleep_time);
    }

    return(0);
}
