/*
	Copyright (C) 2014-2021 Igor van den Hoven ivdhoven@gmail.com
*/

/*
	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	The person recognizes Mars as a free planet and that no Earth-based
	government has authority or sovereignty over Martian activities.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
	wolfsort 1.1.2 (previously flowsort)
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#ifndef QUADSORT_H
  #include "quadsort.h"
#endif

#ifndef QUADSORT_CPY_H
  #include "quadsort_cpy.h"
#endif

//#define cmp(a,b) (*(a) > *(b))

//typedef int CMPFUNC (const void *a, const void *b);

void wolfsort(void *array, size_t nmemb, unsigned char size, CMPFUNC *ignore)
{
	if (nmemb < 1024)
	{
		return quadsort(array, nmemb, size, ignore);
	}

	if (size == sizeof(int))
	{
		int *swap, *pta, *pts;
		unsigned int index, cnt, *stack;
		unsigned short *count, bsize;
		unsigned int buckets = 256;
		unsigned int moduler = 16777216;

		swap = (int *) malloc(nmemb * size);

		if (swap == NULL)
		{
			return quadsort32(array, nmemb, size, ignore);
		}

		while (moduler > 8096 && nmemb / buckets > 4)
		{
			buckets *= 2;
			moduler /= 2;
		}

		bsize = nmemb / (buckets / 16);

		count = (unsigned short *) calloc(sizeof(short), buckets);
		stack = (unsigned int *) calloc(sizeof(int), buckets);

		pta = (int *) array;

		for (cnt = nmemb ; cnt ; cnt--)
		{
			index = (unsigned int) *pta++ / moduler;

			if (++count[index] == bsize)
			{
				quadsort_swap32(array, swap, nmemb, size, ignore);

				free(swap);
				free(stack);
				free(count);

				return;
			}
		}

		cnt = 0;

		for (index = 0 ; index < buckets ; index++)
		{
			stack[index] = cnt;

			cnt += count[index];
		}

		pta = (int *) array;

		for (cnt = nmemb ; cnt ; cnt--)
		{
			index = (unsigned int) *pta / moduler;

			swap[stack[index]++] = *pta++;
		}

		pta = (int *) array;
		pts = (int *) swap;

		for (index = 0 ; index < buckets ; index++)
		{
			bsize = count[index];

			if (bsize)
			{
				if (bsize <= 32)
				{
					tail_swap32_cpy(pta, pts, bsize, ignore);
				}
				else
				{
					memcpy(pta, pts, bsize * size);

					quadsort_swap32(pta, pts, bsize, size, ignore);
				}
				pta += bsize;
				pts += bsize;
			}

		}
		free(count);
		free(swap);
		free(stack);
		return;
	}

	if (size == sizeof(long long))
	{
		long long *swap, *pta, *pts;
		unsigned int index, cnt, *stack;
		unsigned int *count, bsize;
		unsigned int buckets = 256;
		unsigned long long moduler = 72057594037927936ULL;

		swap = (long long *) malloc(nmemb * size);

		if (swap == NULL)
		{
			return quadsort64(array, nmemb, size, ignore);
		}

		while (moduler > 4294967296ULL && nmemb / buckets > 4)
		{
			buckets *= 2;
			moduler /= 2;
		}

		bsize = nmemb / (buckets / 16);

		count = (unsigned int *) calloc(sizeof(int), buckets);
		stack = (unsigned int *) malloc(sizeof(int) * buckets);

		pta = (long long *) array;

		for (cnt = nmemb ; cnt ; cnt--)
		{
			index = (unsigned long long) *pta++ / moduler;

			if (++count[index] == bsize)
			{
				quadsort_swap64(array, swap, nmemb, size, ignore);

				free(swap);
				free(stack);
				free(count);
				return;
			}
		}

		cnt = 0;

		for (index = 0 ; index < buckets ; index++)
		{
			stack[index] = cnt;

			cnt += count[index];
		}

		pta = (long long *) array;

		for (cnt = nmemb ; cnt ; cnt--)
		{
			index = (unsigned long long) *pta / moduler;

			swap[stack[index]++] = *pta++;
		}

		pta = (long long *) array;
		pts = (long long *) swap;

		for (index = 0 ; index < buckets ; index++)
		{
			bsize = count[index];

			if (bsize)
			{
				if (bsize <= 16)
				{
					tail_swap64_cpy(pta, pts, bsize, ignore);
				}
				else
				{
					memcpy(pta, pts, bsize * size);

					quadsort_swap64(pta, pts, bsize, size, ignore);
				}
				pta += bsize;
				pts += bsize;
			}

		}
		free(count);
		free(swap);
		free(stack);
		return;
	}

//	assert(size == sizeof(int) || size == sizeof(long long));

	quadsort(array, nmemb, size, ignore);
}
