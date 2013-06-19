#ifndef LL_H
#define LL_H
struct ll_node {
	char* s;
	int count;
	int next;
	int length;
};
typedef struct ll_node ll_node;

struct ll {
	int size;
	int used;
	int head;
	ll_node * base;
};
typedef struct ll ll;

ll* ll_create(int initial_size);
int ll_add(ll* l, char* s,int length,int count);
void ll_print(ll* l);
char* ll_max(ll* l, int min,int* how_many,int length);
void ll_reset(ll* l);
#endif



