#ifndef _LCTHW_TSTREE_H
#define _LCTHW_TSTREE_H

#include <stdlib.h>
#include <lcthw/darray.h>

typedef struct TSTree {
	char splitchar;
	struct TSTree *low;
	struct TSTree *equal;
	struct TSTree *high;
	void *value;
} TSTree;

typedef struct Collection {
	char **keys;
	char *key;
	size_t n;
	size_t max;
	size_t keysize;
} Collection;

void collection_print(Collection *coll);

void collection_destroy(Collection *coll);

void recursive_collect(TSTree *node, Collection *coll, char *current_key, int depth);

Collection *collection_init(size_t max, size_t keysize);

void *TSTree_search(TSTree *root, const char *key, size_t len);

void *TSTree_search_prefix(TSTree * root, const char *key, size_t len);

typedef void (*TSTree_traverse_cb) (void *value, void *data);

TSTree *TSTree_insert(TSTree * node, const char *key, size_t len,void *value);

void TSTree_traverse(TSTree * node, TSTree_traverse_cb cb, void *data);

Collection *TSTree_collect(TSTree *root, const char *key, size_t len);

void TSTree_destroy(TSTree * root);

#endif
