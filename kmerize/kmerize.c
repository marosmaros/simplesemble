#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "kmerize.h"
#include "../ll/ll.h"

int hash_len;
long long int size;

long long int kmerize_string_to_int(const char* s) {
	long long int index=0;
	int j;
	for (j=0; j<hash_len; j++) {
		switch (s[j]) {
			case 'C':
				index=(index*4)+0;
				break;
			case 'G':
				index=(index*4)+1;
				break;
			case 'A':
				index=(index*4)+2;
				break;
			case 'T':
				index=(index*4)+3;
				break;
			default:
				if (s[j]!='N') {
					printf("%c %s\n",s[j],s);
				}
				assert(s[j]=='N');
				return -1;
		}
	}
	return index;
}

void kmerize_merge(int* r1, int* r2) {
	int i;
	for (i=0; i<size; i++ ){
		r1[i]+=r2[i];
	}	
}

int kmerize_init(int h_len) {
	hash_len=h_len;
	long long int i=1;
	int j;
	for (j=0; j<hash_len; j++ ) {
		i=i<<2;
		if ((i*sizeof(ll*))<=0) {
			fprintf(stderr,"Kmer len is too long!\n");
			exit(1);
		}
	}
	size=i;
	return 0;
}

ll** kmerize_reads(char** reads1,char** reads2,char** reads3, char** reads4, int num_reads) {
	char ** reads[4];

	reads[0]=reads1;
	reads[1]=reads2;
	reads[2]=reads3;
	reads[3]=reads4;
	if (hash_len==-1) {
		fprintf(stderr,"CALL INIT FIRST!\n");
		exit(1);
	}
	ll** r =(ll**)malloc(sizeof(ll*)*size);
	if (r==NULL) {
		fprintf(stderr,"Failed to allocate memory for hash table\n");
		exit(1);
	}
	int k;
	for (k=0; k<size; k++)
		r[k]=NULL;



	#pragma omp parallel 
	{
	int num_threads=omp_get_num_threads();
	int my_num=omp_get_thread_num();
	int i;
	for (i=0; i<num_reads; i++) {
		int q;
		for (q=0; q<4; q++) {
			int read_len=strlen(reads[q][i]);
			if (read_len>hash_len) {
				int j;
				for (j=0; j<(read_len-hash_len); j++) {
					long long int hash_into=kmerize_string_to_int(reads[q][i]+j);
					if (hash_into>=0 && (hash_into%num_threads==my_num)) {
						if (r[hash_into]==NULL) {
							r[hash_into]=ll_create(2);
						}
						//printf("Just adding %d %s\n",read_len-j,reads[q][i]+j);
						ll_add(r[hash_into],reads[q][i]+j+hash_len,read_len-j-hash_len,1);
					}
				}
			}
		}
	}
	}
	return r;//return l;
}

