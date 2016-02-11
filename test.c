#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<sys/time.h>

int main(){
	int sum_c = 0;
	struct timeval start, finish;
    printf("%ld", CLOCKS_PER_SEC);
	while(1){
		if(sum_c == 0){
            gettimeofday(&start, NULL);
			// start = GetTickCount();
		}
		usleep(1000); //0.001s
		sum_c ++;
		if(sum_c >= 1){
            gettimeofday(&finish, NULL);
			// finish = GetTickCount();
			// printf("start = %ld,finish = %ld\n",start,finish);
            long int diff = finish.tv_usec - start.tv_usec;
			double time = 0.000001*diff;
			printf("time = %lf\n",time);
			sum_c = 0;
		}
	}
	return 0;
}

