#include <stdlib.h>
#include <stdio.h>

#include "lcthw/stats.h"
#include "lcthw/dbg.h"

Stats *Stats_recreate(double sum,
						double sumsq,
						unsigned long n,
						double min,
						double max)
{
	Stats *st = calloc(1, sizeof(Stats));
	check_mem(st);

	st->sum = sum;
	st->sumsq = sumsq;
	st->n = n;
	st->min = min;
	st->max = max;

	return st;

error:
	return NULL;
}

Stats *Stats_create()
{
	return Stats_recreate(0.0, 0.0, 0L, 0.0, 0.0);
}

double Stats_mean(Stats *st)
{
	return st->sum / st->n;
}

double Stats_stddev(Stats *st)
{
	return sqrt( (st->sumsq - (st->sum * st->sum / st->n)) / (st->n - 1));
}

void Stats_sample(Stats *st, double s)
{
	check(st != NULL, "Invalid Stats.");
	
	st->sum += s;
	st->sumsq += s*s;
	st->n++;
	if (st->n == 1) {
		st->max = s;
		st->min = s;
	}
	if (s < st->min) {
		st->min = s;
	}
	else if (s > st->max) {
		st->max = s;
	}

error:
	return;
}

void Stats_dump(Stats *st)
{
	printf("SUM: %f | "
			"SUMSQ: %f | "
			"N: %lu | "
			"MIN: %f | "
			"MAX: %f | "
			"MEAN: %f | "
			"STDDEV: %f",
			st->sum,
			st->sumsq,
			st->n,
			st->min,
			st->max,
			Stats_mean(st),
			Stats_stddev(st));
	printf("\n");
}

char *Stats_outdump(Stats *st, char *info)
{
	sprintf(info, "SUM: %f | "
			"SUMSQ: %f | "
			"N: %lu | "
			"MIN: %f | "
			"MAX: %f | "
			"MEAN: %f | "
			"STDDEV: %f\n",
			st->sum,
			st->sumsq,
			st->n,
			st->min,
			st->max,
			Stats_mean(st),
			Stats_stddev(st));

	return info;
}
