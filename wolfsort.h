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
	wolfsort 1.1
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

size_t wolf_quad_swap32(int *swap, int *array, size_t nmemb, CMPFUNC *ignore)
{
	size_t offset;
	register int *pta, *pts, *ptt, *pte, tmp;

	pta = array;
	pts = swap;
	pte = pts + nmemb - 4;

	offset = 0;

	while (pts <= pte)
	{
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
					tmp = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
				else
				{
					tmp = pta[1]; pta[1] = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
				}
			}
			else if (cmp(&pta[0], &pta[3]) > 0)
			{
				tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp; tmp = pta[1]; pta[1] = pta[3]; pta[3] = tmp;
			}
			else if (cmp(&pta[1], &pta[3]) <= 0)
			{
				tmp = pta[0]; pta[0] = pta[2]; pta[2] = pta[1]; pta[1] = tmp;
			}
			else
			{
				tmp = pta[0]; pta[0] = pta[2]; pta[2] = pta[3]; pta[3] = pta[1]; pta[1] = tmp;
			}
		}
		pts += 4;
		pta += 4;
	}

	switch (nmemb % 4)
	{
		case 0:
			break;
		case 1:
			pta[0] = pts[0];
			break;

		case 2:
			if (cmp(&pts[0], &pts[1]) > 0)
			{
				pta[0] = pts[1]; pta[1] = pts[0];
			}
			else
			{
				pta[0] = pts[0]; pta[1] = pts[1];
			}
			break;

		case 3:
			if (cmp(&pts[0], &pts[1]) > 0)
			{
				if (cmp(&pts[1], &pts[2]) > 0)
				{
					pta[0] = pts[2]; pta[1] = pts[1]; pta[2] = pts[0];
				}
				else if (cmp(&pts[0], &pts[2]) > 0)
				{
					pta[0] = pts[1]; pta[1] = pts[2]; pta[2] = pts[0];
				}
				else
				{
					pta[0] = pts[1]; pta[1] = pts[0]; pta[2] = pts[2];
				}
			}
			else if (cmp(&pts[1], &pts[2]) > 0)
			{
				if (cmp(&pts[0], &pts[2]) > 0)
				{
					pta[0] = pts[2]; pta[1] = pts[0]; pta[2] = pts[1];
				}
				else
				{
					pta[0] = pts[0]; pta[1] = pts[2]; pta[2] = pts[1];
				}
			}
			else
			{
				pta[0] = pts[0]; pta[1] = pts[1]; pta[2] = pts[2];
			}
			break;
	}

	pta = pte = array + offset;

	for (tmp = (nmemb - offset) / 16 ; tmp > 0 ; --tmp, pte += 16)
	{
		if (cmp(&pta[3], &pta[4]) <= 0)
		{
			if (cmp(&pta[11], &pta[12]) <= 0)
			{
				if (cmp(&pta[7], &pta[8]) <= 0)
				{
					pta += 16;
					continue;
				}
				pts = swap;

				*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
				*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
				*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
				*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;

				goto step3;
			}
			pts = swap;

			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;

			goto step2;
		}

		// step1:

		pts = swap;

		if (cmp(&pta[3], &pta[7]) <= 0)
		{
			ptt = pta + 4;

			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;

			while (pta < pte + 4)
			{
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			}
			while (ptt < pte + 8)
			{
				*pts++ = *ptt++;
			}
			pta = ptt;
		}
		else if (cmp(&pta[0], &pta[7]) > 0)
		{
			ptt = pta + 4;
			*pts++ = *ptt++; *pts++ = *ptt++; *pts++ = *ptt++; *pts++ = *ptt++;
			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
			pta = ptt;
		}
		else
		{
			ptt = pta + 4;

			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;

			while (ptt < pte + 8)
			{
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			}

			while (pta < pte + 4)
			{
				*pts++ = *pta++;
			}

			pta = ptt;
		}

		if (cmp(&pta[3], &pta[4]) <= 0)
		{
			ptt = pta;
			pts = swap;
			pta = pte;

			if (cmp(&pts[7], &ptt[0]) <= 0)
			{
				*pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++;
				*pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++;

				pta += 8;
			}
			else if (cmp(&pts[7], &ptt[7]) <= 0)
			{
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

				while (pts < swap + 8)
				{
					*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				}

				pta = pte + 16;
			}
			else if (cmp(&pts[0], &ptt[7]) > 0)
			{
				*pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++;
				*pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++;
			}
			else
			{
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

				while (ptt < pte + 16)
				{
					*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
				}

				while (pts < swap + 8)
				{
					*pta++ = *pts++;
				}
			}
			continue;
		}

		step2:

		if (cmp(&pta[3], &pta[7]) <= 0)
		{
			ptt = pta + 4;

			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;

			while (pta < pte + 12)
			{
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			}
			while (pts < swap + 16)
			{
				*pts++ = *ptt++;
			}
		}
		else if (cmp(&pta[0], &pta[7]) > 0)
		{
			ptt = pta + 4;

			*pts++ = *ptt++; *pts++ = *ptt++; *pts++ = *ptt++; *pts++ = *ptt++;
			*pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++; *pts++ = *pta++;
		}
		else
		{
			ptt = pta + 4;

			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;

			while (ptt < pte + 16)
			{
				*pts++ = cmp(pta, ptt) > 0 ? *ptt++ : *pta++;
			}
			while (pts < swap + 16)
			{
				*pts++ = *pta++;
			}
		}

		step3:

		pta = pte;
		pts = swap;

		if (cmp(&pts[7], &pts[15]) <= 0)
		{
			ptt = pts + 8;

			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

//			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

			while (pts < swap + 8)
			{
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			}
			while (ptt < swap + 16)
			{
				*pta++ = *ptt++;
			}
		}
		else if (cmp(&pts[0], &pts[15]) > 0)
		{
			ptt = pts + 8;

			*pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++;
			*pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++; *pta++ = *ptt++;

			*pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++;
			*pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++; *pta++ = *pts++;
		}
		else
		{
			ptt = pts + 8;

			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

			*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;

			while (ptt < swap + 16)
			{
				*pta++ = cmp(pts, ptt) > 0 ? *ptt++ : *pts++;
			}
			while (pts < swap + 8)
			{
				*pta++ = *pts++;
			}
		}
	}

	if (nmemb % 16 > 4)
	{
		tail_merge32(pta, swap, nmemb % 16, 4, NULL);
	}
	return 0;
}

void cub_sort32(int *array, int *dest, size_t nmemb, CMPFUNC *ignore)
{
	register int tmp, *ptd = dest, *pta = array;
	register unsigned char i, mid, cnt, max;

	max = (unsigned char) nmemb;

	switch (max)
	{
		case 0:
			return;
		case 1:
			*dest = *array;
			return;
		case 2:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				ptd[0] = pta[1]; ptd[1] = pta[0];
			}
			else
			{
				ptd[0] = pta[0]; ptd[1] = pta[1];
			}
			if (cmp(&pta[2], &pta[3]) > 0)
			{
				ptd[2] = pta[3]; ptd[3] = pta[2];
			}
			else
			{
				ptd[2] = pta[2]; ptd[3] = pta[3];
			}
			return;

		case 3:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				if (cmp(&pta[1], &pta[2]) > 0)
				{
					ptd[0] = pta[2]; ptd[1] = pta[1]; ptd[2] = pta[0];
				}
				else if (cmp(&pta[0], &pta[2]) > 0)
				{
					ptd[0] = pta[1]; ptd[1] = pta[2]; ptd[2] = pta[0];
				}
				else
				{
					ptd[0] = pta[1]; ptd[1] = pta[0]; ptd[2] = pta[2];
				}
			}
			else if (cmp(&pta[1], &pta[2]) > 0)
			{
				if (cmp(&pta[0], &pta[2]) > 0)
				{
					ptd[0] = pta[2]; ptd[1] = pta[0]; ptd[2] = pta[1];
				}
				else
				{
					ptd[0] = pta[0]; ptd[1] = pta[2]; ptd[2] = pta[1];
				}
			}
			else
			{
				ptd[0] = pta[0]; ptd[1] = pta[1]; ptd[2] = pta[2];
			}
			return;

		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				ptd[0] = pta[1]; ptd[1] = pta[0];
			}
			else
			{
				ptd[0] = pta[0]; ptd[1] = pta[1];
			}

			if (cmp(&pta[2], &pta[3]) > 0)
			{
				ptd[2] = pta[3]; ptd[3] = pta[2];
			}
			else
			{
				ptd[2] = pta[2]; ptd[3] = pta[3];
			}

			if (cmp(&ptd[1], &ptd[2]) > 0)
			{
				if (cmp(&ptd[0], &ptd[2]) <= 0)
				{
					if (cmp(&ptd[1], &ptd[3]) <= 0)
					{
						tmp = ptd[1]; ptd[1] = ptd[2]; ptd[2] = tmp;
					}
					else
					{
						tmp = ptd[1]; ptd[1] = ptd[2]; ptd[2] = ptd[3]; ptd[3] = tmp;
					}
				}
				else if (cmp(&ptd[0], &ptd[3]) > 0)
				{
					tmp = ptd[0]; ptd[0] = ptd[2]; ptd[2] = tmp; tmp = ptd[1]; ptd[1] = ptd[3]; ptd[3] = tmp;
				}
				else if (cmp(&ptd[1], &ptd[3]) <= 0)
				{
					tmp = ptd[0]; ptd[0] = ptd[2]; ptd[2] = ptd[1]; ptd[1] = tmp;
				}
				else
				{
					tmp = ptd[0]; ptd[0] = ptd[2]; ptd[2] = ptd[3]; ptd[3] = ptd[1]; ptd[1] = tmp;
				}
			}

			for (cnt = 4 ; cnt < max ; cnt++)
			{
				if (cmp(&ptd[cnt - 1], &pta[cnt]) > 0)
				{
					if (cmp(&ptd[0], &pta[cnt]) > 0)
					{
						for (mid = cnt ; mid > 0 ; mid--)
						{
							ptd[mid] = ptd[mid - 1];
						}
		
						ptd[0] = pta[cnt];
					}
					else
					{
						ptd[cnt] = ptd[cnt - 1];

						i = cnt - 2;

						while (cmp(&ptd[i], &pta[cnt]) > 0)
						{
							ptd[i + 1] = ptd[i];
							i--;
						}
						ptd[++i] = pta[cnt];
					}
				}
				else
				{
					ptd[cnt] = pta[cnt];
				}
			}
			return;
	}
}

void wolfsort(void *array, size_t nmemb, size_t size, CMPFUNC *ignore)
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
			int *swap = malloc(8 * size + nmemb * size / 2);

			if (quad_swap32(array, swap, nmemb, NULL) != nmemb)
			{
				tail_merge32(array, swap, nmemb, 16, NULL);
			}
			free(swap);
		}
		else if (nmemb < 512)
		{
			int *swap = malloc(8 * size + nmemb * size / 2);

			if (quad_swap32(array, swap, nmemb, NULL) != nmemb)
			{
				quad_merge32(array, swap, nmemb, 16, NULL);
			}
			free(swap);
		}
		else
		{
			int *swap, *pta, *pts, *stack;
			unsigned int index, cnt, max;

			unsigned int buckets = 256;
			unsigned int moduler = 16777216;

			while (moduler > 8192 && nmemb / buckets > 8)
			{
				buckets *= 2;
				moduler /= 2;
			}

			max = nmemb / (buckets / 8);

			stack = calloc(size, buckets);

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
						swap = realloc(swap, size * nmemb / 2);

						if (quad_swap32(array, swap, nmemb, NULL) == 0)
						{
							quad_merge32(array, swap, nmemb, 16, NULL);
						}

						free(swap);

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

						while (cnt--)
						{
							*pta++ = *pts++;
						}
					}
				}
				free(swap);

				swap = malloc(max * size);

				pta = array;
				pts = swap;

				for (index = 0 ; index < buckets ; index++)
				{
					cnt = stack[index];

					if (cnt)
					{
						if (cnt <= 16)
						{
							tail_swap32(pta, cnt, NULL);
						}
						else if (cnt < 128)
						{
							if (quad_swap32(pta, pts, cnt, NULL) == 0)
							{
								tail_merge32(pta, pts, cnt, 16, NULL);
							}
						}
						else
						{
							if (quad_swap32(pta, pts, cnt, NULL) == 0)
							{
								quad_merge32(pta, pts, cnt, 16, NULL);
							}
						}
						pta += cnt;
					}
				}
				free(swap);

				return;
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
					if (quad_swap32(array, swap, nmemb, NULL) == 0)
					{
						quad_merge32(array, swap, nmemb, 16, NULL);
					}
					free(swap);
				}
			}
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
			long long *swap = malloc(8 * size + nmemb * size / 2);

			if (quad_swap64(array, swap, nmemb, NULL) != nmemb)
			{
				tail_merge64(array, swap, nmemb, 16, NULL);
			}
			free(swap);
		}
		else if (nmemb < 512)
		{
			long long *swap = malloc(8 * size + nmemb * size / 2);

			if (quad_swap64(array, swap, nmemb, NULL) != nmemb)
			{
				quad_merge64(array, swap, nmemb, 16, NULL);
			}
			free(swap);
		}
		else
		{
			long long *swap, *pta, *pts;
			unsigned int index, cnt, max, *stack;

			unsigned int buckets = 256;
			unsigned long long moduler = 72057594037927936ULL;

			while (moduler > 4294967296ULL && nmemb / buckets > 8)
			{
				buckets *= 2;
				moduler /= 2;
			}

			max = nmemb / (buckets / 8);

			stack = calloc(size, buckets);

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
						swap = realloc(swap, size * nmemb / 2);

						if (quad_swap64(array, swap, nmemb, NULL) == 0)
						{
							quad_merge64(array, swap, nmemb, 16, NULL);
						}

						free(swap);

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

						while (cnt--)
						{
							*pta++ = *pts++;
						}
					}
				}
				free(swap);

				swap = malloc(max * size);

				pta = array;
				pts = swap;

				for (index = 0 ; index < buckets ; index++)
				{
					cnt = stack[index];

					if (cnt)
					{
						if (cnt <= 16)
						{
							tail_swap64(pta, cnt, NULL);
						}
						else if (cnt < 128)
						{
							if (quad_swap64(pta, pts, cnt, NULL) == 0)
							{
								tail_merge64(pta, pts, cnt, 16, NULL);
							}
						}
						else
						{
							if (quad_swap64(pta, pts, cnt, NULL) == 0)
							{
								quad_merge64(pta, pts, cnt, 16, NULL);
							}
						}
						pta += cnt;
					}
				}
				free(swap);

				return;
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
					if (quad_swap64(array, swap, nmemb, NULL) == 0)
					{
						quad_merge64(array, swap, nmemb, 16, NULL);
					}
					free(swap);
				}
			}
			return;
		}
	}
	else
	{
		assert(size == sizeof(int) || size == sizeof(long long));
	}
}
