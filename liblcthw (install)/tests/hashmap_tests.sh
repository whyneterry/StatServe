#include "minunit.h"
#include <lcthw/hashmap.h>
#include <assert.h>
#include <lcthw/bstrlib.h>
#include <time.h>

Hashmap *map = NULL;
static int trav_calls = 0;

struct tagbstring key1 = bsStatic("key111");
struct tagbstring key2 = bsStatic("key222");
struct tagbstring key3 = bsStatic("key333");
struct tagbstring key4 = bsStatic("key444");
struct tagbstring key5 = bsStatic("key555");
struct tagbstring key6 = bsStatic("key666");
struct tagbstring key7 = bsStatic("key777");
struct tagbstring key8 = bsStatic("key888");
struct tagbstring key9 = bsStatic("key999");

struct tagbstring value1 = bsStatic("value111");
struct tagbstring value2 = bsStatic("value222");
struct tagbstring value3 = bsStatic("value333");
struct tagbstring value4 = bsStatic("value444");
struct tagbstring value5 = bsStatic("value555");
struct tagbstring value6 = bsStatic("value666");
struct tagbstring value7 = bsStatic("value777");
struct tagbstring value8 = bsStatic("value888");
struct tagbstring value9 = bsStatic("value999");

static int traverse_good_cb(HashmapNode *node)
{
	debug("KEY: %s", bdata((bstring)node->key));
	trav_calls++;
	return 0;
}

static int traverse_fail_cb(HashmapNode *node)
{
	debug("KEY: %s", bdata((bstring)node->key));
	trav_calls++;
	if (trav_calls == 2) {
		return -1;
	}
	else {
		return 0;
	}
}

char *test_create()
{
	map = Hashmap_create(NULL, NULL);
	mu_assert(map != NULL, "Error with creating Hashmap");	

	return NULL;
}

char *test_destroy()
{
	Hashmap_destroy(map);

	return NULL;
}

char *test_get_set()
{
	int rc;
	rc = Hashmap_set(map, &key1, &value1);
	mu_assert(rc == 0, "Error with setting Hashmap");
	
	rc = Hashmap_set(map, &key2, &value2);
	mu_assert(rc == 0, "Error with setting Hashmap");

	rc = Hashmap_set(map, &key3, &value3);
	mu_assert(rc == 0, "Error with setting Hashmap");
	
	rc = Hashmap_set(map, &key4, &value4);
	mu_assert(rc == 0, "Error with setting Hashmap");
	
	rc = Hashmap_set(map, &key5, &value5);
	mu_assert(rc == 0, "Error with setting Hashmap");

	rc = Hashmap_set(map, &key6, &value6);
	mu_assert(rc == 0, "Error with setting Hashmap");
	
	rc = Hashmap_set(map, &key7, &value7);
	mu_assert(rc == 0, "Error with setting Hashmap");
	
	rc = Hashmap_set(map, &key8, &value8);
	mu_assert(rc == 0, "Error with setting Hashmap");

	rc = Hashmap_set(map, &key9, &value9);
	mu_assert(rc == 0, "Error with setting Hashmap");
	
	bstring temp;
	
	clock_t t;
	t = clock();
	
	Method method = BASIC;	
	temp = (bstring)Hashmap_get(map, &key1, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value1, "Got wrong value");	
	
	temp = (bstring)Hashmap_get(map, &key2, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value2, "Got wrong value");

	temp = (bstring)Hashmap_get(map, &key3, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value3, "Got wrong value");

	temp = (bstring)Hashmap_get(map, &key4, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value4, "Got wrong value");	
	
	temp = (bstring)Hashmap_get(map, &key5, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value5, "Got wrong value");

	temp = (bstring)Hashmap_get(map, &key6, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value6, "Got wrong value");

	temp = (bstring)Hashmap_get(map, &key7, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value7, "Got wrong value");	
	
	temp = (bstring)Hashmap_get(map, &key8, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value8, "Got wrong value");

	temp = (bstring)Hashmap_get(map, &key9, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value9, "Got wrong value");

	t = clock() - t;
	double time_taken = ((double)t) / CLOCKS_PER_SEC;
	printf("Basic get took %f seconds\n", time_taken);	

	t = clock();
	method = BINARY;

	temp = (bstring)Hashmap_get(map, &key1, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value1, "Got wrong value");	
	
	temp = (bstring)Hashmap_get(map, &key2, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value2, "Got wrong value");

	temp = (bstring)Hashmap_get(map, &key3, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value3, "Got wrong value");

	temp = (bstring)Hashmap_get(map, &key4, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value4, "Got wrong value");	
	
	temp = (bstring)Hashmap_get(map, &key5, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value5, "Got wrong value");

	temp = (bstring)Hashmap_get(map, &key6, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value6, "Got wrong value");

	temp = (bstring)Hashmap_get(map, &key7, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value7, "Got wrong value");	
	
	temp = (bstring)Hashmap_get(map, &key8, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value8, "Got wrong value");

	temp = (bstring)Hashmap_get(map, &key9, method);
	mu_assert(temp != NULL, "Error with getting element from Hashmap");
	mu_assert(temp == &value9, "Got wrong value");

	t = clock() - t;
	time_taken = ((double)t) / CLOCKS_PER_SEC;
	printf("Binary get took %f seconds\n", time_taken);
	
	printf("Sorted bucket with key5:\n");	
	uint32_t hash;
    DArray *bucket = Hashmap_find_bucket(map, &key5, 0, &hash);
	DArray_print(bucket);

	printf("Sorted bucket with key2:\n");	
    bucket = Hashmap_find_bucket(map, &key2, 0, &hash);
	DArray_print(bucket);

	return NULL;
}

char *test_traverse()
{
	int rc = Hashmap_traverse(map, traverse_good_cb);
	mu_assert(rc == 0, "Error with traverse_good_cb");
	mu_assert(trav_calls == 9, "Error with tg_cb");

	trav_calls = 0;
	rc = Hashmap_traverse(map, traverse_fail_cb);
	mu_assert(rc == -1, "Error with traverse_fail_cb");
	mu_assert(trav_calls == 2, "Error with tf_cb");

	return NULL;
}

char *all_tests()
{
	mu_suite_start();
	
	mu_run_test(test_create);
	mu_run_test(test_get_set);
	mu_run_test(test_traverse);
	mu_run_test(test_destroy);
	
	return NULL;
}

RUN_TESTS(all_tests);
