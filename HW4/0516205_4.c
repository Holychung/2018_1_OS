/*
Student No.: 0516205
Student Name: Holy Chung
Email: mickey94378@gmail.com
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not supposed to be posted to a public server, such as a public GitHub repository or a public web page. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <limits.h>

#define MAX INT_MAX

void ReadFile(void);
void Swap(int *a, int *b);
void PrintArray(int *array);
void OutputFile(char filename[], int *array);
void *ThreadPool();
void MergeSort(int n);
void *Partition(void *pid);
void BubbleSort(int idx);
void Merge(int idx);
void ResetArray(void);

int num, sec, usec;
int *array1, *array2;
sem_t sem[16];
sem_t anti_sem[16];
sem_t thread, dispatch, mutex;
struct timeval start, end;
int job_index, job_tail;
int num_proc;

struct parameter{
    int id;
    int head;
    int tail;
}params[16], job_list[16];

int main(){
    char filename[100]="output_n.txt";
    ReadFile();

    for(num_proc=1;num_proc<=8;num_proc++){ 
        ResetArray();
        // MergeSort with thread pool
        gettimeofday(&start, 0);
        MergeSort(num_proc);
        gettimeofday(&end, 0);
        sec = end.tv_sec - start.tv_sec;
        usec = end.tv_usec - start.tv_usec;
        printf("%d threads: %f sec\n", num_proc, sec+(float)(usec/1000000.0));
        filename[7]=num_proc+48;
        filename[12]='\0';
        OutputFile(filename, array1);
        // PrintArray(array1);
    }
    return 0;
}

void ReadFile(void){
    int i;
    char filename[100];
    printf("Prompt for the name of the input file: ");
    scanf("%s", filename);

    FILE *fin;
    if ((fin=fopen(filename, "r"))==NULL){
        printf("Error open file\n");
        exit(1);
    }
    
    fscanf(fin, "%d", &num);
    array1=(int*)malloc(sizeof(int)*num);
    array2=(int*)malloc(sizeof(int)*num);
    for(i=0; i<num; i++){
        fscanf(fin, "%d", &array1[i]);
        array2[i]=array1[i];
    }
    return;
}

void MergeSort(int n){
    pthread_t t[16];
    params[1].head=0;
    params[1].tail=num-1;
    job_index=1;
    job_tail=1;
    sem_init(&mutex,0,1);
    sem_init(&dispatch,0,0);
    sem_init(&thread,0,1);
    int i;

    // create a thread pool with n threads
    for(i=0;i<n;i++){
        pthread_create(&t[i], NULL, ThreadPool, NULL);
    }

    // init semaphore
    for(i=1;i<16;i++){
        params[i].id=i;
        sem_init(&sem[i], 0, 0);
        sem_init(&anti_sem[i], 0, 0);
    }

    // start Partition
    for(i=1;i<8;i++){
        pthread_create(&t[i], NULL, Partition, &params[i].id);
    }
    
    sem_post(&sem[1]);
    sem_wait(&anti_sem[1]);

    return;
}

void *ThreadPool(){
	// A job is bubble sort or merge two subarrays
    while(1){
        sem_wait(&thread);
        sem_wait(&dispatch);
        sem_wait(&mutex);
        int idx=job_index;
        job_index++;
        sem_post(&mutex);
        if(job_list[idx].id<8)
            Merge(idx);
        else
            BubbleSort(idx);
        sem_post(&thread);
        if(job_index>15)
			break;
    }
}

void *Partition(void *pid){
    int id=*(int*)pid;
    // wait upper level arrary finish dividing
    sem_wait(&sem[id]);
    
    // critical section, divide into two subarray
    int head=params[id].head, tail=params[id].tail, mid=(tail+head)/2;
    params[2*id].head=head;
    params[2*id].tail=mid;
    params[2*id+1].head=mid+1;
    params[2*id+1].tail=tail;
    
    if(id>3&&id<8){
    	// dispatch a job two do bubble sort
    	sem_wait(&mutex);
        job_list[job_tail].id=2*id;
        job_list[job_tail].head=head;
        job_list[job_tail].tail=mid;
        job_tail++;
        sem_post(&dispatch);
        job_list[job_tail].id=2*id+1;
        job_list[job_tail].head=mid+1;
        job_list[job_tail].tail=tail;
        job_tail++;
        sem_post(&dispatch);
	    sem_post(&mutex);
    }
    else{
    	// signal two threads 2*id, 2*id+1 to sort two subarrays
        sem_post(&sem[2*id]);
        sem_post(&sem[2*id+1]);
    }

    // wait on two subarrays finish sorting
    sem_wait(&anti_sem[2*id]);
    sem_wait(&anti_sem[2*id+1]);

    // dispatch a job to merge two subarrays
    sem_wait(&mutex);
    job_list[job_tail].id=id;
    job_list[job_tail].head=head;
    job_list[job_tail].tail=tail;
    job_tail++;
    sem_post(&dispatch);
    sem_post(&mutex);
}

void BubbleSort(int idx){
    int i, j;
    int id=job_list[idx].id;
    int size=job_list[idx].tail-job_list[idx].head+1;
    for(i=size-1; i>0; i--)
        for(j=job_list[idx].head; j<job_list[idx].head+i; j++)
            if(array1[j]>array1[j+1])
                Swap(&array1[j], &array1[j+1]);
    // signal upper level array
    sem_post(&anti_sem[id]);
}

void Merge(int idx){
    int head=job_list[idx].head;
    int tail=job_list[idx].tail;
    int mid=(tail+head)/2;

    int i, j, size=tail-head+1;
    int lenLeft=mid-head+1, lenRight=tail-mid;
    int LeftSub[lenLeft+1], RightSub[lenRight+1];
    int idxRight=0, idxLeft=0;

    for(i=0; i<lenLeft; i++)
        LeftSub[i]=array1[i+head];
    for(i=0; i<lenRight; i++)
        RightSub[i]=array1[i+mid+1];
    LeftSub[lenLeft]=MAX;
    RightSub[lenRight]=MAX;

    for(i=head; i<tail+1; i++, j++)
        if(LeftSub[idxLeft]<RightSub[idxRight])
            array1[i]=LeftSub[idxLeft++];
        else
            array1[i]=RightSub[idxRight++];

    // signal upper level array
    sem_post(&anti_sem[job_list[idx].id]);

    return;
}

void Swap(int *a, int *b){
    int temp=*a;
    *a=*b;
    *b=temp;
    return;
}

void ResetArray(void){
    int i;
    for(i=0;i<num;i++)
        *(array1+i)=*(array2+i);
}

void PrintArray(int *array){
    int i;
    for(i=0;i<num; i++)
        printf("%d ", array[i]);
    printf("\n");
    return;
}

void OutputFile(char filename[], int *array){
    FILE *fout;
    int i;
    fout=fopen(filename, "w");
    for(i=0;i<num;i++){
        fprintf(fout,"%d ", array[i]);
    }
    fclose(fout);
}