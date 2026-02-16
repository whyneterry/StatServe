#include <lcthw/bstree.h>
#include <lcthw/dbg.h>
#include <lcthw/bstrlib.h>
#include <stdlib.h>
#include <stdio.h>

static int def_compare(void *a, void *b)
{
	return bstrcmp( (bstring)a, (bstring)b );
}

// Helper function to print spaces
static void print_spaces(int count) {
    for (int i = 0; i < count; i++) {
        printf(" ");
    }
}

// Recursive helper function to print the tree
static void BSTree_print_pretty(BSTreeNode *node, int depth, int is_left, char *prefix) {
    if (!node) return;
    
    // Create spacing based on depth
    int indent_size = 6; // Adjust this to control spacing
    
    // Print the prefix for this level
    if (depth > 0) {
        print_spaces(depth * indent_size - indent_size);
        if (is_left) {
            printf("├── ");
        } else {
            printf("└── ");
        }
    }
    
    // Print the node value
    if (node->key && node->data) {
        printf("[K:%s D:%s]\n", 
               bdata((bstring)node->key), 
               bdata((bstring)node->data));
    } else if (node->key) {
        printf("[K:%s]\n", bdata((bstring)node->key));
    } else {
        printf("[NULL]\n");
    }
    
    // Recursively print children
    if (node->left || node->right) {
        if (node->left) {
            BSTree_print_pretty(node->left, depth + 1, 1, prefix);
        }
        if (node->right) {
            BSTree_print_pretty(node->right, depth + 1, 0, prefix);
        }
    }
}

void BSTree_print(BSTree *map) {
    check(map != NULL, "Invalid tree.");
    check(map->root != NULL, "Nothing to print.");
    
    printf("\nBinary Search Tree Structure:\n");
    printf("=============================\n");
    
    BSTree_print_pretty(map->root, 0, 0, "");
    
error:
    return;
}

void BSTree_print_node(BSTreeNode *node)
{
	printf("%s\n", bdata((bstring)node->data));
	if (node->right && node->left) {
		printf("/\n");
		BSTree_print_node(node->left);
		printf("\\\\\n");
		BSTree_print_node(node->right);
	}
	else if (node->right) {
		printf("\\\\\n");
		BSTree_print_node(node->right);
	}

	else if (node->left) {
		printf("/\n");
		BSTree_print_node(node->left);
	}
}

BSTree *BSTree_create(BSTree_compare compare)
{
	BSTree *map = calloc(1, sizeof(BSTree));
	check_mem(map);
	map->compare = compare == NULL ? def_compare : compare;
	map->count = 0;

	return map;

error:
	if (map) {
		BSTree_destroy(map);
	}
	return NULL;
}

static int BSTree_destroy_cb(BSTreeNode *node)
{
	check(node != NULL, "Can't free empty node.");
	free(node);
	
	return 0;

error:
	return -1;	
}

void BSTree_destroy(BSTree *map) {
	check(map != NULL, "Can't destroy nothing.");
	
	BSTree_traverse(map, BSTree_destroy_cb);	
	free(map);

error:
	return;
}

static inline BSTreeNode *BSTreeNode_create(BSTreeNode *parent, void *key, void *data)
{
	BSTreeNode *node = calloc(1, sizeof(BSTreeNode));
	check_mem(node);
	
	node->parent = parent;
	node->key= key;
	node->data = data;
	
	return node;

error:
	return NULL;
}

static inline void BSTree_setnode(BSTree *map, BSTreeNode *node, void *key, void *data)
{
	int cmp = map->compare(key, node->key);

	if (cmp <= 0) {
		if (node->left) {
			BSTree_setnode(map, node->left, key, data);
		}
		else {
			node->left = BSTreeNode_create(node, key, data);
		}
	}
	else {
		if (node->right) {
			BSTree_setnode(map, node->right, key, data);
		}
		else {
			node->right = BSTreeNode_create(node, key, data);
		}
	}
}

int BSTree_set(BSTree *map, void *key, void *data)
{
	check(map != NULL, "Can't set to nothing.");
	check(key != NULL, "key is NULL.");
	check(data != NULL, "data is NULL.");

	if (map->root == NULL) {
		// Empty tree, just create a root
		BSTreeNode *node = BSTreeNode_create(NULL, key, data);
		map->root = node;		
		printf("Setting root with a key: %s\n", bdata((bstring)key));
	}
	else {
		BSTree_setnode(map, map->root, key, data);
		printf("Setting node with a key: %s\n", bdata((bstring)key));
	}

	return 0;

error:
	return -1;
}

static inline BSTreeNode *BSTree_getnode(BSTree *map, BSTreeNode *node, void *key)
{
	int cmp = map->compare(key, node->key);
	
	if (cmp == 0) {
		return node;
	}
	else if (cmp < 0) {
		if (node->left) {
			return BSTree_getnode(map, node->left, key);
		}
		else {
			return NULL;
		}
	}
	else {
		if (node->right) {
			return BSTree_getnode(map, node->right, key);
		}
		else {
			return NULL;
		}
	}
}

void *BSTree_get(BSTree *map, void *key)
{
	check(map != NULL, "Invalid BSTree.");
	check(key != NULL, "Invalid key to get.");
	check(map->root != NULL, "Empty BSTree, set it first.");

	BSTreeNode *node = BSTree_getnode(map, map->root, key);
	check(node != NULL, "Error with getting node.");	

	return node->data;

error:
	return NULL;
}

static inline int BSTree_traverse_nodes(BSTreeNode *node, BSTree_traverse_cb traverse_cb)
{
	int rc = 0;
	if (node->right) {
		rc = BSTree_traverse_nodes(node->right, traverse_cb);
		if (rc != 0) {
			return rc;
		}
	}
	if (node->left) {
		rc = BSTree_traverse_nodes(node->left, traverse_cb);
		if (rc != 0) {
			return rc;
		}
	}
	rc = traverse_cb(node);
	
	return rc;
}

int BSTree_traverse(BSTree *map, BSTree_traverse_cb traverse_cb)
{
	check(map != NULL, "Invalid BSTree.");
	check(map->root != NULL, "Empty BSTree, set it first.");

	return BSTree_traverse_nodes(map->root, traverse_cb);

error:
	return -1;
}

static inline BSTreeNode *BSTree_find_min (BSTreeNode *node)
{
	while (node->left) {
		node = node->left;
	}

	return node;
}

static inline void BSTree_replace_node_in_parent(BSTree *map, BSTreeNode *node, BSTreeNode *new_node)
{
	if (new_node) {
		new_node->parent = node->parent;
	}

	if (node->parent) {
		if (node == node->parent->left) {
			node->parent->left = new_node;
		}
		else if (node == node->parent->right) {
			node->parent->right = new_node;
		}
		else {
			printf("Error with replacing.\n");
			return;
		}
	}
	else {
		map->root = new_node;
	}
}

static inline void BSTree_swap(BSTreeNode *a, BSTreeNode *b)
{
	void *temp = a->key;
	a->key = b->key;
	b->key = temp;
	temp = a->data;
	a->data = b->data;
	b->data = temp;
}

void *BSTree_node_delete(BSTree *map, BSTreeNode *node, void *key)
{
	//printf("Compare: %s ? %s\n", bdata((bstring)key), bdata((bstring)node->key));
	int cmp = map->compare(key, node->key);

	if (cmp < 0) {
		if (node->left) {
			return BSTree_node_delete(map, node->left, key);
		}
		else {
			return NULL;
		}
	}
	else if (cmp > 0) {
		if (node->right) {
			return BSTree_node_delete(map, node->right, key);
		}
		else {
			return NULL;
		}
	}
	else {
		if (node->left && node->right) {
			BSTreeNode *successor = BSTree_find_min(node->right);
			BSTree_swap(successor, node);

			BSTree_replace_node_in_parent(map, successor, successor->right);
			return successor;
		}
		else if (node->right) {
			BSTree_replace_node_in_parent(map, node, node->right);
		}
		else if (node->left) {
			BSTree_replace_node_in_parent(map, node, node->left);
		}
		else {
			BSTree_replace_node_in_parent(map, node, NULL);
		}

		return node;
	}
}

void *BSTree_delete(BSTree *map, void *key)
{
	void *data = NULL;
	if (map->root) {
		BSTreeNode *node = BSTree_node_delete(map, map->root, key);

		if (node) {
			data = node->data;
			free(node);
		}		
	}

	return data;
}
