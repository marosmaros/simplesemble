#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include "ll/ll.h"
#include "io/io.h"
#include "kmerize/kmerize.h"
#include "path/path.h"
#include "findmp/findmp.h"


#define MIN(a,b) ((a)>(b)?(b):(a))
#define SET_MIN_OCC 7

#define MP_SEG_LENGTH 500
#define MP_READ_LENGTH 70

ll* l;
ll** kmers;
path* genome_path;
char* output_filename;
char* seed;
int hash_length;

int min_seed_len;
int seed_len;
int max_seed_len;

int ext_len;
int num_reads;

char** reads1FW;
char** reads1RV;
char** reads2FW;
char** reads2RV;

int min_occ;
int value;

void usage(char* name) {
	printf("%s reads1_file reads2_file seed min_seed_len ext_len output_filename num_threads\n",name);
}

void signal_handler(int sig) {
	write_genome_to_file(genome_path->genome,output_filename);
	char buff[256];
	assert(genome_path->len==strlen(genome_path->genome));
	fprintf(stderr,"Genome is at length %d\n",genome_path->len);
	fprintf(stderr,"Paused, enter X to exit\n");
	fgets(buff,255,stdin);
	if (buff[0]=='X' || buff[0]=='x') {
		exit(1);
	}
	fprintf(stderr,"Resumed...\n");
}


int prune_path(path *p) {
	int i;
	path* candidate;
	if (p->len<(MP_SEG_LENGTH)) {
		fprintf(stderr,"Cannot fix with mpairs, seg_A too small! %d vs %d\n%s\n",p->len,MP_SEG_LENGTH*2,p->genome);
		fprintf(stderr,"Continue with most likely...\n");
		assert(p->used_child_paths>0);
		candidate=path_create(p->child_paths[0]->genome);
		path_collapse(p);
	} else {
		path_collapse(p);
		char* seg_A=p->genome+p->len-MP_SEG_LENGTH;
		double m=-1.0; int m_index=-1;
		assert(p->used_child_paths>0);
		for (i=0; i<p->used_child_paths; i++) {
			path* kid=p->child_paths[i];
			printf("Computing findmp, parent %d/%lu, child %d/%lu\n",p->len,strlen(p->genome),kid->len,strlen(kid->genome));
			if ((kid->len-p->len)>=MP_READ_LENGTH) {
				char* seg_B=kid->genome+p->len;
				double c=findmp(seg_A,seg_B,reads1FW,reads2RV,num_reads,30);
				c+=findmp(seg_A,seg_B,reads2FW,reads1RV,num_reads,30);
				//c+=findmp(seg_A,seg_B,reads1RV,reads2FW,num_reads,seed_len+ext_len);
				//c+=findmp(seg_A,seg_B,reads2RV,reads1FW,num_reads,seed_len+ext_len);
				//c+=findmp(seg_A,seg_B,reads1RV,reads2RV,num_reads,seed_len+ext_len);
				//c+=findmp(seg_A,seg_B,reads1FW,reads2FW,num_reads,seed_len+ext_len);
				if (c>m) {
					m=c;
					m_index=i;
				}
			}
		}
		assert(((double)-1.0)==-1.0);
		if (m<10) {
			fprintf(stderr,"Cannot solve ambig!!!!!,m=%f\n",m);
			write_genome_to_file(p->genome,output_filename);
			if (p->used_child_paths>1 || m_index==-1) {
				exit(1);
			}
		} 
		assert(m_index>=0 && m_index<p->used_child_paths);
		fprintf(stderr,"Taking best solution for ambig, %f supporting\n",m);
		candidate=path_create(p->child_paths[m_index]->genome);
		printf("#### LEN: %d, BEST SOL: %f supporting\n",p->len,m);
	}
	//path_print(candidate);
	//printf("CAN GENOM %s\n",candidate->genome);
	fprintf(stderr,"Freeing rest\n");
	for (i=0; i<p->used_child_paths; i++) {
		path * kid=p->child_paths[i];
		free(kid->genome);
		free(kid->child_paths);
		free(kid);	
	}


	//figure out how much to trim
	assert(candidate->len>p->len);
	int trim=(candidate->len-p->len)/2;
	candidate->genome[candidate->len-trim]='\0';
	candidate->len=candidate->len-trim;
	int sz=p->len-MIN(50,p->len);
	assert(sz>0 && sz<p->len);	
	printf("End of current: %s\n",p->genome+sz);
	printf("Begining of next: %s\n",candidate->genome+p->len);
	//free the big guys stuff
	free(p->genome);
	free(p->child_paths);
	*p=*candidate;
	assert(candidate->used_child_paths==0);
	free(candidate);
	return 1;
}
int check_something() {
        int index=kmerize_string_to_int("GGGGGCATGTCTGTCATACATCGCCGC");
        ll* hash_row=kmers[index];
        if (hash_row==NULL) {
                printf("BASD!!\n");
        }
        int y; int s=0;
        for (y=0; y<hash_row->used; y++) {
                s+=hash_row->base[y].count;
                //printf("%s %d\n", hash_row->base[y].s, hash_row->base[y].count);
        }
	return s;
}

int extend(path* p,int problem) {
	//Check if the path has child paths
	//If so extend those
	if (p->used_child_paths>0) {
		int i;
		for (i=0; i<p->used_child_paths; i++) {
			assert(p->child_paths[i]!=p);
			assert(strcmp(p->child_paths[i]->genome,p->genome)!=0);
			extend(p->child_paths[i],problem);
		}
		return 1;
	}
	//get the seed
	assert(strlen(p->genome)==p->len);
	assert(strlen(p->genome)>=seed_len);
	strncpy(seed,p->genome+p->len-seed_len,seed_len);
	seed[seed_len]='\0';
	//printf("Using seed %s of len %lu, min occ %d\n",seed,strlen(seed),min_occ);

	//find out what the counts are
	ll_reset(l);
	int hash_index=kmerize_string_to_int(seed);
	if (hash_index!=-1) {
		ll* hash_row=kmers[hash_index];
		if (hash_row!=NULL) {
			char* s;
			int current;
			for(current=hash_row->head; current!=-1; current=hash_row->base[current].next) {
				//int j;
				s=hash_row->base[current].s;
				if (hash_row->base[current].length>=(seed_len-hash_length+ext_len) && strncmp(s,seed +hash_length,seed_len-hash_length)==0) {
					s+=seed_len-hash_length;
					//for (j=0; s[j]!='\0' && j<ext_len; j++);
					//if (j==ext_len) {
					ll_add(l,s,ext_len,hash_row->base[current].count);
					//}
				}	
			}
		}
	}	
	//get the maxes
	char buff[ext_len+1];
	int how_many;
	char* max_s=ll_max(l,min_occ,&how_many,ext_len);
	/*Extending
	There are three cases
	case 1:
		There were no possible extensions found
		So, reduce the seed size, if its not minimum
			and try again
		Otherwise if seed_size is min already, try to 
			reduce min_occ, and reset seed size to
			try all the seed sizes again with the new
			min_occ
	case 2:
		There was exactly one possible extension found
			This is good! Add the extension and return
	case 3:
		There was more then one possible extension found,
			Increasing seed size at this point will not help,
			since the only way we could have gotten to this point
			is by going through all possible longer seed sizes already.
			So this needs to be resovled by generating the next possible
			paths and using mate pairs to decide which one to pick.

	*/
	//assert(value==check_something());
	if (how_many==0) {
		//try a smaller seed? keep going?
		//if seed is already smallest, try to reduce min_occ ?
		/*int ratio=(max_seed_len-min_seed_len)/(2*SET_MIN_OCC);
		if ((max_seed_len-seed_len)/ratio>(SET_MIN_OCC-min_occ) && min_occ>1) {
			min_occ--;
			seed_len=max_seed_len;
			return extend(p,problem);
		} else if (seed_len>min_seed_len) {
			seed_len--;
			return extend(p,problem);
		} */
		if (seed_len>min_seed_len && seed_len>30) {
			seed_len--;
			return extend(p,problem);
		} else if (min_occ>5) {
			min_occ--;
			seed_len=max_seed_len;
			return extend(p,problem);
		} else if (min_occ<=5 && min_occ>3) {
			min_occ=3;
			seed_len=min_seed_len;
			return extend(p,problem);
		}
		//Start again from the top
		min_occ=SET_MIN_OCC;
		seed_len=max_seed_len;
		printf("FAILED CASE 1,%s\n",seed);
		return -1;
	/*} else if (how_many==1) {
		//if not in a problem area should probably check with
		//findmp to make sure that still have some score?
		//Have a nice unique hit!
		strncpy(buff,max_s,ext_len);
		buff[ext_len]='\0';
		path_extend(p,buff);
		//Start again from the top
		seed_len=max_seed_len;
		min_occ=SET_MIN_OCC;
		return 	1;*/
	} else {
		//Get the top two
		//printf("grabbing top two\n");
		//m=ll_max(l,min_occ,&tmp);
		path * kid =path_add_new_child(p);
		strncpy(buff,max_s,ext_len);
		buff[ext_len]='\0';
		//printf("Extending by %s\n",buff);
		//l->base[m].count=0;
		path_extend(kid,buff);
		
		int tmp=how_many; 
		while (tmp>1) {
			max_s=ll_max(l,min_occ,&tmp,ext_len);
			assert(max_s!=NULL);
			kid =path_add_new_child(p);
			strncpy(buff,max_s,ext_len);
			buff[ext_len]='\0';
			//printf("Extending by %s\n",buff);
			//l->base[m].count=0;
			path_extend(kid,buff);
		}
		printf("Done\n");
			
		//if the program is not already trying to resolve
		// a ambiguous extension
		if (problem==0) {
			int i;
			printf("Len is %d\n",p->len);
			for (i=0; i<(MP_READ_LENGTH/ext_len+3); i++) {
				extend(p,1);
				//printf("Len is %d/%lu, kid %d/%lu\n",p->len,strlen(p->genome),p->child_paths[0]->len,strlen(p->child_paths[0]->genome));
				//this should only have to be called at the end
				//but has some 'issues'
				//shouldnt cause harm by being called here
				path_collapse(p);
				printf("Parent length is %d/%lu, kid length %d/%lu\n",p->len,strlen(p->genome),p->child_paths[0]->len,strlen(p->child_paths[0]->genome));
			}

			//Prune the possible paths using mate pair information
			fprintf(stderr,"Pruning\n");
			prune_path(p);
			//Make sure only one path survives
			assert(p->used_child_paths==0);
			fprintf(stderr,"Done pruning\n");
		}
		//Start again from the top
		seed_len=max_seed_len;
		min_occ=SET_MIN_OCC;	
		return 1;
	}
}



int main (int argc, char** argv) {
	printf("SEG LEN %d\n",MP_SEG_LENGTH);
	//Parse and check command line arguments
	if (argc!=8) {
		usage(argv[0]);
		exit(1);
	}

	/* Parse the arguments */
	//get the read files
	char* reads1_filename=argv[1];
	char* reads2_filename=argv[2];
	assert(sizeof(char)==1);

	//get kmer and seed len
	seed_len=strlen(argv[3]);
	min_seed_len=atoi(argv[4]);
	max_seed_len=seed_len;
	hash_length=min_seed_len;
	if (hash_length>12) {
		hash_length=12;
	}
	seed=(char*)malloc(max_seed_len+1);
	if (seed==NULL) {
		fprintf(stderr,"Failed to allocate memory for seed\n");
		exit(1);
	}
	strcpy(seed,argv[3]);
	//get add on length
	ext_len=atoi(argv[5]);
	output_filename=argv[6];

	/* Set up before going into main loop */
	min_occ=SET_MIN_OCC;
	//set threads
	omp_set_num_threads(atoi(argv[7]));
	fprintf(stderr,"Using files, \"%s\" and \"%s\", seed, \"%s\", len %d\n",reads1_filename,reads2_filename,seed,ext_len);
	
	//Read in the reads, and their reverse complements from file
	reads1FW=get_reads(reads1_filename,&num_reads,&reads1RV);
	reads2FW=get_reads(reads2_filename,&num_reads,&reads2RV);

//	int y;
//	for (y=0; y<num_reads; y++) {
//		printf("Read in %s vs %s\n",reads1FW[y],reads1RV[y]);
//	}
//	exit(1);

	//make a global linked list, to be used later
	l=ll_create(200);

	//Generate all kmers in a hash table
	fprintf(stderr,"Started kmer generation\n");
	kmerize_init(hash_length);
	kmers=kmerize_reads(reads1FW,reads1RV,reads2FW,reads2RV, num_reads);
	//value=check_something();
	//printf("WHat the value is %d\n",value);
	fprintf(stderr,"Done kmer generation\n");
	
	//Set up the signal handler so user can check status
	signal (SIGINT, signal_handler);

	/* The main loop */
	//Keep going until cant assemble any further
	genome_path=path_create(seed);
	while (extend(genome_path,0)>0);

	/* Before the program exits write the genome out to stdout */
	printf("Final genome %s\n",genome_path->genome);
	/* Also write it to a file */
	write_genome_to_file(genome_path->genome,output_filename);
	
	return 0;
}
