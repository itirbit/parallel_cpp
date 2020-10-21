#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_spinlock_t spinLock;
pthread_rwlock_t rdLock = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t wrLock = PTHREAD_RWLOCK_INITIALIZER;

void* mutex_thread(void* val)
{
	pthread_mutex_lock(&mutex);
	return val;
}

void* spin_thread(void* val)
{
	pthread_spin_lock(&spinLock);
	return val;
}

void* rd_thread(void* val)
{
	pthread_rwlock_rdlock(&rdLock);
	return val;
}

void* wr_thread(void* val)
{
	pthread_rwlock_wrlock(&wrLock);
	return val;
}

int main(int argc, char** argv)
{	
	pthread_spin_init(&spinLock, PTHREAD_PROCESS_SHARED);
	
	pthread_mutex_lock(&mutex);
	pthread_spin_lock(&spinLock);
	pthread_rwlock_rdlock(&rdLock);
	pthread_rwlock_wrlock(&wrLock);

	char data[1024];
	sprintf(data, "%d", getpid());

	FILE* f = fopen("/tmp/main.pid", "w");
	fputs(data, f);
	fclose(f);

	pthread_t threads[4];

	pthread_create(&threads[0], NULL, mutex_thread, NULL);
	pthread_create(&threads[1], NULL, spin_thread, NULL);
	pthread_create(&threads[2], NULL, rd_thread, NULL);
	pthread_create(&threads[3], NULL, wr_thread, NULL);

	void* val;

	pthread_join(threads[0], val);
	pthread_join(threads[1], val);
	pthread_join(threads[2], val);
	pthread_join(threads[3], val);

	return 0;
}

