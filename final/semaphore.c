// 뮤텍스 vs 세마포어
// 뮤텍스 : 열쇠의 개념, 임계영역에 동시에 둘 이상의 쓰레드가 접근이 불가능 함.
// -> ex) 하나의 열쇠에서 그 열쇠를 가지고 있는 쓰레드만이 임계영역에 접근할 수 있다.
// 세마포어 : 카운트의 개념, 둘 이상의 쓰레드가 동시에 접근할 수 있다.
// -> ex) 세마포어의 카운트가 3이라면, 임계영역에 동시에 접근할 수 있는 쓰레드의 수는 3개이다.

// 세마포어 기본 동작
// sem_init으로 임계영역에 동시에 접근할 수 있는 쓰레드의 수를 정의 한다. (세마포어 값)
// sem_wait 함수 호출을 통해 세마포어 값을 하나 감소 시키는데, 만약 0인 상태에서 호출이 되면
// 해당 쓰레드는 블록킹이 되어 대기 상태에 빠진다.
// -> 해당 쓰레드는 sem_post 함수가 호출이 되어 세마포어 값이 하나 증가될 때, 임계영역에 진입할 수 있다.

// 세마포어 초기화 (생성)
// sem_init의 첫번째 인수 : 세마포어의 참조 값을 저장하고 있는 변수의 주소
// sem_init의 두번째 인수 : 0 -> 하나의 프로세스 내에서만 접근이 가능한 세마포어
// sem_init의 세번째 인수 : 세마포어의 초기 값 (카운트 값, 임계영역에 동시에 접근할 수 있는 쓰레드의 수)

// *** 코드 동작  *** // 
// 세마포어 2개 - sem_one, sem_two
// 쓰레드 2개 - id_t1(read), id_t2(accu)
// 1.(i=0) read 함수에서 sem_wait(sem_two) 실행 -> sem_two (세마포어 값: 1->0) -> sem_two 임계영역 진입
// 1.(i=0) accu 함수에서 sem_wait(sem_one) 실행 -> sem_one (세마포어 값은 0 -> 대기 상태)

// 2.(i=0) read 함수에서 num 값 입력 처리 -> sem_post(sem_one) 실행 -> sem_one (세마포어 값: 0->1) -> (i=1) sem_wait(sem_two) 실행 -> sem_two (세마포어 값은 0 -> 대기 상태) 
// 2.(i=0) accu 함수에서 대기중인 sem_one 의 임계영역 진입 (세마포어 값: 1->0) -> sum+=num 처리 -> sem_post(sem_two) -> sem_two (세마포어 값: 0->1) -> (i=1) sem_wait(sem_one) 실행 -> sem_one (세마포어 값은 0 -> 대기상태) 

// 3. (i=1) read 함수에서 대기중인 sem_two의 임계영역 진입 (세마포어 값: 1->0) -> num 값 입력 처리 -> sem_post(sem_one) 실행 -> sem_one (세마포어 값은 0->1)...

// 세마포어 2개를 가지고 각각의 세마포어 값을 0과 1로 더하고 빼어
// read 1번 수행하고 대기하고, accu 1번 수행하고 대기하고.. 번갈아 반복 수행한다.

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void * read(void * arg);
void * accu(void * arg);
static sem_t sem_one;
static sem_t sem_two;
static int num;

int main(int argc, char *argv[])
{
	pthread_t id_t1, id_t2;

	// 세마포어 2개  초기화 (생성)
	// sem_init의 첫번째 인수 : 세마포어의 참조 값을 저장하고 있는 변수의 주소
	// sem_init의 두번째 인수 : 0 -> 하나의 프로세스 내에서만 접근이 가능한 세마포어
	// sem_init의 세번째 인수 : 세마포어의 초기 값 0, 1
	sem_init(&sem_one, 0, 0);
	sem_init(&sem_two, 0, 1);

	pthread_create(&id_t1, NULL, read, NULL);
	pthread_create(&id_t2, NULL, accu, NULL);

	pthread_join(id_t1, NULL);
	pthread_join(id_t2, NULL);

	// 세마포어 소멸
	// sem_destroy의 인수 : 소멸하고자 하는 세마포어의 참조 값을 저장하고 있는 변수의 주소
	sem_destroy(&sem_one);
	sem_destroy(&sem_two);
	
	return 0;
}

void * read(void * arg)
{
	int i;
	for(i=0; i<5; i++)
	{
		fputs("Input num: ", stdout);
		
		// sem_wait : 세마포어 값 하나 감소
		// 세마포어 값이 0일 때 호출 ->  호출한 쓰레드의 함수가 반환되지 않아, 블로킹 상태가 됨.
		// -> 이때, sem_post를 호출하면, 블로킹 상태에서 빠져나옴.
		sem_wait(&sem_two);

		scanf("%d", &num);

		// sem_post : 세마포어 값 하나 증가
		sem_post(&sem_one);
	}
	return NULL;
}

void * accu(void * arg)
{
	int sum=0, i;
	for(i=0; i<5; i++)
	{
		sem_wait(&sem_one);
		sum+=num;
		sem_post(&sem_two);
	}
	printf("Result: %d \n", sum);
	return NULL;
}
