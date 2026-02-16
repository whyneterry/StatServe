#include <lcthw/bstrlib.h>
#include "minunit.h"
#include <stdio.h>

char *test_bstr()
{
	bstring bstr = bfromcstr("xyesos");
	printf("bstr: %s\n", bdata(bstr));

	bstr = blk2bstr("I am ", 5);
	printf("bstr: %s\n", bdata(bstr));

	bstring bstrclone;
	bstrclone = bstrcpy(bstr);
	printf("bstrclone: %s\n", bdata(bstrclone));

	char *cstr = "sw4g";
	int rc = bassigncstr(bstr, cstr);
	mu_assert(rc == 0, "Error with bassigncstr");
	printf("bstr: %s\n", bstr->data);

	rc = bconcat(bstrclone, bstr);
	mu_assert(rc == 0, "Error with bconcat");
	printf("bstrclone: %s\n", bdata(bstrclone));

	//bstr = bstrcpy(bstrclone);
	rc = bstricmp(bstrclone, bstr);
	if (rc > 0) {
		printf("%s > %s\n", bdata(bstrclone), bdata(bstr));
	}
	else if (rc < 0) {
		printf("%s < %s\n", bdata(bstrclone), bdata(bstr));
	}
	else {
		printf("%s = %s\n", bdata(bstrclone), bdata(bstr));
	}

	int id = binstr(bstrclone, 0, bstr);
	printf("Found index: %d | What: %s | Found: %s | Where: %s\n", id, bdata(bstr), &(bdata(bstrclone)[id]), bdata(bstrclone));

	struct bstrList *List = bstrListCreate();
	List = bsplit(bstrclone, ' ');
	printf("qty: %d | mlen: %d | bstr: %s\n", List->qty, List->mlen, bdata( *(List->entry + 1) ) );

	char *name = "Pavel";
	int age = 19;
	bstring bstrfmt = bformat("My name is %s and I'm %d years old", name, age);	
	printf("%s\n", bdata(bstrfmt));
	return NULL;
}

char *all_tests()
{
	mu_suite_start();
	
	mu_run_test(test_bstr);

	return NULL;
}

RUN_TESTS(all_tests);
