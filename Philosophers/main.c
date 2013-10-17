#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

#define DEADLOCK
#define P_COUNT 5

pthread_t philosophers[P_COUNT];
pthread_mutex_t forks[P_COUNT];
pthread_mutex_t print_mutex;

pthread_mutex_t *left_fork(int idx) {
	return &forks[idx];
}

pthread_mutex_t *right_fork(int idx) {
	return &forks[idx + 1 != P_COUNT ? idx + 1 : 0];
}

void dinner_message(int idx, char *note) {
	pthread_mutex_lock(&print_mutex);
	printf("%d %s \n", idx, note);
	fflush(stdout);
	pthread_mutex_unlock(&print_mutex);
}

void *philosopher(void *args) {
	int idx = *(int*)args;

	while(1) {
#ifdef DEADLOCK
		pthread_mutex_lock(left_fork(idx));	
		dinner_message(idx, "took left fork");
		pthread_mutex_lock(right_fork(idx));

		dinner_message(idx, "eats dinner");
		sleep(rand() % 5);

		pthread_mutex_unlock(right_fork(idx));
		pthread_mutex_unlock(left_fork(idx));
#else
		pthread_mutex_lock(left_fork(idx));	
		// dinner_message(idx, "took left fork");
		if(pthread_mutex_trylock(right_fork(idx)) != 0) {
			pthread_mutex_unlock(left_fork(idx));
			// dinner_message(idx, "returned left fork");
			// sleep(rand() % 5);
			continue;
		}

		dinner_message(idx, "eats dinner");
		sleep(rand() % 5);

		pthread_mutex_unlock(right_fork(idx));
		pthread_mutex_unlock(left_fork(idx));
#endif
	}

	return NULL;
}

int main() {
	pthread_mutex_init(&print_mutex, NULL);

	for(int i = 0; i < P_COUNT; ++i) {
		pthread_mutex_init(&forks[i], NULL);
	}

	for(int i = 0; i < P_COUNT; ++i) {
		int *idx = (int*)malloc(sizeof(int));
		*idx = i;
		pthread_create(&philosophers[i], NULL, philosopher, idx);		
	}

	for(int i = 0; i < P_COUNT; ++i) {
		pthread_join(philosophers[i], NULL);
	}
	return 0;
}
