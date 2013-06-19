#ifndef PATHS_H
#define PATHS_H

#define INIT_CHILD_PATHS 3

struct path {
	char* genome;
	int len;
	struct path ** child_paths;
	int used_child_paths;
	int size_child_paths;
};

typedef struct path path;

void path_collapse(path* p);
path* path_create(const char* genome);
path* path_add_child(path* p,path* k);
path* path_add_new_child(path* p);
char* path_extend(path* p, char* ext);
void path_print(path * p);
#endif
