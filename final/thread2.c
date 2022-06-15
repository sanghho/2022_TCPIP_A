// *** 소스코드 부연설명 *** //
// pthread_join 함수를 통해, 함수를 호출한 쓰레드의 종료를 대기 시킬 수 있다.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
void* thread_main(void *arg);

int main(int argc, char *argv[])
{
	pthread_t t_id;
	int thread_param=5;
	void * thr_ret;

	// 쓰레드 생성 (thread_main), thread_param 주소값 전달
	if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0)
	{
		puts("pthread_create() error");
		return -1;
	};

	// pthread_join 함수 첫번째 인자 ID(t_id)의 쓰레드(thread_main)가 종료될 때까지,
	// 이 함수를 호출한 쓰레드(main)를 대기 상태에 둠.
	// -> t_id 쓰레드가 종료될 때까지, main 함수 대기
	if(pthread_join(t_id, &thr_ret)!=0)
	{
		puts("pthread_join() error");
		return -1;
	};	
	
	printf("Thread return message: %s \n", (char*)thr_ret);
	free(thr_ret);
	return 0;
}	

void* thread_main(void *arg)
{
	int i;
	int cnt=*((int*)arg);
	
	// 메모리 공간 동적 할당 -> 함수의 지역변수는, 함수가 종료가 되면 소멸된다.
    // 함수가 끝나서도 문자열이 메모리 공간에 남아 있게 하기 위함
	char * msg=(char *)malloc(sizeof(char)*50);

	strcpy(msg, "Hello, I'am thread~ \n");

	for(i=1; i<cnt; i++)
	{
		sleep(1);
		puts("running thread");
	}

	return (void*)msg; // 문자열의 주소값 반환
}
