#include "minunit.h"
#include <lcthw/ringbuffer.h>

RingBuffer *buffer = NULL;
int length = 33;

char *test1 = "test data 1";
char *test2 = "test data 2";
char *test3 = "test data 3";

char *test_create()
{
	buffer = RingBuffer_create(length);
	mu_assert(buffer != NULL, "RingBuffer_create FAILED.");
	
	return NULL;
}

char *test_write_read()
{
	int rc = RingBuffer_write(buffer, test1, strlen(test1));
	printf("AD: %d\n", RingBuffer_available_data(buffer));
	printf("START: %d\n", buffer->start);
	printf("END: %d\n", buffer->end);

	mu_assert(rc != -1, "Error with write to the buffer.");
	
	rc = RingBuffer_write(buffer, test2, strlen(test2));
	printf("AD: %d\n", RingBuffer_available_data(buffer));
	printf("START: %d\n", buffer->start);
	printf("END: %d\n", buffer->end);

	mu_assert(rc != -1, "Error with write to the buffer.");
	
	rc = RingBuffer_write(buffer, test3, strlen(test3));
	printf("AD: %d\n", RingBuffer_available_data(buffer));	
	printf("START: %d\n", buffer->start);
	printf("END: %d\n", buffer->end);

	mu_assert(rc != -1, "Error with write to the buffer.");

	rc = RingBuffer_write(buffer, test3, strlen(test3));
	printf("AD: %d\n", RingBuffer_available_data(buffer));	
	printf("START: %d\n", buffer->start);
	printf("END: %d\n", buffer->end);
	
	mu_assert(rc == -1, "Should be an error (buffer is full).");

	char *data = calloc(12, sizeof(char));
	for (int i = 0; i < 3; i++){
		rc = RingBuffer_read(buffer, data, strlen(test1));
		mu_assert(rc != -1, "Error with reading from a buffer.");	
	
		printf("START: %d\n", buffer->start);
		printf("END: %d\n", buffer->end);
		printf("[%d] %s\n", strlen(data), data);
	}
	
	rc = RingBuffer_write(buffer, test3, strlen(test3));
	printf("AD: %d\n", RingBuffer_available_data(buffer));	
	printf("START: %d\n", buffer->start);
	printf("END: %d\n", buffer->end);

	mu_assert(rc != -1, "Error with write to the buffer.");

	rc = RingBuffer_read(buffer, data, strlen(test3));
	mu_assert(rc != -1, "Error with reading from the buffer."); 
	printf("%s\n", data);

	free(data);

	return NULL;
}

char *test_destroy()
{
	RingBuffer_destroy(buffer);
	
	return NULL;
}

char *all_tests()
{
	mu_suite_start();
	
	mu_run_test(test_create);
	mu_run_test(test_write_read);
	mu_run_test(test_destroy);
	
	return NULL;
}

RUN_TESTS(all_tests);
