#include "minunit.h"
#include "net.h"

struct tagbstring db_path = bsStatic("storage");

char *test_parse_request()
{
	printf("=========TEST_PARSE_REQUEST========\n");
	BSTree *tree = BSTree_create(NULL);	
	mu_assert(tree != NULL, "Invalid tree.");

	SharedData *s_data = calloc(1, sizeof(SharedData));
	mu_assert(s_data != NULL, "Calloc failed.");
	
	s_data->map = tree;

	bstring test = bfromcstr("age/");
	struct bstrList *list = bsplit(test, '/');
	mu_assert(list != NULL, "Error with spliting.");
	
	for (int i = 0; i < list->qty; ++i) {
		printf("qty: %d, entry[%d]: %s [len: %d]\n", list->qty, i, bdata(list->entry[i]), blength(list->entry[i]));
	}
	bstrListDestroy(list);

	bstring create = bfromcstr("/create logs\n");	
	list = bsplit(create, ' ');

	int rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	bstring sample = bfromcstr("/sample logs 10");	
	list = bsplit(sample, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	bstring sample_2 = bfromcstr("/sample logs 100");	
	list = bsplit(sample_2, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	bstring dump = bfromcstr("/dump logs");	
	list = bsplit(dump, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");
	
	bstring mean = bfromcstr("/mean logs");	
	list = bsplit(mean, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");
	
	bstring delete = bfromcstr("/delete logs");	
	list = bsplit(delete, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");
		
	list = bsplit(dump, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == -1, "Should fail after delete");
	
	bstring list_ = bfromcstr("/list commands");	
	list = bsplit(list_, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");
	
	bstring junk_1 = bfromcstr("gsdfg sdfgsdfgsdfg sdf");	
	bstring junk_2 = bfromcstr("/create logs loxs");	
	bstring junk_3 = bfromcstr("5 345 3245 2345 2345234 ");	
	bstring junk_4 = bfromcstr("/delete admin");
	bstring junk_5 = bfromcstr("/sample admin_panel yes yes");	
	
	list = bsplit(junk_1, ' ');
	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == -1, "Should fail with junk");
	
	list = bsplit(junk_2, ' ');
	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == -1, "Should fail with junk");
	
	list = bsplit(junk_3, ' ');
	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == -1, "Should fail with junk");
	
	list = bsplit(junk_4, ' ');
	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == -1, "Should fail with junk");
		
	list = bsplit(junk_5, ' ');
	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == -1, "Should fail with junk");

	bdestroy(create);
	bdestroy(sample);
	bdestroy(sample_2);
	bdestroy(dump);
	bdestroy(mean);
	bdestroy(delete);
	bdestroy(list_);
	bdestroy(junk_1);
	bdestroy(junk_2);
	bdestroy(junk_3);
	bdestroy(junk_4);
	bdestroy(junk_5);

	BSTree_destroy(tree);
	free(s_data);
	
	return NULL;
}

char *test_mult_parse_request()
{	
	printf("=========TEST_MULT_PARSE_REQUEST========\n");
	int rc = -1;
	BSTree *tree = BSTree_create(NULL);	
	mu_assert(tree != NULL, "Invalid tree.");

	SharedData *s_data = calloc(1, sizeof(SharedData));
	mu_assert(s_data != NULL, "Calloc failed.");
	
	s_data->map = tree;

	bstring mult_create = bfromcstr("/create /prices/ages");
	struct bstrList *list = bsplit(mult_create, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	bstring mult_sample = bfromcstr("/sample /prices/ages 69.69");
	list = bsplit(mult_sample, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	mult_sample = bfromcstr("/sample /prices/ages 0.69");
	list = bsplit(mult_sample, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	mult_sample = bfromcstr("/sample /prices 228");
	list = bsplit(mult_sample, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");
	
	bstring mult_dump = bfromcstr("/dump /prices/ages");
	list = bsplit(mult_dump, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	bstring mult_mean = bfromcstr("/mean /prices/ages");
	list = bsplit(mult_mean, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	bstring mult_delete = bfromcstr("/delete /prices/ages");
	list = bsplit(mult_delete, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	list = bsplit(mult_dump, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == -1, "Error parsing");

	bdestroy(mult_create);
	bdestroy(mult_dump);
	bdestroy(mult_mean);
	bdestroy(mult_sample);
	bdestroy(mult_delete);

	BSTree_destroy(tree);
	free(s_data);

	return NULL;
}

char *test_store_load()
{
	printf("==========TEST_STORE_LOAD=========\n");
	int rc = -1;
	BSTree *tree = BSTree_create(NULL);	
	mu_assert(tree != NULL, "Invalid tree.");

	SharedData *s_data = calloc(1, sizeof(SharedData));
	mu_assert(s_data != NULL, "Calloc failed.");
	
	s_data->map = tree;

	bstring create = bfromcstr("/create prices\n");	
	struct bstrList *list = bsplit(create, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	bstring sample = bfromcstr("/sample /prices 69.69\n");
	list = bsplit(sample, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	bstring store = bfromcstr("/store prices");
	list = bsplit(store, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");
	
	bstring delete = bfromcstr("/delete prices");
	list = bsplit(delete, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing.");

	list = bsplit(store, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == -1, "Should fail. Prices don't exist.");

	printf("=============HERE==========\n");

	bstring load_prices = bfromcstr("/load /prices");
	list = bsplit(load_prices, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	bstring load = bfromcstr("/load /prices/grades");
	list = bsplit(load, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	bstring dump = bfromcstr("/dump /prices/grades");
	list = bsplit(dump, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");
	
	bstring load_2 = bfromcstr("/load prices/test");
	list = bsplit(load_2, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");
	
	bstring dump_2 = bfromcstr("/dump /test");
	list = bsplit(dump_2, ' ');

	rc = parse_request(s_data, list, 1, &db_path, 0);
	bstrListDestroy(list);
	mu_assert(rc == 0, "Error parsing");

	bdestroy(create);
	bdestroy(sample);
	bdestroy(store);
	//bdestroy(delete);
	bdestroy(load);
	bdestroy(load_2);
	bdestroy(load_prices);
	bdestroy(dump);
	bdestroy(dump_2);

	BSTree_destroy(tree);
	free(s_data);

	return NULL;
}

char *test_create_stats()
{
	printf("==========TEST_CREATE_STATS=========\n");
	int rc = 0;
	BSTree *tree = BSTree_create(NULL);	
	mu_assert(tree != NULL, "Invalid tree.");

	SharedData *s_data = calloc(1, sizeof(SharedData));
	mu_assert(s_data != NULL, "Calloc failed.");
	
	s_data->map = tree;

	struct bstrList *urls;
	
	struct tagbstring create = bsStatic("/logs/zed");
	struct tagbstring create_2 = bsStatic("logs/mary");
	struct tagbstring create_3 = bsStatic("/logs/zed");
	struct tagbstring create_4 = bsStatic("logs");
	urls = bsplit(&create, '/');
	Command fake_cmd = { .s_data = s_data,
					.cl_sock_fd = 0,
					.is_sock = 0,
					.db_path = &db_path,
					.binfo = bfromcstr("fakebinfo"),
					.urls = urls };

	rc = create_stats(&fake_cmd);
	bstrListDestroy(urls);
	mu_assert(rc == 0, "Error with create_stats");

	urls = bsplit(&create_2, '/');
	fake_cmd.urls = urls;	

	rc = create_stats(&fake_cmd);
	bstrListDestroy(urls);
	mu_assert(rc == 0, "Error with create_stats");
	
	urls = bsplit(&create_3, '/');
	fake_cmd.urls = urls;	

	rc = create_stats(&fake_cmd);
	bstrListDestroy(urls);
	mu_assert(rc == 0, "Should fail");
	
	urls = bsplit(&create_4, '/');
	fake_cmd.urls = urls;	

	rc = create_stats(&fake_cmd);
	bstrListDestroy(urls);
	mu_assert(rc == 0, "Should fail");

	BSTree_destroy(tree);
	free(s_data);

	return NULL;
}

char *test_hash()
{
	bstring key = bfromcstr("somekey");
	
	bstring hash = NULL;
	hash = md5hash(key, 8);

	mu_assert(hash != NULL, "Error with md5hash");

	printf("%s\n", bdata(hash));
	
	return NULL;
}

char *all_tests()
{
	mu_suite_start();

	mu_run_test(test_parse_request);
	mu_run_test(test_mult_parse_request);
	mu_run_test(test_store_load);
	mu_run_test(test_hash);
	mu_run_test(test_create_stats);

	return NULL;
}

RUN_TESTS(all_tests);
