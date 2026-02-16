#include <stdlib.h>
#include <stdio.h>
#include <lcthw/dbg.h>
#include <lcthw/tstree.h>

static inline TSTree *TSTree_insert_base(TSTree *root, TSTree *node, const char *key, size_t len, void *value)
{
	if (node == NULL) {
		node = (TSTree *) calloc(1, sizeof(TSTree));
	
		if (root == NULL) {
			root = node;
		}

		node->splitchar = *key;
	}

	if (*key < node->splitchar) {
		node->low = TSTree_insert_base(root, node->low, key, len, value);
	}
	else if (*key == node->splitchar) {
		if (len > 1) {
			node->equal = TSTree_insert_base(root, node->equal, key + 1, len - 1, value);
		}
		else {
			check(node->value == NULL, "Duplicate insert into tst.");
			node->value = value;
		}
	}
	else {
		node->high = TSTree_insert_base(root, node->high, key, len, value);
	}

	return node;

error:
	return NULL;
}

TSTree *TSTree_insert(TSTree *node, const char *key, size_t len, void *value)
{
	return TSTree_insert_base(node, node, key, len, value);
}

void *TSTree_search(TSTree *root, const char *key, size_t len)
{
	TSTree *node = root;
	size_t i = 0;

	while (i < len && node) {
		if (key[i] < node->splitchar) {
			node = node->low;
		}
		else if (key[i] == node->splitchar) {
			i++;
			if (i < len) {
				node = node->equal;
			}
		}
		else {
			node = node->high;
		}
	}

	if (node) {
		return node->value;
	}
	else {
		return NULL;
	}
}

void *TSTree_search_prefix(TSTree *root, const char *key, size_t len)
{
	TSTree *node = root;
	TSTree *last = NULL;
	size_t i = 0;

	while (i < len && node) {
		if (key[i] < node->splitchar) {
			node = node->low;
		}
		else if (key[i] == node->splitchar) {
			i++;
			if (i < len) {
				if (node->equal) {
					last = node;
				}
				node = node->equal;
			}
		}
		else {
			node = node->high;
		}
	}

	node = node ? node : last;

	while (node && !node->value) {
		node = node->equal;
	}

	return node ? node->value : NULL;
}

Collection *TSTree_collect(TSTree *root, const char *key, size_t len)
{
    // Находим узел, соответствующий префиксу
    TSTree *node = root;
    TSTree *last = NULL;
    size_t i = 0;
    
    while (i < len && node) {
        if (key[i] < node->splitchar) {
            node = node->low;
        } else if (key[i] == node->splitchar) {
            i++;
            if (i < len) {
                if (node->equal) {
                    last = node;
                }
                node = node->equal;
            }
        } else {
            node = node->high;
        }
    }
    
    node = node ? node : last;
    if (!node) return NULL;
    
    // Создаем коллекцию
    Collection *coll = collection_init(2, 64);  // Начальный размер 64 символа
    
    // Копируем префикс в ключ
	strncpy(coll->key, key, len - 1);
    //coll->key[len] = '\0';
	printf("[%c] [%s]", node->splitchar, (char *)node->value);
	/* 
    // Если узел префикса сам имеет значение
    if (node->value && len > 0) {
        if (coll->n >= coll->max) {
            coll->max *= 2;
            char **temp = (char **)realloc(coll->keys, coll->max * sizeof(char *));
            check(temp != NULL, "Realloc failed.");
            coll->keys = temp;
        }
        coll->keys[coll->n] = strdup(key);
        coll->n++;
    }
    */
    // Собираем ключи из поддерева
    recursive_collect(node, coll, coll->key, len - 1);
    
    // Добавляем NULL-терминатор
    if (coll->n >= coll->max) {
        coll->max += 1;
        char **temp = (char **)realloc(coll->keys, coll->max * sizeof(char *));
        check(temp != NULL, "Realloc failed.");
        coll->keys = temp;
    }
    coll->keys[coll->n] = NULL;
   
	collection_print(coll);
 
    return coll;
    
error:
    if (coll) collection_destroy(coll);
    return NULL;
}

void collection_print(Collection *coll)
{
	int i = 0;
	for (i = 0; i < coll->n; ++i) {
		printf("[%p] %s\n", coll->keys[i], coll->keys[i]);
	}
	
	printf("[%p] keys\n", coll->keys);
	printf("[%p] key\n", coll->key);
	printf("[%p] [%lu] coll\n", coll, sizeof(coll));
	
	printf("[%p] KS: %lu\n[%p] N: %lu\n[%p] MAX: %lu\n",
										&coll->keysize,
										coll->keysize,
										&coll->n,
										coll->n,
										&coll->max,
										coll->max);
}

Collection *collection_init(size_t max, size_t keysize)
{
	Collection *coll = calloc(1, sizeof(Collection));
	check(coll != NULL, "Error with calloc mem for Collection.");
	coll->keys = calloc(max, sizeof(char *));	
	check(coll->keys != NULL, "Error with calloc mem for keys.");
	coll->key = calloc(keysize, sizeof(char));
	check(coll->key != NULL, "Error with calloc mem for key.");
	coll->n = 0;
	coll->max = max;
	coll->keysize = keysize;

	return coll;

error:
	return NULL;
}

void recursive_collect(TSTree *node, Collection *coll, char *current_key, int depth)
{
    if (!node) return;
    
    // Обрабатываем low ветку
    recursive_collect(node->low, coll, current_key, depth);
    
    // Добавляем текущий символ
    if (depth >= coll->keysize - 1) {
        coll->keysize *= 2;
        char *temp = (char *)realloc(coll->key, coll->keysize * sizeof(char));
        check(temp != NULL, "Realloc failed.");
        coll->key = temp;
    }
    coll->key[depth] = node->splitchar;
    //coll->key[depth + 1] = '\0';
    
    // Если узел имеет значение, сохраняем ключ
    if (node->value) {
        if (coll->n >= coll->max) {
            coll->max *= 2;
            char **temp = (char **)realloc(coll->keys, coll->max * sizeof(char *));
            check(temp != NULL, "Realloc failed.");
            coll->keys = temp;
        }
		coll->key[depth + 1] = '\0';
        coll->keys[coll->n] = strdup(coll->key);
        coll->n++;
    }
    
    // Обрабатываем equal ветку
    recursive_collect(node->equal, coll, coll->key, depth + 1);
    
    // Обрабатываем high ветку
    recursive_collect(node->high, coll, current_key, depth);

error:
	return;
}

void collection_destroy(Collection *coll)
{
	if (coll) {
		if (coll->key) {
			free(coll->key);
		}
		if (coll->keys) {
			for (int i = 0; i < coll->n; ++i) {
				free(coll->keys[i]);
			}
			free(coll->keys);
		}
		free(coll);
	}
}

void TSTree_traverse(TSTree *node, TSTree_traverse_cb cb, void *data)
{
	if (node->low) {
		TSTree_traverse(node->low, cb, data);
	}

	if (node->equal) {
		TSTree_traverse(node->equal, cb, data);
	}

	if (node->high) {
		TSTree_traverse(node->high, cb, data);
	}

	if (node->value) {
		cb(node->value, data);
	}
}

void TSTree_destroy(TSTree *node) {
	if (node->low) {
		TSTree_destroy(node->low);
	}
	
	if (node->low) {
		TSTree_destroy(node->low);
	}
	
	if (node->low) {
		TSTree_destroy(node->low);
	}
	
	free(node);
}
