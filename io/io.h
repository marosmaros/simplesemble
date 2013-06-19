#ifndef IO_H
#define IO_H

void write_genome_to_file(char* genome, char* filename);
char* read_in_file(char* filename); 
void reverse(char* d, char* s, int len);
char** get_reads(char* reads_filename,int* num_reads,char*** result_rev_reads);


#endif
