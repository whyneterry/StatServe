#include <lcthw/string_algos.h>
#include <limits.h>
#include <assert.h>

static inline void String_setup_skip_chars(size_t *skip_chars, const unsigned char *needle, ssize_t nlen)
{
	size_t i = 0;
	size_t last = nlen - 1;
	
	for (i = 0; i < UCHAR_MAX + 1; ++i) {
		skip_chars[i] = nlen;
	}

	for (i = 0; i < last; ++i) {
		skip_chars[needle[i]] = last - i;
	}
}

static inline const unsigned char *String_base_search(const unsigned char *haystack,
														ssize_t hlen,
														const unsigned char *needle,
														ssize_t nlen,
														size_t *skip_chars)
{
	assert(haystack != NULL && "Bad haystack.");
	assert(needle != NULL && "Bas needle.");
	assert(hlen > 0 && "Negative hlen.");
	check(nlen > 0, "Negative nlen.");

	size_t i = 0;
	size_t last = nlen - 1;

	while (hlen >= nlen) {
		for (i = last; haystack[i] == needle[i]; --i) {
			if (i == 0) {
				return haystack;
			}
		}
		
		hlen -= skip_chars[haystack[last]];
		haystack += skip_chars[haystack[last]];
	}

error:
	return NULL;
}

int String_find(bstring in, bstring what)
{
	const unsigned char *found = NULL;
	const unsigned char *haystack = (const unsigned char *)bdata(in);
	ssize_t hlen = blength(in);
	const unsigned char *needle = (const unsigned char *)bdata(what);
	ssize_t nlen = blength(what);
	size_t skip_chars[UCHAR_MAX + 1] = { 0 };

	String_setup_skip_chars(skip_chars, needle, nlen);

	found = String_base_search(haystack, hlen, needle, nlen, skip_chars);

	return found != NULL ? found - haystack : -1;
}

StringScanner *StringScanner_create(bstring in)
{
	StringScanner *scan = calloc(1, sizeof(StringScanner));
	check_mem(scan);

	scan->in = in;
	scan->haystack = (const unsigned char *)bdata(in);
	scan->hlen = blength(in);

	assert(scan != NULL && "fuck");
	return scan;

error:
	free(scan);
	return NULL;
}

static inline void StringScanner_set_needle(StringScanner *scan, bstring tofind)
{
	check(scan != NULL, "Invalid StringScanner.");
	check(tofind != NULL, "Invalid tofind string.")

	scan->needle = (const unsigned char *)bdata(tofind);
	scan->nlen = blength(tofind);

	String_setup_skip_chars(scan->skip_chars, scan->needle, scan->nlen);

error:
	return;
}

static inline void StringScanner_reset(StringScanner *scan)
{
	scan->haystack = (const unsigned char *)bdata(scan->in);
	scan->hlen = blength(scan->in);
}

int StringScanner_scan(StringScanner *scan, bstring tofind)
{
	const unsigned char *found = NULL;
	ssize_t found_at = 0;
	ssize_t hlen_shift = 0;	

	if (scan->hlen <= 0) {
		StringScanner_reset(scan);
		return -1;
	}

	if ( (const unsigned char *)bdata(tofind) != scan->needle ) {
		//printf("Changing needle\n");
		StringScanner_set_needle(scan, tofind);
		StringScanner_reset(scan);
	}
	
	found = String_base_search(scan->haystack, scan->hlen, scan->needle, scan->nlen, scan->skip_chars);

	if (found) {
		//printf("Old haystack: %s\nOld hlen: %d\n", scan->haystack, scan->hlen);
		hlen_shift = found - scan->haystack;
		//printf("hlen_shift: %d\n", hlen_shift);
		found_at = found - (unsigned char *)bdata(scan->in);
		scan->haystack = found + scan->nlen;
		scan->hlen = scan->hlen - (hlen_shift + scan->nlen);
		//printf("New haystack: %s\nNew hlen: %d\n", scan->haystack, scan->hlen);
	}
	else {
		// done, reset the setup
		StringScanner_reset(scan);
		found_at = -1;
	}

	return found_at;
}

void StringScanner_destroy(StringScanner *scan)
{
	check(scan != NULL, "Can't destroy nothing");
	free(scan);

error:
	return;	
}
