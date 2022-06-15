// *** 소스코드 부연설명 *** //
// main 함수에서 별도의 쓰레드(thread_main)를 생성함
// main 함수가 종료가 되면, 프로세스가 종료되기 때문에, 
// main 함수에서 생성한 쓰레드(thread_main)도 같이 종료됨.

// sleep 함수를 이용하여 main 쓰레드(함수) 실행을 관리하여, 
// main 함수에서 생성한 쓰레드(thread_main)가 수행할 수 있도록 제어함.

// (문제점) -> thread_main 쓰레드(함수)의 실행 시간을 정확히 예측하여, main 쓰레드(함수)를 sleep 함수를 통해 제어해야 한다.
// 이는, 프로그램 흐름을 정확히 예측해야 한다는 의미이므로, 현실적으로 불가능하다.
// (해결) -> sleep 함수가 아닌 pthread_join 함수를 사용하여 제어한다. 

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
void* thread_main(void *arg);

int main(int argc, char *argv[])
{
	pthread_t t_id; // 쓰레드를 저장하기 위한 변수
	int thread_param=5;
	
	// 쓰레드 생성 -> thread_main 함수 호출 (thread_param의 주소 값을 전달)
	if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0)
	{
		puts("pthread_create() error");
		return -1;
	};
	
	sleep(10); // main 함수 실행을 10초간 중지 -> 프로세스의 종료 시기 늦춤
	puts("end of main");
	return 0; // 프로세스 종료 -> 프로세스 안에 생성된 쓰레드 종료
}


// thread_main 함수 종료 -> 쓰레드 종료
void* thread_main(void *arg)
{
	int i;
	int cnt=*((int*)arg); // 형변환
	
	for(i=0; i<cnt; i++)
	{
		sleep(1);
		puts("running thread");
	}

	return NULL;
}
