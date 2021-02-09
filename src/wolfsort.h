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
	wolfsort 1.1.1
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

#define cmp(a,b) (*(a) > *(b))

//typedef int CMPFUNC (const void *a, const void *b);

void wolfsort(void *array, size_t nmemb, size_t size, CMPFUNC *ignore)
{
	if (nmemb < 2)
	{
		return;
	}

	if (size == sizeof(int))
	{
		if (nmemb < 512)
		{
			quadsort32(array, nmemb, sizeof(int), NULL);
		}
		else
		{
			int *swap, *pta, *pts;
			unsigned int index, cnt, max, i, mid;
			unsigned short *stack;

			unsigned int buckets = 256;
			unsigned int moduler = 16777216;

			while (moduler > 8192 && nmemb / buckets > 4)
			{
				buckets *= 2;
				moduler /= 2;
			}

			max = nmemb / (buckets / 8);

			stack = (unsigned short *) calloc(sizeof(short), buckets);

			swap = malloc(max * buckets * size);

			if (swap)
			{
				pta = array;

				for (cnt = 0 ; cnt < nmemb ; cnt++)
				{
					index = (unsigned int) *pta / moduler;

					swap[index * max + stack[index]] = *pta++;

					if (++stack[index] == max)
					{
						free(swap);

						swap = malloc(size * nmemb / 2);

						quadsort_swap32(array, swap, nmemb, sizeof(int), NULL);

						free(swap);
						free(stack);
						return;
					}
				}

				pta = array;

				for (index = 0 ; index < buckets ; index++)
				{
					cnt = stack[index];

					if (cnt)
					{
						pts = swap + index * max;

						if (cnt <= 16)
						{
							tail_swap32_cpy(pta, pts, cnt, ignore);
							pta += cnt;
							continue;
						}

						while (cnt--)
						{
							*pta++ = *pts++;
						}
						
						cnt = stack[index];
						
						pta -= cnt;
						pts -= cnt;

						quadsort_swap32(pta, pts, cnt, sizeof(int), NULL);

						pta += cnt;
					}
				}
			}
			else
			{
				swap = malloc(size * nmemb / 2);

				if (swap == NULL)
				{
					fprintf(stderr, "wolfsort(%p,%zu,%zu): malloc() failed: %s\n", array, nmemb, size, strerror(errno));
				}
				else
				{
					quadsort_swap32(array, swap, nmemb, sizeof(int), NULL);
				}
			}
			free(swap);
			free(stack);
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
			unsigned int index, cnt, max, i, mid, *stack;

			unsigned int buckets = 256;
			unsigned long long moduler = 72057594037927936ULL;

			while (moduler > 4294967296ULL && nmemb / buckets > 6)
			{
				buckets *= 2;
				moduler /= 2;
			}

			max = nmemb / (buckets / 8);

			stack = (unsigned int *) calloc(size, buckets);

			swap = malloc(max * buckets * size);

			if (swap)
			{
				pta = array;

				for (cnt = 0 ; cnt < nmemb ; cnt++)
				{
					index = (unsigned long long) *pta / moduler;

					swap[index * max + stack[index]] = *pta++;

					if (++stack[index] == max)
					{
						free(swap);

						swap = malloc(size * nmemb / 2);

						if (quad_swap64(array, nmemb, NULL) == 0)
						{
							quad_merge64(array, swap, nmemb, 16, NULL);
						}

						free(swap);
						free(stack);
						return;
					}
				}

				pta = array;

				for (index = 0 ; index < buckets ; index++)
				{
					cnt = stack[index];

					if (cnt)
					{
						pts = swap + index * max;

						switch (cnt)
						{
							case 1:
								*pta++ = *pts;
								continue;

							case 2:
								if (cmp(&pts[0], &pts[1]) > 0)
								{
									*pta++ = pts[1];
									*pta++ = pts[0];
									continue;
								}
								*pta++ = pts[0];
								*pta++ = pts[1];
								continue;

							case 3:
								if (cmp(&pts[0], &pts[1]) > 0)
								{
									if (cmp(&pts[1], &pts[2]) > 0)
									{
										*pta++ = pts[2]; *pta++ = pts[1]; *pta++ = pts[0];
									}
									else if (cmp(&pts[0], &pts[2]) > 0)
									{
										*pta++ = pts[1]; *pta++ = pts[2]; *pta++ = pts[0];
									}
									else
									{
										*pta++ = pts[1]; *pta++ = pts[0]; *pta++ = pts[2];
									}
								}
								else if (cmp(&pts[1], &pts[2]) > 0)
								{
									if (cmp(&pts[0], &pts[2]) > 0)
									{
										*pta++ = pts[2]; *pta++ = pts[0]; *pta++ = pts[1];
									}
									else
									{
										*pta++ = pts[0]; *pta++ = pts[2]; *pta++ = pts[1];
									}
								}
								else
								{
									*pta++ = pts[0]; *pta++ = pts[1]; *pta++ = pts[2];
								}
								continue;

							case 4:
							case 5:
							case 6:
							case 7:
							case 8:
							case 9:
								if (cmp(&pts[0], &pts[1]) > 0)
								{
									pta[0] = pts[1]; pta[1] = pts[0];
								}
								else
								{
									pta[0] = pts[0]; pta[1] = pts[1];
								}

								if (cmp(&pts[2], &pts[3]) > 0)
								{
									pta[2] = pts[3]; pta[3] = pts[2];
								}
								else
								{
									pta[2] = pts[2]; pta[3] = pts[3];
								}

								if (cmp(&pta[1], &pta[2]) > 0)
								{
									if (cmp(&pta[0], &pta[2]) <= 0)
									{
										if (cmp(&pta[1], &pta[3]) <= 0)
										{
											*pts = pta[1]; pta[1] = pta[2]; pta[2] = *pts;
										}
										else
										{
											*pts = pta[1]; pta[1] = pta[2]; pta[2] = pta[3]; pta[3] = *pts;
										}
									}
									else if (cmp(&pta[0], &pta[3]) > 0)
									{
										*pts = pta[0]; pta[0] = pta[2]; pta[2] = *pts; *pts = pta[1]; pta[1] = pta[3]; pta[3] = *pts;
									}
									else if (cmp(&pta[1], &pta[3]) <= 0)
									{
										*pts = pta[0]; pta[0] = pta[2]; pta[2] = pta[1]; pta[1] = *pts;
									}
									else
									{
										*pts = pta[0]; pta[0] = pta[2]; pta[2] = pta[3]; pta[3] = pta[1]; pta[1] = *pts;
									}
								}

								for (cnt = 4 ; cnt < stack[index] ; cnt++)
								{
									if (cmp(&pta[cnt - 1], &pts[cnt]) > 0)
									{
										if (cmp(&pta[0], &pts[cnt]) > 0)
										{
											for (mid = cnt ; mid > 0 ; mid--)
											{
												pta[mid] = pta[mid - 1];
											}
							
											pta[0] = pts[cnt];
										}
										else
										{
											pta[cnt] = pta[cnt - 1];

											i = cnt - 2;

											while (cmp(&pta[i], &pts[cnt]) > 0)
											{
												pta[i + 1] = pta[i];
												i--;
											}
											pta[++i] = pts[cnt];
										}
									}
									else
									{
										pta[cnt] = pts[cnt];
									}
								}
								pta += cnt;
								continue;
						}

						while (cnt--)
						{
							*pta++ = *pts++;
						}
						
						cnt = stack[index];
						
						pta -= cnt;
						pts -= cnt;

						if (cnt <= 16)
						{
							tail_swap64(pta, cnt, NULL);
						}
						else if (cnt < 128)
						{
							if (quad_swap64(pta, cnt, NULL) == 0)
							{
								tail_merge64(pta, pts, cnt, 16, NULL);
							}
						}
						else
						{
							if (quad_swap64(pta, cnt, NULL) == 0)
							{
								quad_merge64(pta, pts, cnt, 16, NULL);
							}
						}
						pta += cnt;
					}
				}
			}
			else
			{
				swap = malloc(size * nmemb / 2);

				if (swap == NULL)
				{
					fprintf(stderr, "wolfsort(%p,%zu,%zu): malloc() failed: %s\n", array, nmemb, size, strerror(errno));
				}
				else
				{
					if (quad_swap64(array, nmemb, NULL) == 0)
					{
						quad_merge64(array, swap, nmemb, 16, NULL);
					}
				}
			}
			free(swap);
			free(stack);
			return;
		}
	}
	else
	{
		assert(size == sizeof(int) || size == sizeof(long long));
	}
}
