#include <stdio.h>
#include "ll.h"

int main () {
	printf("Tester for ll\n");
	ll * l1 = ll_create(2);
	printf("Head is %d\n",l1->head);
	ll_add(l1,"AAAAAAAAAAAAA",5,1);
	ll_add(l1,"AAAAAAAAAAAAA",5,1);
	ll_add(l1,"AAAAAAAAAAAAA",5,1);
	ll_add(l1,"AAAAAAAAAAAAA",5,1);
	ll_add(l1,"AAAAAAAAAAACCA",5,1);
	ll_add(l1,"AAAAAAAAAAACCA",5,1);
	ll_add(l1,"AAAAAAAAAAACCA",5,1);
	ll_add(l1,"AAAAAAAAAAACCA",5,1);
	ll_add(l1,"AAAAAAAAAAACCA",5,1);
	ll_add(l1,"AAAAAAAAAAACCA",5,1);
	ll_add(l1,"ACAAAAAAAAACCA",5,1);
	ll_add(l1,"ACAAAAAAAAACCA",5,1);
	ll_add(l1,"ACAAAAAAAAACCA",5,1);
	ll_add(l1,"ACAAAAAAAAACCA",5,1);
	ll_print(l1);
	int hw;
	ll_max(l1,300,&hw,5);
	ll_max(l1,300,&hw,5);
	ll_max(l1,300,&hw,5);
	ll_max(l1,300,&hw,5);
	ll_max(l1,300,&hw,5);
	ll_max(l1,300,&hw,5);
	ll_max(l1,300,&hw,5);
	ll_max(l1,300,&hw,5);
	ll_max(l1,300,&hw,5);
	ll_max(l1,300,&hw,5);
	ll_max(l1,300,&hw,5);
	ll_max(l1,300,&hw,5);
	ll_max(l1,300,&hw,5);
	ll_max(l1,300,&hw,5);
	printf("There are %d max's\n",hw);
	ll_max(l1,3,&hw,5);
	printf("There are %d max's\n",hw);
	ll_max(l1,3,&hw,5);
	printf("There are %d max's\n",hw);
	ll_max(l1,3,&hw,5);
	printf("There are %d max's\n",hw);
	ll_max(l1,3,&hw,5);
	ll_max(l1,3,&hw,5);
	ll_max(l1,3,&hw,5);
	ll_max(l1,3,&hw,5);
	ll_max(l1,3,&hw,5);
	ll_max(l1,3,&hw,5);
	ll_max(l1,3,&hw,5);
	ll_max(l1,3,&hw,5);
	ll_print(l1);
	return 0;
}

