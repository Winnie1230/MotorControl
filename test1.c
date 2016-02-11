#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(){
	long i = 10000000L;
	clock_t start, finish;
	double duration;
	printf("Time to do %ld empty loops is",i);
	start = clock();
	while(i--);
	finish = clock();
	duration = (double)(finish-start)/CLOCKS_PER_SEC;
	printf("%f seconds\n",duration);
	return 0;
}
