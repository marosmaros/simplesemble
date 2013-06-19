#ifndef KMERIZE_H
#include "../ll/ll.h"
#define KMERIZE
struct kmer_element {
	char* s;
	int count;
};
typedef struct kmer_element kmer_element;

struct kmer_array {
	int size;
	kmer_element* head;
};

typedef struct kmer_array kmer_array;
ll** kmerize_reads(char** reads1,char** reads2,char** reads3, char** reads4, int num_reads);
void kmerize_merge(int* r1, int* r2);
int kmerize_init(int h_len);
long long int kmerize_string_to_int(const char* s);
#endif
