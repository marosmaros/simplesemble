#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#define MIN(X,Y) ((X) < (Y) ?  (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ?  (Y) : (X))
double findmp(const char* a, const char* b, char** readsF, char** readsR,int num_reads, int seed_len) {
	//want to find reads from readsR that are mapping into end of b
	printf("GOT B |%s|%lu\n",b,strlen(b));
	//from these want to count those that have a mp in a
	int b_len=strlen(b);
	int a_len=strlen(a);
	//char* seed=(char*)malloc(seed_len+1);
	double * hit_count;
	int num_threads;
	#pragma omp parallel
	{
	#pragma omp single
	{
	num_threads=omp_get_num_threads();
	//printf("Starting up\n");
	hit_count=(double*)malloc(sizeof(double)*num_threads);
	if (hit_count==NULL) {
		fprintf(stderr,"ERROR !!! a3423\n");
		exit(1);
	}
	int i;
	for (i=0; i<num_threads; i++) {
		hit_count[i]=0;
	}
	//printf("DONE!\n");
	}
	int thread_num=omp_get_thread_num();
	//printf("Thread %d/%d reporting\n",thread_num,num_threads);
	int j;
	char seed[seed_len+1];
	#pragma omp for
	for (j=0; j<=(b_len-seed_len); j++) {
		strncpy(seed,b+j,seed_len);
		seed[seed_len]='\0';
		//if (j==b_len-seed_len) {
		//	printf("The last seed is %s\n",seed);
		//}
		int local_hits=0;
		int i;
		for (i=0; i<num_reads; i++ ) {
			char* s = strstr(readsR[i],seed);
			if (s!=NULL) {
				//found a match, now grab mate pair and check a
				char buff[seed_len+1];
				int k;
				for (k=0; k<=(strlen(readsF[i])-seed_len); k+=10) {
					strncpy(buff,readsF[i]+k,seed_len);
					buff[seed_len]='\0';
					char * t=strstr(a,buff);
					if (t!=NULL) {
						int distance_between_pair=(a_len-(t-a)-1 + seed_len) + j;
						double score=1.0-MIN(abs(distance_between_pair-500)/500.0,1);	
						hit_count[thread_num]+=score;
						local_hits++;
					}	
				}
			}	
		}
		//printf("%d:%s had %d hits\n",thread_num,seed,local_hits,hit_count[thread_num]);
	}
	//printf("%d:%s has %d hits\n",thread_num,seed,hit_count[thread_num]);
	}


	double sum=0.0;
	int i;
	for (i=0; i<num_threads; i++) {
		sum+=hit_count[i];
	}
	printf("%s have %f\n", b, sum);
	//if (strcmp("TGGAGTGGATGTATTGGGGTGTTCGGAGAGCCAGATAATGTGTTTGTTGTAAAAGGCCAATTGCATATGCTGCTGTCGAACGATAAAGCTGGAAGGAATCCATTTGTA",b)==0) {
	//	exit(1);
	//}
	return sum;
}
