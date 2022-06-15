// *** 소스코드 부연설명 *** //
// 뮤텍스 기반의 동기화
// pthread_mutex_lock 함수 호출을 통해 thread_inc  쓰레드가 임계영역에 들어와 있다면,
// thread_des 쓰레드는 pthread_mutex_lock 함수 호출로 A 쓰레드가 진입한 임계영역이 들어갈 수 없다. (블록킹 상태)
// thread_inc 쓰레드가 임계영역에서 나갈 때까지, thread_des 쓰레드는 대기 상태가 된다.
// thread_inc 쓰레드가 pthread_mutex_unlock 함수 호출을 하여, 임계영역에서 빠져 나오면,
// thread_des 쓰레드는 임계영역에 들어갈 수 있다.

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD	100

void * thread_inc(void * arg);
void * thread_des(void * arg);

long long num=0; //각 쓰레드가 접근하는 전역변수
pthread_mutex_t mutex; // 뮤텍스 참조 값 전역 변수

int main(int argc, char *argv[]) 
{
	pthread_t thread_id[NUM_THREAD];
	int i;
	
	pthread_mutex_init(&mutex, NULL); // 뮤텍스 초기화 (생성)
	
	for(i=0; i<NUM_THREAD; i++)
	{
		// 2개의 쓰레드가 실행되는 형태
		if(i%2)
			pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
		else
			pthread_create(&(thread_id[i]), NULL, thread_des, NULL);	
	}	

	for(i=0; i<NUM_THREAD; i++)
		pthread_join(thread_id[i], NULL);

	printf("result: %lld \n", num);
	pthread_mutex_destroy(&mutex); // 뮤텍스 소멸
	return 0;
}

// num 값을 증가시키는 쓰레드
void * thread_inc(void * arg) 
{
	int i;
	pthread_mutex_lock(&mutex); // 임계영역 진입
	for(i=0; i<50000000; i++)
		num+=1; // 실제 임계 영역 (실질적으로 접근하고 있는 영역임)
	pthread_mutex_unlock(&mutex); // 임계영역 빠져나감
	return NULL;
}

// num 값을 감소시키는 쓰레드
void * thread_des(void * arg)
{
	int i;
	for(i=0; i<50000000; i++)
	{
		pthread_mutex_lock(&mutex);
		num-=1; // 실제 임계 영역
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}
