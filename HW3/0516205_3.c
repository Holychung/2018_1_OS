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
void SingleSort(int head, int tail, int times);
void SingleBubbleSort(int head, int tail);
void SingleMerge(int head, int mid, int tail);
void ParallelMergeSort(void);
void ParallelMerge(int head, int mid, int tail);
void *ParallelPartition(void *pid);
void *ParallelBubbleSort(void *pid);
void Swap(int *a, int *b);
void PrintArray(int *array);
void OutputFile(char filename[], int *array);

int num, sec, usec;
int *array1, *array2;
sem_t sem[16], final;
sem_t anti_sem[16];
struct timeval start, end;

struct parameter{
    int id;
    int head;
    int tail;
}params[16];

int main(){
    ReadFile();

    // Single thread merge sort
    gettimeofday(&start, 0);
    SingleSort(0, num-1, 0);
    gettimeofday(&end, 0);
    sec = end.tv_sec - start.tv_sec;
    usec = end.tv_usec - start.tv_usec;
    printf("Single thread: %f sec\n", sec+(float)(usec/1000000.0));
    OutputFile("output2.txt", array1);

    // Multi thread merge sort
    gettimeofday(&start, 0);
    ParallelMergeSort();
    gettimeofday(&end, 0);
    sec = end.tv_sec - start.tv_sec;
    usec = end.tv_usec - start.tv_usec;
    printf("Multiple threads: %f sec\n", sec+(float)(usec/1000000.0));
    OutputFile("output1.txt", array2);

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

void SingleSort(int head, int tail, int times){
    if(times<3){
        int mid=(head+tail)/2;
        SingleSort(head, mid, times+1);
        SingleSort(mid+1, tail, times+1);
        SingleMerge(head, mid ,tail);
    }
    else
        SingleBubbleSort(head, tail);
}

void SingleMerge(int head, int mid, int tail){
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
    return;
}

void SingleBubbleSort(int head, int tail){
    int i, j;
    int size=tail-head+1;
    for(i=size-1; i>0; i--)
        for(j=head; j<head+i; j++)
            if(array1[j]>array1[j+1])
                Swap(&array1[j], &array1[j+1]);
    return;
}

void ParallelMergeSort(void){
    pthread_t t[16];
    params[1].head=0;
    params[1].tail=num-1;
    sem_init(&final, 0, 0);
    int i;
    for(i=1;i<16;i++){
        params[i].id=i;
        sem_init(&sem[i], 0, 0);
        sem_init(&anti_sem[i], 0, 0);
        if(i<8)
            pthread_create(&t[i], NULL, *ParallelPartition, &params[i].id);
        else
            pthread_create(&t[i], NULL, *ParallelBubbleSort, &params[i].id);
    }
    sem_post(&sem[1]);
    sem_wait(&anti_sem[1]);

    return;
}

void *ParallelPartition(void *pid){
    int id=*(int*)pid;

    // wait upper level arrary finish dividing
    sem_wait(&sem[id]);
    
    // critical section, divide into two subarray
    int head=params[id].head, tail=params[id].tail, mid=(tail+head)/2;
    params[2*id].head=head;
    params[2*id].tail=mid;
    params[2*id+1].head=mid+1;
    params[2*id+1].tail=tail;
    
    // signal two threads 2*id, 2*id+1 to sort two subarrays
    sem_post(&sem[2*id]);
    sem_post(&sem[2*id+1]);
    
    // wait on two subarrays finish sorting
    sem_wait(&anti_sem[2*id]);
    sem_wait(&anti_sem[2*id+1]);
    
    // merge two subarrays
    ParallelMerge(head, mid, tail);

    // signal upper level thread
    sem_post(&anti_sem[id]);
}

void ParallelMerge(int head, int mid, int tail){
    int i, j, size=tail-head+1;
    int lenLeft=mid-head+1, lenRight=tail-mid;
    int LeftSub[lenLeft+1], RightSub[lenRight+1];
    int idxRight=0, idxLeft=0;

    for(i=0; i<lenLeft; i++)
        LeftSub[i]=array2[i+head];
    for(i=0; i<lenRight; i++)
        RightSub[i]=array2[i+mid+1];
    LeftSub[lenLeft]=MAX;
    RightSub[lenRight]=MAX;

    for(i=head; i<tail+1; i++, j++)
        if(LeftSub[idxLeft]<RightSub[idxRight])
            array2[i]=LeftSub[idxLeft++];
        else
            array2[i]=RightSub[idxRight++];
    return;
}

void *ParallelBubbleSort(void *pid){
    int id=*(int*)pid;
    sem_wait(&sem[id]);
    int i, j;
    int size=params[id].tail-params[id].head+1;
    for(i=size-1; i>0; i--)
        for(j=params[id].head; j<params[id].head+i; j++)
            if(array2[j]>array2[j+1])
                Swap(&array2[j], &array2[j+1]);
    // signal upper level array
    sem_post(&anti_sem[id]);
}

void Swap(int *a, int *b){
    int temp=*a;
    *a=*b;
    *b=temp;
    return;
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
    for(i=0;i<num;i++)
        fprintf(fout,"%d ", array[i]);
    fclose(fout);
}