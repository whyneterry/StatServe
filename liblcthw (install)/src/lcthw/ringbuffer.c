#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lcthw/dbg.h>
#include <lcthw/ringbuffer.h>

RingBuffer *RingBuffer_create(int length)
{
    RingBuffer *buffer = calloc(1, sizeof(RingBuffer));
    buffer->length = length + 1;
    buffer->start = 0;
    buffer->end = 0;
    buffer->buffer = calloc(buffer->length, 1);

    return buffer;
}

void RingBuffer_destroy(RingBuffer * buffer)
{
    if (buffer) {
        free(buffer->buffer);
        free(buffer);
    }
}

int RingBuffer_available_data(RingBuffer *buffer)
{
	if (buffer->end >= buffer->start) {
		return buffer->end - buffer->start;
	}
	else {
		return buffer->length - buffer->start + buffer->end;
	}
}

int RingBuffer_available_space(RingBuffer *buffer)
{
	return buffer->length - 1 - RingBuffer_available_data(buffer);
}

int RingBuffer_write(RingBuffer * buffer, char *data, int length)
{
    check(RingBuffer_available_data(buffer) <= buffer->length - 1, "Buffer is full.");

    check(length <= RingBuffer_available_space(buffer),
        "Not enough space: %d available, %d request",
        RingBuffer_available_space(buffer), length);

    void *result = memcpy(RingBuffer_ends_at(buffer), data, length);
    check(result != NULL, "Failed to write data into buffer.");

    RingBuffer_commit_write(buffer, length);

    return length;
error:
    return -1;
}

int RingBuffer_read(RingBuffer * buffer, char *target, int amount)
{
    check_debug(amount <= RingBuffer_available_data(buffer),
        "Not enough in the buffer: has %d, needs %d",
        RingBuffer_available_data(buffer), amount);

    void *result = memcpy(target, RingBuffer_starts_at(buffer), amount);
    check(result != NULL, "Failed to write buffer into data.");

    RingBuffer_commit_read(buffer, amount);

    if (buffer->end == buffer->start) {
        buffer->start = buffer->end = 0;
    }

    return amount;
error:
    return -1;
}

bstring RingBuffer_gets(RingBuffer * buffer, int amount)
{
    check(amount > 0, "Need more than 0 for gets, you gave: %d ",
        amount);
    check_debug(amount <= RingBuffer_available_data(buffer),
        "Not enough in the buffer.");
	
    bstring result = blk2bstr(RingBuffer_starts_at(buffer), amount);
    check(result != NULL, "Failed to create gets result.");
    check(blength(result) == amount, "Wrong result length.");

    RingBuffer_commit_read(buffer, amount);
    assert(RingBuffer_available_data(buffer) >= 0
        && "Error in read commit.");

	if (buffer->start == buffer->end) {
		buffer->start = buffer->end = 0;
	}

    return result;
error:
    return NULL;
}
