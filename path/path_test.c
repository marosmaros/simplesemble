#include <stdio.h>
#include "path.h"

int main() {
	path * test = path_create("what the heck is going on ? ");
	path * kid1 = path_add_new_child(test);
	path_extend(kid1,"i dont know");
	path * kid2 = path_add_new_child(test);
	path_extend(kid2," i do ");
	path * kid3 = path_add_new_child(kid1);
	path_extend(kid3," know ");
	path * kid4 = path_add_new_child(kid1);
	path_extend(kid4," no ");
	printf("printing test\n");
	path_print(test);
	path_collapse(test);
	path_collapse(test);
	path_collapse(test);
	path_collapse(test);
	path_collapse(test);
	printf("%s %d, %d\n\n",test->genome,test->len,test->used_child_paths);
	
	path_print(test);
		
	return 0;
}

