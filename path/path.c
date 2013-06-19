#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "path.h"

path* path_create(const char* genome) {
	//alloc the path struct
	path* p = (path*)malloc(sizeof(path));
	if (p==NULL) {
		fprintf(stderr,"Failed to alloc mem for a path\n");
		exit(1);
	}
	//alloc pointers to kids
	p->child_paths=(path**)malloc(sizeof(path*)*INIT_CHILD_PATHS);
	if (p->child_paths==NULL) {
		fprintf(stderr,"Failed to alloc memfor child paths\n");
		exit(1);
	}

	//alloc and copy genome
	p->genome=(char*)malloc(strlen(genome)+1);
	if (p->genome==NULL) {
		fprintf(stderr,"Failed to alloc new genome\n");
		exit(1);
	}
	strcpy(p->genome,genome);
	p->len=strlen(p->genome);

	//zero out the kid pointers
	int i;
	for (i=0; i<INIT_CHILD_PATHS; i++) {
		p->child_paths[i]=NULL;
	}

	//set the digits
	p->used_child_paths=0;
	p->size_child_paths=INIT_CHILD_PATHS;
	return p;
}

path* path_add_child(path* p,path* k) {
	//make sure got enough room
	if (p->used_child_paths==p->size_child_paths) {
		p->child_paths=realloc(p->child_paths,sizeof(path*)*p->size_child_paths*2);
		if (p->child_paths==NULL) {
			fprintf(stderr,"Failed to update child paths\n");
			exit(1);
		}
		p->size_child_paths=p->size_child_paths*2;
		int i;
		for (i=p->used_child_paths; i<p->size_child_paths; i++) {
			p->child_paths[i]=NULL;
		}
	}	
	assert(p->child_paths[p->used_child_paths]==NULL);
	p->child_paths[p->used_child_paths]=k;	
	return 	p->child_paths[p->used_child_paths++];
}
path* path_add_new_child(path* p) {
	path* k =path_create(p->genome);	
	path_add_child(p,k);
	return 	k;
}
	
char* path_extend(path* p, char* ext) {
	assert(strlen(ext)<15);
	assert(strlen(p->genome)==p->len);
	p->genome=realloc(p->genome,p->len+strlen(ext)+1);
	if (p->genome==NULL) {
		fprintf(stderr,"BAD!\n");
		exit(1);
	}
	strcpy(p->genome+p->len,ext);
	assert(strlen(p->genome)==(p->len+strlen(ext)));
	p->len=strlen(p->genome);
	//printf("Extending with |%s| |%s|\n",ext,p->genome);
	return p->genome;
}

void path_print(path * p) {
	printf("%s\n",p->genome);
	int i;
	for (i=0; i<p->used_child_paths; i++) {
		path_print(p->child_paths[i]);
	}
}
void path_collapse_acc(path* p,path* acc) {
	int num_kids=p->used_child_paths;
	if (num_kids==0) {
		path_add_child(acc,p);
		return;
	}
	int i;
	for(i=0; i<num_kids; i++) {
		path_collapse_acc(p->child_paths[i],acc);
	}
	for (i=0; i<num_kids; i++) {
		p->child_paths[i]=NULL;
	}
	free(p->child_paths);
	p->genome=NULL;
	free(p->genome);
	//free(p);
}

void path_collapse(path* p) {
	path* acc=path_create(p->genome);
	path_collapse_acc(p,acc);
	*p=*acc;
	free(acc);
}
