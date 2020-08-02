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
	flowsort 1.1.1
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
		else if (nmemb < 512)
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
			unsigned int index, cnt, tmp, max, i, mid, *stack;

			unsigned int buckets = 256;
			unsigned int moduler = 16777216;

			while (moduler > 8096 && nmemb / buckets > 4)
			{
				buckets *= 2;
				moduler /= 2;
			}

			max = nmemb / (buckets / 8);

			stack = calloc(size, buckets);

			swap = malloc(nmemb * size);

			if (swap)
			{
				pta = array;

				for (cnt = 0 ; cnt < nmemb ; cnt++)
				{
					index = (unsigned int) *pta++ / moduler;

					if (++stack[index] == max)
					{
						if (quad_swap32(array, nmemb, NULL) == 0)
						{
							quad_merge32(array, swap, nmemb, 16, NULL);
						}
						free(swap);
						free(stack);
						
						return;
					}
				}

				cnt = mid = 0;

				for (index = 0 ; index < buckets ; index++)
				{
					mid += stack[index];

					stack[index] = mid - stack[index];
				}

				pta = array;

				for (cnt = 0 ; cnt < nmemb ; cnt++)
				{
					index = (unsigned int) *pta / moduler;

					swap[stack[index]++] = *pta++;
				}

				pta = array;
				max = 0;

				for (index = 0 ; index < buckets ; index++)
				{
					tmp = max;
					max = stack[index];
					cnt = stack[index] - tmp;

					if (cnt)
					{
						pts = swap + tmp;

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

								for (cnt = 4 ; cnt < stack[index] - tmp ; cnt++)
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

						cnt = stack[index] - tmp;

						pta -= cnt;
						pts -= cnt;

						if (cnt <= 16)
						{
							tail_swap32(pta, cnt, NULL);
						}
						else if (cnt < 128)
						{
							if (quad_swap32(pta, cnt, NULL) == 0)
							{
								tail_merge32(pta, pts, cnt, 16, NULL);
							}
						}
						else
						{
							if (quad_swap32(pta, cnt, NULL) == 0)
							{
								quad_merge32(pta, pts, cnt, 16, NULL);
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
					fprintf(stderr, "flowsort(%p,%zu,%zu): malloc() failed: %s\n", array, nmemb, size, strerror(errno));
				}
				else
				{
					if (quad_swap32(array, nmemb, NULL) == 0)
					{
						quad_merge32(array, swap, nmemb, 16, NULL);
					}
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
			unsigned int index, cnt, tmp, max, i, mid, *stack;

			unsigned int buckets = 256;
			unsigned long long moduler = 72057594037927936ULL;

			while (moduler > 4294967296ULL && nmemb / buckets > 6)
			{
				buckets *= 2;
				moduler /= 2;
			}

			max = nmemb / (buckets / 8);

			stack = calloc(size, buckets);

			swap = malloc(nmemb * size);

			if (swap)
			{
				pta = array;

				for (cnt = 0 ; cnt < nmemb ; cnt++)
				{
					index = (unsigned long long) *pta++ / moduler;

					if (++stack[index] == max)
					{
						if (quad_swap64(array, nmemb, NULL) == 0)
						{
							quad_merge64(array, swap, nmemb, 16, NULL);
						}
						free(swap);
						free(stack);
						
						return;
					}
				}

				mid = 0;

				for (index = 0 ; index < buckets ; index++)
				{
					mid += stack[index];

					stack[index] = mid - stack[index];
				}

				pta = array;

				for (cnt = 0 ; cnt < nmemb ; cnt++)
				{
					index = (unsigned long long) *pta / moduler;

					swap[stack[index]++] = *pta++;
				}

				pta = array;
				max = 0;

				for (index = 0 ; index < buckets ; index++)
				{
					tmp = max;
					max = stack[index];

					cnt = stack[index] - tmp;

					if (cnt)
					{
						pts = swap + tmp;

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

								for (cnt = 4 ; cnt < stack[index] - tmp ; cnt++)
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

						cnt = stack[index] - tmp;

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
					fprintf(stderr, "flowsort(%p,%zu,%zu): malloc() failed: %s\n", array, nmemb, size, strerror(errno));
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
