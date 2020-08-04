/*
	Copyright (C) 2014-2020 Igor van den Hoven ivdhoven@gmail.com
*/

/*
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
	flowsort 1.1.2
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#ifndef QUADSORT_H
  #include "quadsort.h"
#endif

#define cmp(a,b) (*(a) > *(b))

typedef int CMPFUNC (const void *a, const void *b);

void flowsort(void *array, size_t nmemb, size_t size, CMPFUNC *ignore)
{
	if (nmemb < 2)
	{
		return;
	}

	if (size == sizeof(int))
	{
		if (nmemb <= 16)
		{
			tail_swap32(array, nmemb, NULL);
		}
		else if (nmemb < 128)
		{
			if (quad_swap32(array, nmemb, NULL) != nmemb)
			{
				int *swap = malloc(8 * size + nmemb * size / 2);

				tail_merge32(array, swap, nmemb, 16, NULL);

				free(swap);
			}
		}
		else if (nmemb < 768)
		{
			if (quad_swap32(array, nmemb, NULL) != nmemb)
			{
				int *swap = malloc(nmemb * size / 2);

				quad_merge32(array, swap, nmemb, 16, NULL);

				free(swap);
			}
		}
		else
		{
			int *swap, *pta, *pts;
			unsigned char *count;
			unsigned int *stack1, *stack2, index, cnt, max;

			unsigned int buckets = 512;
			unsigned int moduler = 8388608;

			while (moduler > 512 && nmemb / buckets > 4)
			{
				buckets *= 2;
				moduler /= 2;
			}

			count = calloc(sizeof(char), buckets);

			stack1 = malloc(sizeof(int) * buckets);
			stack2 = malloc(sizeof(int) * buckets / 512);

			swap = malloc(size * nmemb);

			if (swap)
			{
				pta = array;

				for (cnt = 0 ; cnt < nmemb ; cnt++)
				{
					index = (unsigned int) *pta++ / moduler;

					if (++count[index] == 255)
					{
						if (quad_swap32(array, nmemb, NULL) == 0)
						{
							quad_merge32(array, swap, nmemb, 16, NULL);
						}
						free(swap);
						free(stack1);
						free(stack2);
						free(count);

						return;
					}
				}

				max = 0;

				for (index = 0 ; index < buckets ; index++)
				{
					stack1[index] = max;

					if (index % 512 == 0)
					{
						stack2[index / 512] = max;
					}
					max += count[index];
				}

				pta = array;

				for (cnt = 0 ; cnt < nmemb ; cnt++)
				{
					index = (unsigned int) *pta / moduler / 512;

					swap[stack2[index]++] = *pta++;
				}

				pts = swap;
				pta = array;

				for (cnt = 0 ; cnt < nmemb ; cnt++)
				{
					index = (unsigned int) *pts / moduler;

					pta[stack1[index]++] = *pts++;
				}

				for (index = 0 ; index < buckets ; index++)
				{
					cnt = count[index];

					if (cnt)
					{
						if (cnt <= 16)
						{
							tail_swap32(pta, cnt, NULL);
						}
						else if (cnt < 128)
						{
							if (quad_swap32(pta, cnt, NULL) == 0)
							{
								tail_merge32(pta, swap, cnt, 16, NULL);
							}
						}
						else
						{
							if (quad_swap32(pta, cnt, NULL) == 0)
							{
								quad_merge32(pta, swap, cnt, 16, NULL);
							}
						}
						pta += cnt;
					}
				}
			}
			else
			{
				fprintf(stderr, "flowsort(%p,%zu,%zu): malloc() failed: %s\n", array, nmemb, size, strerror(errno));
			}
			free(swap);
			free(stack1);
			free(stack2);
			free(count);

			return;
		}
	}
	else if (size == sizeof(long long))
	{
		if (nmemb <= 16)
		{
			tail_swap64(array, nmemb, NULL);
		}
		else if (nmemb < 128)
		{
			if (quad_swap64(array, nmemb, NULL) != nmemb)
			{
				long long swap[64];
				tail_merge64(array, swap, nmemb, 16, NULL);
			}
		}
		else if (nmemb < 512)
		{
			if (quad_swap64(array, nmemb, NULL) != nmemb)
			{
				long long *swap = malloc(nmemb * size / 2);

				quad_merge64(array, swap, nmemb, 16, NULL);

				free(swap);
			}
		}
		else
		{
			long long *swap, *pta, *pts;
			unsigned int *stack1, *stack2, index, cnt, max;
			unsigned char *count;

			unsigned int buckets = 512;
			unsigned long long moduler = 72057594037927936ULL / 4ULL;

			while (moduler > 512 && nmemb / buckets > 4)
			{
				buckets *= 2;
				moduler /= 2;
			}

			count = calloc(sizeof(char), buckets);

			stack1 = malloc(sizeof(long long) * buckets);
			stack2 = malloc(sizeof(long long) * buckets / 512);

			swap = malloc(size * nmemb);

			if (swap)
			{
				pta = array;

				for (cnt = 0 ; cnt < nmemb ; cnt++)
				{
					index = (unsigned long long) *pta++ / moduler;

					if (++count[index] == 255)
					{
						if (quad_swap32(array, nmemb, NULL) == 0)
						{
							quad_merge64(array, swap, nmemb, 16, NULL);
						}
						free(swap);
						free(stack1);
						free(stack2);
						free(count);

						return;
					}
				}

				max = 0;

				for (index = 0 ; index < buckets ; index++)
				{
					stack1[index] = max;

					if (index % 512 == 0)
					{
						stack2[index / 512] = max;
					}
					max += count[index];
				}

				pta = array;

				for (cnt = 0 ; cnt < nmemb ; cnt++)
				{
					index = (unsigned long long) *pta / moduler / 512;

					swap[stack2[index]++] = *pta++;
				}

				pts = swap;
				pta = array;

				for (cnt = 0 ; cnt < nmemb ; cnt++)
				{
					index = (unsigned long long) *pts / moduler;

					pta[stack1[index]++] = *pts++;
				}

				for (index = 0 ; index < buckets ; index++)
				{
					cnt = count[index];

					if (cnt)
					{
						if (cnt <= 16)
						{
							tail_swap64(pta, cnt, NULL);
						}
						else if (cnt < 128)
						{
							if (quad_swap64(pta, cnt, NULL) == 0)
							{
								tail_merge64(pta, swap, cnt, 16, NULL);
							}
						}
						else
						{
							if (quad_swap64(pta, cnt, NULL) == 0)
							{
								quad_merge64(pta, swap, cnt, 16, NULL);
							}
						}
						pta += cnt;
					}
				}
			}
			else
			{
				fprintf(stderr, "flowsort(%p,%zu,%zu): malloc() failed: %s\n", array, nmemb, size, strerror(errno));
			}
			free(swap);
			free(stack1);
			free(stack2);
			free(count);

			return;
		}
	}
	else
	{
		assert(size == sizeof(int) || size == sizeof(long long));
	}
}
