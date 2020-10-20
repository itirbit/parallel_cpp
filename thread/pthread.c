#include <pthread.h>

void* thread_loop(void* val)
{
	while(1);
	return val;
}


int main(int argc, char** argv)
{
	pthread_t thread;
	void* getVal;
	pthread_create(&thread, NULL, thread_loop, NULL);
	pthread_join(thread, &getVal);
	return 0;
}
