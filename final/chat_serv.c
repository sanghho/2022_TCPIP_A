// *** 소스코드 부연 설명 *** //
// 동작 구조
// 1. 클라이언트 연결 요청 -> 쓰레드 생성 -> 쓰레드가 클라이언트에게 서비스를 제공
// ex) 클라이언트 10명 -> 쓰레드 10개 생성

// 클라이언트 정보를 서버가 등록함. -> 전역변수 형태로 관리 (모든 쓰레드가 접근을 하여 그 정보를 얻기 위함)
// -> 클라이언트가 10명이면 10명이 서로 상호 대화를 할 수 있는 서버 모델임. (한 사람이 말하면 나머지 9명에게 그 메시지를 전달해야 함)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);

// 서버에 접속한 클라이언트의 소켓 관리를 위한 변수, 배열
// 이 전역변수, 전역 배열에 접근하는 코드(값을 변경) 는 하나의 임계영역을 구성함
int clnt_cnt=0;
int clnt_socks[MAX_CLNT];

pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;

	if(argc!=2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	pthread_mutex_init(&mutx, NULL);
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);

		// accept 함수 호출 -> 현재 연결된 클라이언트에게 파일 디스크립트 정보를 배열에 저장
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

		// 뮤텍스의 동기화 처리  -> 배열이 전역변수로 선언이 되어 있어서 동시 접근이 가능한 영역이기 때문에
		// 클라이언트 정보를 등록하는 문장을 실행할 때는, 다른 쓰레드가 접근하지 못하도록 함
		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++]=clnt_sock; // 새로운 연결이 형성될 때마다 -> 변수와 배열에 해당 정보 등록
		pthread_mutex_unlock(&mutx);
		
		// 추가된 클라이언트에게 쓰레드를 생성함 -> handle_clnt 함수 실행
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock); 
		
		pthread_detach(t_id); //종료된 쓰레드가 메모리에서 완전히 소멸
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	
	close(serv_sock);
	return 0;
}

// 데이터를 읽고, 전송하는 기능 수행 (read, write)
void * handle_clnt(void * arg)
{
	int clnt_sock=*((int*)arg);
	int str_len=0, i;
	char msg[BUF_SIZE];

	while((str_len= read(clnt_sock, msg, sizeof(msg)))!=0)

		// send 대상 : 모든 쓰레드가 접근 가능한 clnt_socks[i] -> 현재 연결되어 있는 클라이언트 정보 (모든 클라이언트에게 메시지를 전달)
		send_msg(msg, str_len);
	
	pthread_mutex_lock(&mutx);

	// 클라이언트 삭제
	for(i=0; i<clnt_cnt; i++)
	{
		if(clnt_sock==clnt_socks[i])
		{
			while(i++ < clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}

	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}

// 클라이언트에게 메시지를 전송하는 기능 수행
void send_msg(char * msg, int len)
{
	int i;
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx);
}

void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputs('\n', stderr);
	exit(1);
}

