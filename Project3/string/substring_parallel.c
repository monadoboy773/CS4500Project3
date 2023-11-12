#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX 1024
#define NUM_THREADS 4
int total = 0;
int n1,n2;
char *s1,*s2;
FILE *fp;

pthread_mutex_t lock;

int readf(FILE *fp)
{
	if((fp=fopen("strings.txt", "r"))==NULL){
		printf("ERROR: can't open string.txt!\n");
		return 0;
	}
	s1=(char *)malloc(sizeof(char)*MAX);
	if(s1==NULL){
		printf("ERROR: Out of memory!\n");
		return -1;
	}
	s2=(char *)malloc(sizeof(char)*MAX);
	if(s2==NULL){
		printf("ERROR: Out of memory\n");
		return -1;
	}
	/*read s1 s2 from the file*/
	s1=fgets(s1, MAX, fp);
	s2=fgets(s2, MAX, fp);
	n1=strlen(s1)-1;  /*length of s1*/
	n2=strlen(s2)-1; /*length of s2*/
	
	if(s1==NULL || s2==NULL || n1<n2)  /*when error exit*/
		return -1;
}

void *num_substring(void *num)
{
	int i,j,k;
	int count;
	int start = *((int *)num);
	int end = start + (n1/NUM_THREADS) - n2 + 1;

	for(i = start; i <= end; i++){   
		count = 0;
		for(j = i,k = 0; k < n2; j++,k++){  /*search for the next string of size of n2*/  
			if (*(s1+j)!=*(s2+k)){
				break;
			}
			else
				count++;
			if(count==n2){
				pthread_mutex_lock(&lock);
				total++;		/*find a substring in this step*/  
				printf("total substrings updated: %d\n", total);                        
				pthread_mutex_unlock(&lock);
			}
		}
	}
	pthread_exit(NULL);
	//return (void *) total;
}

int main(int argc, char *argv[])
{
	int count;
	readf(fp);
	
	pthread_t threads[NUM_THREADS]; // Creates an array of threads
	int thread_num[NUM_THREADS];

	pthread_mutex_init(&lock, NULL);

	int i;
	for(i = 0; i< NUM_THREADS; i++)
	{
		// Initializes a new thread
		// &threads[i]: Assigns id to each index in array
		// NULL: uses default thread attributes
		// num_substring: the function that the thread executes
		// (void *) &thread_num[i]: The input to the function
		thread_num[i] = i * (n1/NUM_THREADS); // splits substring into four parts for the threads
		pthread_create(&threads[i],NULL, num_substring, &thread_num[i]);
		printf("Thread %d created\n", i+1);
	}

	for(i = 0; i < NUM_THREADS; i++)
	{
		// join threads
		pthread_join(threads[i],NULL);
	}

	pthread_mutex_destroy(&lock);

	
 	printf("The number of substrings is: %d\n", total);
	return 1;
}