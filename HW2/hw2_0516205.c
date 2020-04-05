/*
Student No.: 0516205
Student Name: 鍾禾翊
Email: mickey94378@gmail.com
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not supposed to be posted to a public server, such as a public GitHub repository or a public web page. 
*/

#include<stdio.h>
#include<stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

void initial_matrix(unsigned int *A, unsigned int *C, int dimension){
	for(int i = 0; i < dimension; i++){
		for(int j = 0; j < dimension; j++){
			A[i*dimension+j] = i*dimension + j;
		}
	}
	return;
}

void multiplication(unsigned int *A, unsigned int *C, int head, int tail, int dimension){
	unsigned int sum = 0;
	for(int i = head; i <= tail; i++){
		for(int j = 0; j < dimension; j++){
			sum = 0;
			for(int k = 0; k < dimension; k++){
				sum += A[i*dimension + k] * A[k*dimension + j];
			}
			C[i*dimension+j] = sum;
		}
	}
	return;
}

unsigned int checksum(unsigned int *C, int dimension){
	unsigned int sum = 0;
	for(int j = 0; j < dimension*dimension; j++)
		sum += C[j];
	return sum;
}

int main(){
    int dimension;
    printf("Input the matrix dimension: ");
	scanf("%d", &dimension);
	int matrixA =  shmget(0, 2600000, IPC_CREAT|0660);
	int matrixC =  shmget(0, 2600000, IPC_CREAT|0660);
	struct timeval start, end;
	
	unsigned int *Aid = (unsigned int *)shmat(matrixA, NULL, 0);
	unsigned int *Cid = (unsigned int *)shmat(matrixC, NULL, 0);
	initial_matrix(Aid, Cid, dimension);

	pid_t pid, wpid;
	// do 1~16 process to compare time elapsed
	for(int i = 1; i <= 16; i++){
		// reset matrix C
		for(int j = 0; j < dimension*dimension; j++)
			Cid[j] = 0;

		int task = dimension / i;	// task is the number of row which every process need to calculate
		gettimeofday(&start, 0);
		// fork every process
		for(int pi = 0; pi < i; pi++){
			pid = fork();
			if(pid == 0){
				int head = pi*task;
				int tail = head + task - 1;
				if(pi == i-1)
					tail = dimension - 1;
				multiplication(Aid, Cid, head, tail, dimension);
				exit(0);
			}
			else if(pid < 0){
				// Error
				printf("Error forking!\n");
			}
			else{
				// Parent process
			}
		}
		int status = 0;
		while ((wpid = wait(&status)) > 0);	// wait for all process
		gettimeofday(&end,0);
		int sec = end.tv_sec - start.tv_sec;
		int usec = end.tv_usec - start.tv_usec;
		printf("Multiplying matrices using %d process\n", i);
		printf("Elapsed time: %f sec, Checksum: %u\n", sec + (usec/1000000.0), checksum(Cid, dimension));
	}

	shmctl(matrixA, IPC_RMID , NULL);
	shmctl(matrixC, IPC_RMID , NULL);
	shmdt(Aid);
	shmdt(Cid);

	return 0;
}