#include "client.h"

int main(){
    struct timeval time;
    gettimeofday(&time, NULL);
    time.tv_usec;
    srand(time.tv_usec);

    char message[MSG_BUF_LEN]="";

    struct sockaddr_in server_addr;

    while(1){

        current_port = get_server_add(&server_addr,PORT_RECIEVERS);

        if(current_port == -1)
            continue;

        rcv_msg(server_addr,current_port,message);

        //parse_message(message);

        sleep(buf_message.sleep_time);
    }

    return(0);
}
