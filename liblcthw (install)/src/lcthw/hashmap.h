#ifndef LCTHW_HASHMAP_H
#define LCTHW_HASHMAP_H

#include <stdint.h>
#include <lcthw/darray.h>
#include <lcthw/darray_algos.h>

#define DEFAULT_NUMBER_OF_BUCKETS 2

typedef int (*Hashmap_compare) (void *a, void *b);
typedef uint32_t (*Hashmap_hash) (void *key);

typedef enum Methods {
	BASIC,
	BINARY
} Method;

typedef struct Hashmap {
	DArray *buckets;
	Hashmap_compare compare;
	Hashmap_hash hash;
} Hashmap;

typedef struct HashmapNode {
	void *key;
	void *data;
	uint32_t hash;
} HashmapNode;

typedef int (*Hashmap_traverse_cb) (HashmapNode *node);

Hashmap *Hashmap_create(Hashmap_compare compare, Hashmap_hash);

void Hashmap_destroy(Hashmap *map);

int Hashmap_set(Hashmap *map, void *key, void *data);

void *Hashmap_get(Hashmap *map, void *key, int method);

int Hashmap_traverse(Hashmap *map, Hashmap_traverse_cb traverse_cb);

void *Hashmap_delete(Hashmap *map, void *key);

DArray *Hashmap_find_bucket(Hashmap *map, void *key, int create, uint32_t *hash_out);

#endif
