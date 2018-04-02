/*
gcc -pthread -o server server.c
./server
nc 127.0.0.1 1232
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
// #define portnum 1233

int client_num = 0;
int client_id[5];
int portnum = 0;
pthread_mutex_t mutx;

void * client_connection(void *arg){
	int client_fd = (int)arg;
	int str_len = 0;
	char message[BUFFSIZE];
	int i;

	printf("New thread id is %lu\n",pthread_self());
	while((str_len = read(client_fd, message, sizeof(message))) != 0){
		printf("Client %d : %s\n", client_fd, message);
		send_message(message, client_fd);
	}
	pthread_mutex_lock(&mutx);
	for(i=0; i<client_num; i++){									//클라이언트 접속종료시 현재 등록된 클라이언트수 정리
		if(client_fd == client_id[i]){
			for( ; i<client_num-1; i++)
				client_id[i] = client_id[i+1];
			break;
		}
	}
	client_num--;
	pthread_mutex_unlock(&mutx);
	printf("Dead thread id is %lu\n",pthread_self());
	printf("Disconnected to Client : %d\n", client_fd);
	close(client_fd);
}

void send_message(char * message, int client_fd){
	int i;
	char send_message[100] = "";
	pthread_mutex_lock(&mutx);
	sprintf(send_message, "Client %d : %s\n", client_fd, message);
	for(i=0; i<client_num; i++)
		write(client_id[i], send_message, sizeof(send_message));
	pthread_mutex_unlock(&mutx);
}

int main()
{
    char buffer[BUFFSIZE];
    struct sockaddr_in server_addr, client_addr;
    int server_fd, client_fd;							//server_fd, client_fd : 각 소켓 번호
	int client_addr_size = 0;
	int issuccess = 0;
	pthread_t thread;

	printf("Portnum : ");
	scanf("%d",&portnum);

	memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;					//IPv4 설정
    server_addr.sin_port = htons(portnum);				//포트설정
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//자신의 ip주소로 설정. 어차피 localhost 127.0.0.1 사용해서 상관없음

	if(pthread_mutex_init(&mutx, NULL))
		error("Mutex Init Error!");

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){								// 소켓 생성
        printf("socket() error!\n");
        exit(1);
    }
 
	issuccess = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(issuccess == -1) {								//bind() 호출
        printf("bind() error!\n");
        exit(1);
    }
 
	issuccess = listen(server_fd, 5);
    if(issuccess == -1){						//소켓을 수동 대기모드로 설정
        printf("Wait state fail!\n");
        exit(1);
    }
 
    memset(buffer, 0x00, sizeof(buffer));
    printf("Open!! server\n");
    client_addr_size = sizeof(client_addr);

	while(1){
		client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_size);
		pthread_mutex_lock(&mutx);
		client_id[client_num++] = client_fd;
		pthread_mutex_unlock(&mutx);
		pthread_create(&thread, NULL, client_connection, (void*)client_fd);
		printf("Connected to Client : %d\n", client_fd);
		pthread_detach(thread);
	}
    close(server_fd);
    return 0;
}
