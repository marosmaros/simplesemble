#include <stdio.h>
#include "io.h"

int main(int argc, char** argv) {
	char** readsFW;
	char** readsRV;
	int num_reads;
	readsFW = get_reads(argv[1],&num_reads,&readsRV);
	int i;
	for (i=0; i<num_reads; i++) {
		printf("%s %s\n",readsFW[i],readsRV[i]);
	}	
	return 0;
}
