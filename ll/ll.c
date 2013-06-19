#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ll.h"


void ll_reset(ll* l) {
	l->head=-1;
	l->used=0;
}

void ll_double(ll* l) {
	l->base=realloc(l->base,sizeof(ll_node)*l->size*2);
	if (l->base==NULL) {
		fprintf(stderr,"failed to get enough mem for ll resize\n");
		exit(1);
	}
	l->size*=2;
}


char* ll_max(ll* l,int min,int * how_many, int length) {
	int m=min,m_index=-1;
	*how_many=0;
	int current=l->head;
	//rehash all the row values in a seperate linked list
	//of the new length
	ll * temp=ll_create(l->used);
	while (current!=-1) {
		ll_add(temp,l->base[current].s,length,l->base[current].count);
		current=l->base[current].next;
	}
	//Get the max of this new list
	current = temp->head;
	while (current!=-1) {
		if (temp->base[current].count>=min) {
			(*how_many)++;	
		}
		if (temp->base[current].count>=m) {
			m_index=current;
			m=temp->base[current].count;
		}
		current=temp->base[current].next;
	}
	char* ret=NULL;
	if (m_index!=-1) {
		ret=temp->base[m_index].s;
		//take down the counts from the previous list
		current = l->head;
		while (current!=-1) {
			if (strncmp(l->base[current].s,ret,length)==0) {
				l->base[current].count=0;
			}
			current=l->base[current].next;
		}	
	}
	free(temp->base);
	free(temp);
	
	return ret;
}

int ll_add(ll* l, char* s, int length, int count) {
	//printf("Adding %s\n",s);
	if (l->used==l->size) {
		ll_double(l);
	}
	assert(l->used<l->size);
	if (l->head==-1) {
		l->head=0;
		l->base[0].count=count;
		l->base[0].s=s;
		l->base[0].next=-1;
		l->base[0].length=length;
		l->used++;
		return count;
	}
	int prev=-1;
	int next=l->head;
	//Keep going until hit or pass correct spot
	while (next>=0 && l->base[next].length>length) {
		prev=next;
		next=l->base[next].next;
	}
	while (next>=0 && l->base[next].length==length && strncmp(l->base[next].s,s,length)<0) {
		prev=next;
		next=l->base[next].next;
	}
	//Should have either next<0 or (smaller or equal length and smaller or equal string) 
	if (next<0 || l->base[next].length!=length || strncmp(l->base[next].s,s,length)!=0) {
		int current=l->used++;
		if (prev==-1) {
			l->head=current;
		} else {
			l->base[prev].next=current;
		}
		l->base[current].s=s;
		l->base[current].count=count;
		l->base[current].next=next;
		l->base[current].length=length;
		return l->base[current].count;
	} else {
		assert(strncmp(l->base[next].s,s,length)==0);
		l->base[next].count+=count;
		return l->base[next].count;
	}
	return 0;
}

ll* ll_create(int initial_size) {
	ll* r=(ll*)malloc(sizeof(ll));
	if (r==NULL) {
		fprintf(stderr,"Could not malloc new ll\n");
		exit(1);
	}
	r->base=(ll_node*)malloc(sizeof(ll_node)*initial_size);
	if (r->base==NULL) {
		fprintf(stderr,"Could not allocate enough mem for linked list nodes\n");
		exit(1);
	}
	r->head=-1;
	r->size=initial_size;
	r->used=0;	
	return r;
}


void ll_print(ll* l) {
	char buff[2000];
	int current=l->head;
	while (current!=-1) {
		strncpy(buff,l->base[current].s,l->base[current].length);
		buff[l->base[current].length]='\0';
		printf("|%s| %d\n",buff,l->base[current].count);
		current=l->base[current].next;
	}

}
