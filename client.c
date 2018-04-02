/*
gcc -pthread -o client client.c
./client
nc 127.0.0.1 1234
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include <pthread.h>
#define BUFFSIZE 100
//#define portnum 1233

int portnum = 0;

void * writing(void *arg){
    int server_fd = (int)arg;
    char message[BUFFSIZE];
    while(1){
        scanf("%s", message);
        write(server_fd, message, strlen(message));
    }
}

int main()
{
    char buffer[BUFFSIZE];
    struct sockaddr_in server_addr;
    int server_fd;					                	//server_fd : 각 소켓 번호
	int client_addr_size = 0;
	int issuccess = 0;
    int str_len = 0;
	pthread_t thread;

    printf("Portnum : ");
	scanf("%d",&portnum);

	server_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){								// 소켓 생성
        printf("socket() error!\n");
        exit(1);
    }

    memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;					//IPv4 설정
    server_addr.sin_port = htons(portnum);				//포트설정
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//자신의 ip주소로 설정. 어차피 localhost 127.0.0.1 사용해서 상관없음

    connect(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
    printf("Open!! client\n");

    pthread_create(&thread, NULL, writing, (void*)server_fd);
    while(1) {
        str_len = read(server_fd, buffer, BUFFSIZE);
        if(str_len != -1){
        printf("%s", buffer);
        }
    }
    close(server_fd);
    return 0;
}


