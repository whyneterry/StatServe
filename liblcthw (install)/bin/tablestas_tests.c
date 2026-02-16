#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lcthw/stats.h"
#include "lcthw/dbg.h"

typedef struct HStats {
	char *header;
	Stats *stats;
} Hstats;

char **readUntil(FILE *fptr, char to)
{
	char *buffer = calloc(128, sizeof(char));
	check_mem(buffer);

	char **headers = calloc(10, sizeof(char *));
	check_mem(headers);

	int c;
	int pos = 0, count = 0;

	while ( (c = fgetc(fptr)) != EOF && c != '\n') {
		if (c != (int)to) {
			buffer[pos++] = (char)c;
		}
		else if (c == (int)c && pos > 0) {
			buffer[pos] = '\0';
			char *header = strdup(buffer);
			headers[count++] = header;
			pos = 0;
		}
	}

	if (pos > 0) {
		buffer[pos] = '\0';
		char *header = strdup(buffer);
		headers[count++] = header;
	}
	else {
		goto error;
	}

	free(buffer);

	return headers;

error:
	if (buffer) {
		free(buffer);
	}
	if (headers) {
		int i = 0;
		for (i = 0; i < count; i++) {
			free(headers[i]);
		}
		free(headers);
	}

	return NULL;
}

int printWords(char **words)
{
	int i = 0;
	while (words[i] != NULL) {
		//printf("%s | ", words[i]);
		++i;
	}
	//printf("\n");
	
	return i;
}

int main(int argc, char *argv[])
{
	FILE *fptr = fopen("testtable.txt", "r");
	check(fptr, "Failed to open file.");

	char **headers = readUntil(fptr, ' ');
	int count = printWords(headers);

	Hstats *hstats = calloc(count, sizeof(Hstats));

	int i = 0;
	char **stats;
	while ( (stats = readUntil(fptr, ' ')) != NULL) {
	
		for (i = 0; i < count; i++) {
			if (hstats[i].header == NULL) {
				hstats[i].header = headers[i];
			}
			if (hstats[i].stats == NULL) {
				hstats[i].stats = Stats_create();
			}
					
			Stats_sample(hstats[i].stats, (double)atoi(stats[i]));
		}
	}

	for (i = 0; i < count; i++) {
		printf("%s:\n", hstats[i].header);
		Stats_dump(hstats[i].stats);
	}

error:
	return -1;
}
