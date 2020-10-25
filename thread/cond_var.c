#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

pthread_cond_t cv;
pthread_barrier_t barrier;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void* cv_thread(void* v)
{
	printf("in thread cv\n");
	pthread_cond_wait(&cv, &mutex);
	printf("done thread cv\n");
	return v;
}

void* barrier_thread(void* v)
{
	printf("in thread barrier\n");
	pthread_barrier_wait(&barrier);
	printf("done barrier thread\n");
	return v;
}


int main(int argc, char** argv)
{
	int r = pthread_cond_init(&cv, NULL);
	if (r != 0)
	{
		perror("pthread_cond_init fail");
		return 1;
	}
	int bres = pthread_barrier_init(&barrier, NULL, 3);
	if (bres != 0)
	{
		perror("pthread_barrier_init fail");
		return 1;
	}
	

	FILE* f = fopen("/tmp/main.pid","w");
	fprintf(f, "%d", getpid());
	fclose(f);

	pthread_mutex_lock(&mutex);

	pthread_t thread1;
	pthread_t thread2;
	
	pthread_create(&thread1, NULL, barrier_thread, NULL);
	pthread_create(&thread2, NULL, cv_thread, NULL);

	void* val;
	pthread_join(thread1, val);
	pthread_join(thread2, val);

	pthread_barrier_destroy(&barrier);
	pthread_cond_destroy(&cv);
	pthread_mutex_destroy(&mutex);
	return 0;
}
