#include <stdio.h>

int call_void(void *arg) {
	return *(int*)arg;
}

int main() {
	int j = 5;
	printf("%d", call_void(&j));
	return 0;
}