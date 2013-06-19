#include <stdio.h>
#include <omp.h>
#include "kmerize.h"
#include "../io/io.h"

int main(int argc, char** argv) {
	char * filename=argv[1];
	char** readsFW;
	char** readsRV;
	int num_reads;
	omp_set_num_threads(1);
	readsFW=get_reads(filename,&num_reads,&readsRV);
	kmerize_init(10);
	kmerize_reads(readsFW,readsFW,readsFW,readsFW,num_reads);	
	return 0;
}
