#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "io.h"

void write_genome_to_file(char* genome, char* filename) {
	FILE * fptr=fopen(filename,"w");
	if (fptr==NULL) {
		fprintf(stderr,"Failed to open file to write out genome\n");
		exit(1);
	}
	int perline=80;
	char buff[perline+1];
	int genome_len=strlen(genome);
	fprintf(fptr,">Genome_LEN_%lu\n",(long unsigned)strlen(genome));
	int index;
	for (index=0; index<genome_len; index+=perline) {
		strncpy(buff,genome+index,perline);
		buff[perline]='\0';
		fprintf(fptr,"%s\n",buff);
	}
	fclose(fptr);
}

char* read_in_file(char* filename) {
	//Get the file size
	struct stat file_stat;
	stat(filename, &file_stat);	
	unsigned int file_size=file_stat.st_size;
	fprintf(stderr,"File size is: %u bytes\n",file_size);
	//malloc enouch memory
	char* data=(char*)malloc(file_size+1);
	//open the file
	FILE * fptr=fopen(filename,"r");
	if (fptr==NULL) {
		fprintf(stderr,"There has been an error opening the file \"%s\"\n",filename);
		exit(1);
	}
	//Read the file in
	int res=fread(data,file_size,1,fptr);
	if (res!=1) {
		fprintf(stderr,"There was an error when reading from the file \"%s\"\n",filename);
		exit(1);
	}
	fprintf(stderr,"have read in %lu chars, of %lu file size\n",(long unsigned)strlen(data),(long unsigned)file_size);
	if (data[file_size-1]!='\n') {
		data[file_size-1]='\n';
		data[file_size]='\0';
	}
	return data;
}

void reverse(char* d, char* s, int len) {
	int i;
	for (i=0; i<len; i++) {
		switch (s[i]) {
			case 'A':
				d[len-1-i]='T';
				break;
			case 'C':
				d[len-1-i]='G';
				break;
			case 'G':
				d[len-1-i]='C';
				break;
			case 'T':
				d[len-1-i]='A';
				break;
			default:
				d[len-1-i]=s[i];
		}
	}
	d[len]='\0';
}


char** get_reads(char* reads_filename,int* num_reads,char*** result_rev_reads) {
	//find out how many reads there are
	char* data=read_in_file(reads_filename);
	int data_size=strlen(data);
	*num_reads=0;
	int i;
	assert(data[data_size-1]=='\n');
	data[data_size-1]='\n';
	for (i=0; i<data_size; i++) {
		if (data[i]=='\n') {
			(*num_reads)++;
		}
	}
	fprintf(stderr,"Read in %d lines\n",*num_reads);

	//get the reversed data
	char* rev_data=(char*)malloc(data_size+1);

	//get pointers to each read, and fix up the new lines
	//malloc enogh mem
	char** reads=(char**)malloc(sizeof(char*)*(*num_reads));
	char** rev_reads=(char**)malloc(sizeof(char*)*(*num_reads));
	if (reads==NULL || rev_reads==NULL) {
		fprintf(stderr,"failed to allocate mem for reads\n");
		exit(1);
	}
	int index=0;
	reads[index++]=data; //put in the first read
	for (i=0; i<data_size; i++) {
		if (data[i]=='\n') {
			data[i]='\0';
			if (index<*num_reads) {
				reads[index++]=data+i+1;
			}
		}
	}

	index=0;
	for (i=0; i<(*num_reads); i++) {
		int l=strlen(reads[i]);
		rev_reads[i]=rev_data+index;
		reverse(rev_data+index,reads[i],l);
		index+=l+1;
	}

	*result_rev_reads=rev_reads;

	return reads;
}

