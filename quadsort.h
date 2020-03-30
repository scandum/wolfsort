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
	quadsort 1.1.2.4
*/

#ifndef QUADSORT_H
#define QUADSORT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>
#include <errno.h>

#define cmp(a,b) (*(a) > *(b))

typedef int CMPFUNC (const void *a, const void *b);

// 32 bit handling


void tail_swap32(int *array, size_t nmemb, CMPFUNC *ignore)
{
	int tmp, pts[8];
	register int *pta = array;
	register unsigned char i, mid, cnt, max;

	max = (unsigned char) nmemb;

	switch (max)
	{
		case 0:
		case 1:
			return;

		case 2:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}
			return;

		case 3:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				if (cmp(&pta[1], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
				}
				else if (cmp(&pta[0], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
				else
				{
					tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
				}
			}
			else if (cmp(&pta[1], &pta[2]) > 0)
			{
				if (cmp(&pta[0], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = pta[1]; pta[1] = tmp;
				}
				else
				{
					tmp = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
			}
			return;

		case 4:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}

			if (cmp(&pta[2], &pta[3]) > 0)
			{
				tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
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
			return;

		case 5:
		case 6:
		case 7:
		case 8:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts[0] = pta[1]; pts[1] = pta[0];
			}
			else
			{
				pts[0] = pta[0]; pts[1] = pta[1];
			}

			if (cmp(&pta[2], &pta[3]) > 0)
			{
				pts[2] = pta[3]; pts[3] = pta[2];
			}
			else
			{
				pts[2] = pta[2]; pts[3] = pta[3];
			}

			if (cmp(&pts[1], &pts[2]) > 0)
			{
				if (cmp(&pts[0], &pts[2]) <= 0)
				{
					if (cmp(&pts[1], &pts[3]) <= 0)
					{
						tmp = pts[1]; pts[1] = pts[2]; pts[2] = tmp;
					}
					else
					{
						tmp = pts[1]; pts[1] = pts[2]; pts[2] = pts[3]; pts[3] = tmp;
					}
				}
				else if (cmp(&pts[0], &pts[3]) > 0)
				{
					tmp = pts[0]; pts[0] = pts[2]; pts[2] = tmp; tmp = pts[1]; pts[1] = pts[3]; pts[3] = tmp;
				}
				else if (cmp(&pts[1], &pts[3]) <= 0)
				{
					tmp = pts[0]; pts[0] = pts[2]; pts[2] = pts[1]; pts[1] = tmp;
				}
				else
				{
					tmp = pts[0]; pts[0] = pts[2]; pts[2] = pts[3]; pts[3] = pts[1]; pts[1] = tmp;
				}
			}

			switch (max)
			{
				case 5:

					break;
				case 6:
					if (cmp(&pta[4], &pta[5]) > 0)
					{
						tmp = pta[4]; pta[4] = pta[5]; pta[5] = tmp;
					}
					break;

				case 7:
					if (cmp(&pta[4], &pta[5]) > 0)
					{
						if (cmp(&pta[5], &pta[6]) > 0)
						{
							tmp = pta[4]; pta[4] = pta[6]; pta[6] = tmp;
						}
						else if (cmp(&pta[4], &pta[6]) > 0)
						{
							tmp = pta[4]; pta[4] = pta[5]; pta[5] = pta[6]; pta[6] = tmp;
						}
						else
						{
							tmp = pta[4]; pta[4] = pta[5]; pta[5] = tmp;
						}
					}
					else if (cmp(&pta[5], &pta[6]) > 0)
					{
						if (cmp(&pta[4], &pta[6]) > 0)
						{
							tmp = pta[4]; pta[4] = pta[6]; pta[6] = pta[5]; pta[5] = tmp;
						}
						else
						{
							tmp = pta[5]; pta[5] = pta[6]; pta[6] = tmp;
						}
					}
					break;

				case 8:
					if (cmp(&pta[4], &pta[5]) > 0)
					{
						tmp = pta[4]; pta[4] = pta[5]; pta[5] = tmp;
					}

					if (cmp(&pta[6], &pta[7]) > 0)
					{
						tmp = pta[6]; pta[6] = pta[7]; pta[7] = tmp;
					}

					if (cmp(&pta[5], &pta[6]) > 0)
					{
						if (cmp(&pta[4], &pta[6]) <= 0)
						{
							if (cmp(&pta[5], &pta[7]) <= 0)
							{
								tmp = pta[5]; pta[5] = pta[6]; pta[6] = tmp;
							}
							else
							{
								tmp = pta[5]; pta[5] = pta[6]; pta[6] = pta[7]; pta[7] = tmp;
							}
						}
						else if (cmp(&pta[4], &pta[7]) > 0)
						{
							tmp = pta[4]; pta[4] = pta[6]; pta[6] = tmp; tmp = pta[5]; pta[5] = pta[7]; pta[7] = tmp;
						}
						else if (cmp(&pta[5], &pta[7]) <= 0)
						{
							tmp = pta[4]; pta[4] = pta[6]; pta[6] = pta[5]; pta[5] = tmp;
						}
						else
						{
							tmp = pta[4]; pta[4] = pta[6]; pta[6] = pta[7]; pta[7] = pta[5]; pta[5] = tmp;
						}
					}
					break;
			}

			cnt = i = 0;
			mid = 4;

			pta[cnt++] = cmp(&pts[i], &pta[mid]) > 0 ? pta[mid++] : pts[i++];

			while (i < 4 && mid < max)
			{
				if (cmp(&pts[i], &pta[mid]) > 0)
				{
					pta[cnt++] = pta[mid++];
				}
				else
				{
					pta[cnt++] = pts[i++];
				}
			}
			while (i < 4)
			{
				pta[cnt++] = pts[i++];
			}
			return;
	}

	// 9-15

	if (cmp(&pta[0], &pta[1]) > 0)
	{
		tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
	}

	if (cmp(&pta[2], &pta[3]) > 0)
	{
		tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
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

	if (cmp(&pta[4], &pta[5]) > 0)
	{
		tmp = pta[4]; pta[4] = pta[5]; pta[5] = tmp;
	}

	if (cmp(&pta[6], &pta[7]) > 0)
	{
		tmp = pta[6]; pta[6] = pta[7]; pta[7] = tmp;
	}

	if (cmp(&pta[5], &pta[6]) > 0)
	{
		if (cmp(&pta[4], &pta[6]) <= 0)
		{
			if (cmp(&pta[5], &pta[7]) <= 0)
			{
				tmp = pta[5]; pta[5] = pta[6]; pta[6] = tmp;
			}
			else
			{
				tmp = pta[5]; pta[5] = pta[6]; pta[6] = pta[7]; pta[7] = tmp;
			}
		}
		else if (cmp(&pta[4], &pta[7]) > 0)
		{
			tmp = pta[4]; pta[4] = pta[6]; pta[6] = tmp; tmp = pta[5]; pta[5] = pta[7]; pta[7] = tmp;
		}
		else if (cmp(&pta[5], &pta[7]) <= 0)
		{
			tmp = pta[4]; pta[4] = pta[6]; pta[6] = pta[5]; pta[5] = tmp;
		}
		else
		{
			tmp = pta[4]; pta[4] = pta[6]; pta[6] = pta[7]; pta[7] = pta[5]; pta[5] = tmp;
		}
	}


	pta += 8;

	switch (max)
	{
		case 9:
			break;

		case 10:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}
			break;

		case 11:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				if (cmp(&pta[1], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
				}
				else if (cmp(&pta[0], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
				else
				{
					tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
				}
			}
			else if (cmp(&pta[1], &pta[2]) > 0)
			{
				if (cmp(&pta[0], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = pta[1]; pta[1] = tmp;
				}
				else
				{
					tmp = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
			}
			break;

		default:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}

			if (cmp(&pta[2], &pta[3]) > 0)
			{
				tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
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

			for (cnt = 4 ; cnt + 8 < max ; cnt++)
			{
				if (cmp(&pta[cnt - 1], &pta[cnt]) > 0)
				{
					tmp = pta[cnt];

					if (cmp(&pta[0], &pta[cnt]) > 0)
					{
						for (mid = cnt ; mid > 0 ; mid--)
						{
							pta[mid] = pta[mid-1];
						}
						pta[0] = tmp;
					}
					else
					{
						pta[cnt] = pta[cnt - 1];

						i = cnt - 2;

						while (cmp(&pta[i], &tmp) > 0)
						{
							pta[i + 1] = pta[i];
							i--;
						}
						pta[++i] = tmp;
					}
				}
			}
	}
	pta -= 8;

	if (cmp(&pta[3], &pta[4]) <= 0)
	{
		if (cmp(&pta[7], &pta[8]) <= 0)
		{
			return;
		}
		for (i = 0 ; i < 8 ; i++)
		{
			pts[i] = pta[i];
		}
	}
	else if (cmp(&pta[0], &pta[7]) > 0)
	{
		pts[0] = pta[4];
		pts[1] = pta[5];
		pts[2] = pta[6];
		pts[3] = pta[7];

		pts[4] = pta[0];
		pts[5] = pta[1];
		pts[6] = pta[2];
		pts[7] = pta[3];
	}
	else
	{
		cnt = 0;
		i = 0;
		mid = 4;

		pts[cnt++] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];
		pts[cnt++] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];
		pts[cnt++] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];
		pts[cnt++] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];
		pts[cnt++] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];

		while (i < 4 && mid < 8)
		{
			if (cmp(&pta[i], &pta[mid]) > 0)
			{
				pts[cnt++] = pta[mid++];
			}
			else
			{
				pts[cnt++] = pta[i++];
			}
		}
		while (i < 4)
		{
			pts[cnt++] = pta[i++];
		}
		while (mid < 8)
		{
			pts[cnt++] = pta[mid++];
		}
	}

	cnt = 0;
	i = 0;
	mid = 8;

	pta[cnt++] = cmp(&pts[i], &pta[mid]) > 0 ? pta[mid++] : pts[i++];

	while (i < 8 && mid < max)
	{
		if (cmp(&pts[i], &pta[mid]) > 0)
		{
			pta[cnt++] = pta[mid++];
		}
		else
		{
			pta[cnt++] = pts[i++];
		}
	}
	while (i < 8)
	{
		pta[cnt++] = pts[i++];
	}
}

void tail_merge32(int *array, int *swap, size_t nmemb, size_t block, CMPFUNC *ignore);

size_t quad_swap32(int *array, size_t nmemb, CMPFUNC *ignore)
{
	int swap[16];
	size_t offset;
	register int *pta, *pts, *ptt, *pte, tmp;

	pta = array;
	pte = pta + nmemb - 4;

	offset = 0;

	while (pta <= pte)
	{
		if (cmp(&pta[0], &pta[1]) > 0)
		{
			if (cmp(&pta[2], &pta[3]) > 0)
			{
				if (cmp(&pta[1], &pta[2]) > 0)
				{
					pts = pta;
					pta += 4;
					goto swapper;
				}
				tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
			}
			tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
		}
		else if (cmp(&pta[2], &pta[3]) > 0)
		{
			tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
		}

		if (cmp(&pta[1], &pta[2]) > 0)
		{
			tmp = pta[1];

			if (cmp(&pta[0], &pta[2]) <= 0)
			{
				if (cmp(&pta[1], &pta[3]) <= 0)
				{
					pta[1] = pta[2]; pta[2] = tmp;
				}
				else
				{
					pta[1] = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
				}
			}
			else if (cmp(&pta[0], &pta[3]) > 0)
			{
				pta[1] = pta[3]; pta[3] = tmp; tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
			}
			else if (cmp(&pta[1], &pta[3]) <= 0)
			{
				pta[1] = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
			}
			else
			{
				pta[1] = pta[0]; pta[0] = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
			}
		}
		pta += 4;

		continue;

		swapper:

		if (pta <= pte)
		{
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				if (cmp(&pta[2], &pta[3]) > 0)
				{
					if (cmp(&pta[1], &pta[2]) > 0)
					{
						if (cmp(&pta[-1], &pta[0]) > 0)
						{
							pta += 4;

							goto swapper;
						}
					}
					tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
				}
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}
			else if (cmp(&pta[2], &pta[3]) > 0)
			{
				tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
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
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
					tmp = pta[1]; pta[1] = pta[3]; pta[3] = tmp;

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

			if (pts == array)
			{
				offset = (pta - pts) / 16 * 16;
			}
			ptt = pta - 1;
			pta += 4;
		}
		else
		{
			if (pts == array)
			{
				switch (nmemb % 4)
				{
					case 3:
						if (cmp(&pta[2], &pta[3]) <= 0)
						{
							offset = (pta - pts) / 16 * 16;
							break;
						}
					case 2:
						if (cmp(&pta[1], &pta[2]) <= 0)
						{
							offset = (pta - pts) / 16 * 16;
							break;
						}
					case 1:
						if (cmp(&pta[0], &pta[1]) <= 0)
						{
							offset = (pta - pts) / 16 * 16;
							break;
						}
					case 0:
						goto swapped;
				}
			}
			ptt = pta - 1;
		}

		while (pts < ptt)
		{
			tmp = *pts;
			*pts++ = *ptt;
			*ptt-- = tmp;
		}
		continue;

		swapped:

		ptt = pts + nmemb - 1;

		while (pts < ptt)
		{
			tmp = *pts;
			*pts++ = *ptt;
			*ptt-- = tmp;
		}
		return 1;
	}

	switch (nmemb % 4)
	{
		case 0:
		case 1:
			break;
		case 2:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}
			break;
		case 3:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				if (cmp(&pta[1], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
				}
				else if (cmp(&pta[0], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
				else
				{
					tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
				}
			}
			else if (cmp(&pta[1], &pta[2]) > 0)
			{
				if (cmp(&pta[0], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = pta[1]; pta[1] = tmp;
				}
				else
				{
					tmp = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
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

void quad_merge32(int *array, int *swap, const size_t nmemb, size_t block, CMPFUNC *ignore)
{
	size_t offset;
	register int *pta, *pts, *c, *c_max, *d, *d_max;

	while (block * 8 < nmemb)
	{
		offset = 0;

		while (offset + block * 4 <= nmemb)
		{
			pta = array + offset;

			c_max = pta + block;

			if (cmp(c_max - 1, c_max) <= 0)
			{
				c_max = pta + block * 3;

				if (cmp(c_max - 1, c_max) <= 0)
				{
					c_max = pta + block * 2;

					if (cmp(c_max - 1, c_max) <= 0)
					{
						offset += block * 4;
						continue;
					}
					pts = swap;

					c = pta;

					while (c < c_max - 8)
					{
						*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
					}
					while (c < c_max)
						*pts++ = *c++;

					c = c_max;
					c_max = c + block * 2;

					while (c < c_max - 8)
					{
						*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
					}
					while (c < c_max)
						*pts++ = *c++;

					goto step3;
				}
				pts = swap;

				c = pta;
				c_max = pta + block * 2;

				while (c < c_max)
					*pts++ = *c++;

				goto step2;
			}

			// step1:

			pts = swap;

			c = pta;

			d = c_max;
			d_max = d + block;

			if (cmp(c_max - 1, d_max - 1) <= 0)
			{
				while (c < c_max)
				{
					while (cmp(c, d) > 0)
					{
						*pts++ = *d++;
					}
					*pts++ = *c++;
				}
				while (d < d_max)
					*pts++ = *d++;
			}
			else
			{
				while (d < d_max)
				{
					while (cmp(c, d) <= 0)
					{
						*pts++ = *c++;
					}
					*pts++ = *d++;
				}

				while (c < c_max)
					*pts++ = *c++;
			}

			step2:

			c = pta + block * 2;

			c_max = c + block;
			d = c_max;
			d_max = d + block;

			if (cmp(c_max - 1, d_max - 1) <= 0)
			{
				while (c < c_max)
				{
					while (cmp(c, d) > 0)
					{
						*pts++ = *d++;
					}
					*pts++ = *c++;
				}

				while (d < d_max)
					*pts++ = *d++;
			}
			else
			{
				while (d < d_max)
				{
					while (cmp(c, d) <= 0)
					{
						*pts++ = *c++;
					}
					*pts++ = *d++;
				}

				while (c < c_max)
					*pts++ = *c++;
			}

			step3:

			pts = swap;

			c = pts;

			c_max = c + block * 2;

			d = c_max;
			d_max = d + block * 2;

			if (cmp(c_max - 1, d_max - 1) <= 0)
			{
				while (c < c_max)
				{
					while (cmp(c, d) > 0)
					{
						*pta++ = *d++;
					}
					*pta++ = *c++;
				}

				while (d < d_max)
					*pta++ = *d++;
			}
			else
			{
				while (d < d_max)
				{
					while (cmp(d, c) > 0)
					{
						*pta++ = *c++;
					}
					*pta++ = *d++;
				}

				while (c < c_max)
					*pta++ = *c++;
			}
			offset += block * 4;
		}
		tail_merge32(array + offset, swap, nmemb - offset, block, NULL);

		block *= 4;
	}
	tail_merge32(array, swap, nmemb, block, NULL);
}


void tail_merge32(int *array, int *swap, size_t nmemb, size_t block, CMPFUNC *ignore)
{
	size_t offset;
	register int *pta, *pts, *c, *c_max, *d, *d_max, *e;

	pts = swap;

	while (block < nmemb)
	{
		for (offset = 0 ; offset + block < nmemb ; offset += block * 2)
		{
			pta = array + offset;

			c_max = pta + block;

			if (cmp(c_max - 1, c_max) <= 0)
			{
				continue;
			}

			if (offset + block * 2 <= nmemb)
				d_max = pta + block * 2;
			else
				d_max = array + nmemb;

			if (cmp(pta, pta + block) <= 0)
			{
				if (offset + block * 2 <= nmemb)
					c_max = pts + block;
				else
					c_max = pts + nmemb - (offset + block);

				d = d_max - 1;
				e = pta + block - 1;

				while (cmp(e, d) <= 0)
				{
					d_max--;
					d--;
					c_max--;
				}

				c = c_max - 1;

				while (c >= pts + 8)
				{
					*c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--;
				}

				while (c >= pts)
				{
					*c-- = *d--;
				}

				c = c_max - 1;
				d = pta + block - 1;
				e = d_max - 1;

				*e-- = *d--;

				while (c >= pts)
				{
					while(cmp(d, c) > 0)
					{
						*e-- = *d--;
					}
					*e-- = *c--;
				}
			}
			else
			{
				if (offset + block * 2 <= nmemb)
					c_max = pts + block;
				else
					c_max = pts + nmemb - (offset + block);

				d = d_max - 1;
				e = pta + block - 1;

				while (cmp(e, d) <= 0)
				{
					d_max--;
					d--;
					c_max--;
				}

				c = c_max - 1;

				while (c >= pts + 8)
				{
					*c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--;
				}

				while (c >= pts)
				{
					*c-- = *d--;
				}

				c = c_max - 1;
				d = pta + block - 1;
				e = d_max - 1;

				*e-- = *d--;

				while (d >= pta)
				{
					while (cmp(d, c) <= 0)
					{
						*e-- = *c--;
					}
					*e-- = *d--;
				}
				while (c >= pts)
				{
					*e-- = *c--;
				}
			}
		}
		block *= 2;
	}
}

// 64 bit handling

void tail_swap64(long long *array, size_t nmemb, CMPFUNC *ignore)
{
	long long tmp, pts[8];
	register long long *pta = array;
	register unsigned char i, mid, cnt, max;

	max = (unsigned char) nmemb;

	switch (max)
	{
		case 0:
		case 1:
			return;

		case 2:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}
			return;

		case 3:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				if (cmp(&pta[1], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
				}
				else if (cmp(&pta[0], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
				else
				{
					tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
				}
			}
			else if (cmp(&pta[1], &pta[2]) > 0)
			{
				if (cmp(&pta[0], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = pta[1]; pta[1] = tmp;
				}
				else
				{
					tmp = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
			}
			return;

		case 4:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}

			if (cmp(&pta[2], &pta[3]) > 0)
			{
				tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
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
			return;

		case 5:
		case 6:
		case 7:
		case 8:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				pts[0] = pta[1]; pts[1] = pta[0];
			}
			else
			{
				pts[0] = pta[0]; pts[1] = pta[1];
			}

			if (cmp(&pta[2], &pta[3]) > 0)
			{
				pts[2] = pta[3]; pts[3] = pta[2];
			}
			else
			{
				pts[2] = pta[2]; pts[3] = pta[3];
			}

			if (cmp(&pts[1], &pts[2]) > 0)
			{
				if (cmp(&pts[0], &pts[2]) <= 0)
				{
					if (cmp(&pts[1], &pts[3]) <= 0)
					{
						tmp = pts[1]; pts[1] = pts[2]; pts[2] = tmp;
					}
					else
					{
						tmp = pts[1]; pts[1] = pts[2]; pts[2] = pts[3]; pts[3] = tmp;
					}
				}
				else if (cmp(&pts[0], &pts[3]) > 0)
				{
					tmp = pts[0]; pts[0] = pts[2]; pts[2] = tmp; tmp = pts[1]; pts[1] = pts[3]; pts[3] = tmp;
				}
				else if (cmp(&pts[1], &pts[3]) <= 0)
				{
					tmp = pts[0]; pts[0] = pts[2]; pts[2] = pts[1]; pts[1] = tmp;
				}
				else
				{
					tmp = pts[0]; pts[0] = pts[2]; pts[2] = pts[3]; pts[3] = pts[1]; pts[1] = tmp;
				}
			}

			switch (max)
			{
				case 5:

					break;
				case 6:
					if (cmp(&pta[4], &pta[5]) > 0)
					{
						tmp = pta[4]; pta[4] = pta[5]; pta[5] = tmp;
					}
					break;

				case 7:
					if (cmp(&pta[4], &pta[5]) > 0)
					{
						if (cmp(&pta[5], &pta[6]) > 0)
						{
							tmp = pta[4]; pta[4] = pta[6]; pta[6] = tmp;
						}
						else if (cmp(&pta[4], &pta[6]) > 0)
						{
							tmp = pta[4]; pta[4] = pta[5]; pta[5] = pta[6]; pta[6] = tmp;
						}
						else
						{
							tmp = pta[4]; pta[4] = pta[5]; pta[5] = tmp;
						}
					}
					else if (cmp(&pta[5], &pta[6]) > 0)
					{
						if (cmp(&pta[4], &pta[6]) > 0)
						{
							tmp = pta[4]; pta[4] = pta[6]; pta[6] = pta[5]; pta[5] = tmp;
						}
						else
						{
							tmp = pta[5]; pta[5] = pta[6]; pta[6] = tmp;
						}
					}
					break;

				case 8:
					if (cmp(&pta[4], &pta[5]) > 0)
					{
						tmp = pta[4]; pta[4] = pta[5]; pta[5] = tmp;
					}

					if (cmp(&pta[6], &pta[7]) > 0)
					{
						tmp = pta[6]; pta[6] = pta[7]; pta[7] = tmp;
					}

					if (cmp(&pta[5], &pta[6]) > 0)
					{
						if (cmp(&pta[4], &pta[6]) <= 0)
						{
							if (cmp(&pta[5], &pta[7]) <= 0)
							{
								tmp = pta[5]; pta[5] = pta[6]; pta[6] = tmp;
							}
							else
							{
								tmp = pta[5]; pta[5] = pta[6]; pta[6] = pta[7]; pta[7] = tmp;
							}
						}
						else if (cmp(&pta[4], &pta[7]) > 0)
						{
							tmp = pta[4]; pta[4] = pta[6]; pta[6] = tmp; tmp = pta[5]; pta[5] = pta[7]; pta[7] = tmp;
						}
						else if (cmp(&pta[5], &pta[7]) <= 0)
						{
							tmp = pta[4]; pta[4] = pta[6]; pta[6] = pta[5]; pta[5] = tmp;
						}
						else
						{
							tmp = pta[4]; pta[4] = pta[6]; pta[6] = pta[7]; pta[7] = pta[5]; pta[5] = tmp;
						}
					}
					break;
			}

			cnt = i = 0;
			mid = 4;

			pta[cnt++] = cmp(&pts[i], &pta[mid]) > 0 ? pta[mid++] : pts[i++];

			while (i < 4 && mid < max)
			{
				if (cmp(&pts[i], &pta[mid]) > 0)
				{
					pta[cnt++] = pta[mid++];
				}
				else
				{
					pta[cnt++] = pts[i++];
				}
			}
			while (i < 4)
			{
				pta[cnt++] = pts[i++];
			}
			return;
	}

	// 9-15

	if (cmp(&pta[0], &pta[1]) > 0)
	{
		tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
	}

	if (cmp(&pta[2], &pta[3]) > 0)
	{
		tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
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

	if (cmp(&pta[4], &pta[5]) > 0)
	{
		tmp = pta[4]; pta[4] = pta[5]; pta[5] = tmp;
	}

	if (cmp(&pta[6], &pta[7]) > 0)
	{
		tmp = pta[6]; pta[6] = pta[7]; pta[7] = tmp;
	}

	if (cmp(&pta[5], &pta[6]) > 0)
	{
		if (cmp(&pta[4], &pta[6]) <= 0)
		{
			if (cmp(&pta[5], &pta[7]) <= 0)
			{
				tmp = pta[5]; pta[5] = pta[6]; pta[6] = tmp;
			}
			else
			{
				tmp = pta[5]; pta[5] = pta[6]; pta[6] = pta[7]; pta[7] = tmp;
			}
		}
		else if (cmp(&pta[4], &pta[7]) > 0)
		{
			tmp = pta[4]; pta[4] = pta[6]; pta[6] = tmp; tmp = pta[5]; pta[5] = pta[7]; pta[7] = tmp;
		}
		else if (cmp(&pta[5], &pta[7]) <= 0)
		{
			tmp = pta[4]; pta[4] = pta[6]; pta[6] = pta[5]; pta[5] = tmp;
		}
		else
		{
			tmp = pta[4]; pta[4] = pta[6]; pta[6] = pta[7]; pta[7] = pta[5]; pta[5] = tmp;
		}
	}


	pta += 8;

	switch (max)
	{
		case 9:
			break;

		case 10:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}
			break;

		case 11:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				if (cmp(&pta[1], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
				}
				else if (cmp(&pta[0], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
				else
				{
					tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
				}
			}
			else if (cmp(&pta[1], &pta[2]) > 0)
			{
				if (cmp(&pta[0], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = pta[1]; pta[1] = tmp;
				}
				else
				{
					tmp = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
			}
			break;

		default:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}

			if (cmp(&pta[2], &pta[3]) > 0)
			{
				tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
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

			for (cnt = 4 ; cnt + 8 < max ; cnt++)
			{
				if (cmp(&pta[cnt - 1], &pta[cnt]) > 0)
				{
					tmp = pta[cnt];

					if (cmp(&pta[0], &pta[cnt]) > 0)
					{
						for (mid = cnt ; mid > 0 ; mid--)
						{
							pta[mid] = pta[mid-1];
						}
						pta[0] = tmp;
					}
					else
					{
						pta[cnt] = pta[cnt - 1];

						i = cnt - 2;

						while (cmp(&pta[i], &tmp) > 0)
						{
							pta[i + 1] = pta[i];
							i--;
						}
						pta[++i] = tmp;
					}
				}
			}
	}
	pta -= 8;

	if (cmp(&pta[3], &pta[4]) <= 0)
	{
		if (cmp(&pta[7], &pta[8]) <= 0)
		{
			return;
		}
		for (i = 0 ; i < 8 ; i++)
		{
			pts[i] = pta[i];
		}
	}
	else if (cmp(&pta[0], &pta[7]) > 0)
	{
		pts[0] = pta[4];
		pts[1] = pta[5];
		pts[2] = pta[6];
		pts[3] = pta[7];

		pts[4] = pta[0];
		pts[5] = pta[1];
		pts[6] = pta[2];
		pts[7] = pta[3];
	}
	else
	{
		cnt = 0;
		i = 0;
		mid = 4;

		pts[cnt++] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];
		pts[cnt++] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];
		pts[cnt++] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];
		pts[cnt++] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];
		pts[cnt++] = cmp(&pta[i], &pta[mid]) > 0 ? pta[mid++] : pta[i++];

		while (i < 4 && mid < 8)
		{
			if (cmp(&pta[i], &pta[mid]) > 0)
			{
				pts[cnt++] = pta[mid++];
			}
			else
			{
				pts[cnt++] = pta[i++];
			}
		}
		while (i < 4)
		{
			pts[cnt++] = pta[i++];
		}
		while (mid < 8)
		{
			pts[cnt++] = pta[mid++];
		}
	}

	cnt = 0;
	i = 0;
	mid = 8;

	pta[cnt++] = cmp(&pts[i], &pta[mid]) > 0 ? pta[mid++] : pts[i++];

	while (i < 8 && mid < max)
	{
		if (cmp(&pts[i], &pta[mid]) > 0)
		{
			pta[cnt++] = pta[mid++];
		}
		else
		{
			pta[cnt++] = pts[i++];
		}
	}
	while (i < 8)
	{
		pta[cnt++] = pts[i++];
	}
}

void tail_merge64(long long *array, long long *swap, const size_t nmemb, size_t block, CMPFUNC *ignore);

size_t quad_swap64(long long *array, size_t nmemb, CMPFUNC *ignore)
{
	long long swap[16];
	size_t offset;
	register long long *pta, *pts, *ptt, *pte, tmp;

	pta = array;
	pte = pta + nmemb - 4;

	offset = 0;

	while (pta <= pte)
	{
		if (cmp(&pta[0], &pta[1]) > 0)
		{
			if (cmp(&pta[2], &pta[3]) > 0)
			{
				if (cmp(&pta[1], &pta[2]) > 0)
				{
					pts = pta;
					pta += 4;
					goto swapper;
				}
				tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
			}
			tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
		}
		else if (cmp(&pta[2], &pta[3]) > 0)
		{
			tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
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
		pta += 4;

		continue;

		swapper:

		if (pta <= pte)
		{
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				if (cmp(&pta[2], &pta[3]) > 0)
				{
					if (cmp(&pta[1], &pta[2]) > 0)
					{
						if (cmp(&pta[-1], &pta[0]) > 0)
						{
							pta += 4;

							goto swapper;
						}
					}
					tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
				}
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}
			else if (cmp(&pta[2], &pta[3]) > 0)
			{
				tmp = pta[2]; pta[2] = pta[3]; pta[3] = tmp;
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
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
					tmp = pta[1]; pta[1] = pta[3]; pta[3] = tmp;

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

			if (pts == array)
			{
				offset = (pta - pts) / 16 * 16;
			}
			ptt = pta - 1;
			pta += 4;
		}
		else
		{
			if (pts == array)
			{
				switch (nmemb % 4)
				{
					case 3:
						if (cmp(&pta[2], &pta[3]) <= 0)
						{
							offset = (pta - pts) / 16 * 16;
							break;
						}
					case 2:
						if (cmp(&pta[1], &pta[2]) <= 0)
						{
							offset = (pta - pts) / 16 * 16;
							break;
						}
					case 1:
						if (cmp(&pta[0], &pta[1]) <= 0)
						{
							offset = (pta - pts) / 16 * 16;
							break;
						}
					case 0:
						goto swapped;
				}
			}
			ptt = pta - 1;
		}

		while (pts < ptt)
		{
			tmp = *pts;
			*pts++ = *ptt;
			*ptt-- = tmp;
		}
		continue;

		swapped:

		ptt = pts + nmemb - 1;

		while (pts < ptt)
		{
			tmp = *pts;
			*pts++ = *ptt;
			*ptt-- = tmp;
		}
		return 1;
	}

	switch (nmemb % 4)
	{
		case 0:
		case 1:
			break;
		case 2:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
			}
			break;
		case 3:
			if (cmp(&pta[0], &pta[1]) > 0)
			{
				if (cmp(&pta[1], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = tmp;
				}
				else if (cmp(&pta[0], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
				else
				{
					tmp = pta[0]; pta[0] = pta[1]; pta[1] = tmp;
				}
			}
			else if (cmp(&pta[1], &pta[2]) > 0)
			{
				if (cmp(&pta[0], &pta[2]) > 0)
				{
					tmp = pta[0]; pta[0] = pta[2]; pta[2] = pta[1]; pta[1] = tmp;
				}
				else
				{
					tmp = pta[1]; pta[1] = pta[2]; pta[2] = tmp;
				}
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
		tail_merge64(pta, swap, nmemb % 16, 4, NULL);
	}
	return 0;
}

void quad_merge64(long long *array, long long *swap, const size_t nmemb, size_t block, CMPFUNC *ignore)
{
	size_t offset;
	register long long *pta, *pts, *c, *c_max, *d, *d_max;

	while (block * 8 < nmemb)
	{
		offset = 0;

		while (offset + block * 4 <= nmemb)
		{
			pta = array + offset;

			c_max = pta + block;

			if (cmp(c_max - 1, c_max) <= 0)
			{
				c_max = pta + block * 3;

				if (cmp(c_max - 1, c_max) <= 0)
				{
					c_max = pta + block * 2;

					if (cmp(c_max - 1, c_max) <= 0)
					{
						offset += block * 4;
						continue;
					}
					pts = swap;

					c = pta;

					while (c < c_max - 8)
					{
						*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
					}
					while (c < c_max)
						*pts++ = *c++;

					c = c_max;
					c_max = c + block * 2;

					while (c < c_max - 8)
					{
						*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
						*pts++ = *c++; *pts++ = *c++; *pts++ = *c++; *pts++ = *c++;
					}
					while (c < c_max)
						*pts++ = *c++;

					goto step3;
				}
				pts = swap;

				c = pta;
				c_max = pta + block * 2;

				while (c < c_max)
					*pts++ = *c++;

				goto step2;
			}

			// step1:

			pts = swap;

			c = pta;

			d = c_max;
			d_max = d + block;

			if (cmp(c_max - 1, d_max - 1) <= 0)
			{
				while (c < c_max)
				{
					while (cmp(c, d) > 0)
					{
						*pts++ = *d++;
					}
					*pts++ = *c++;
				}
				while (d < d_max)
					*pts++ = *d++;
			}
			else
			{
				while (d < d_max)
				{
					while (cmp(c, d) <= 0)
					{
						*pts++ = *c++;
					}
					*pts++ = *d++;
				}

				while (c < c_max)
					*pts++ = *c++;
			}

			step2:

			c = pta + block * 2;

			c_max = c + block;
			d = c_max;
			d_max = d + block;

			if (cmp(c_max - 1, d_max - 1) <= 0)
			{
				while (c < c_max)
				{
					while (cmp(c, d) > 0)
					{
						*pts++ = *d++;
					}
					*pts++ = *c++;
				}

				while (d < d_max)
					*pts++ = *d++;
			}
			else
			{
				while (d < d_max)
				{
					while (cmp(c, d) <= 0)
					{
						*pts++ = *c++;
					}
					*pts++ = *d++;
				}

				while (c < c_max)
					*pts++ = *c++;
			}

			step3:

			pts = swap;

			c = pts;

			c_max = c + block * 2;

			d = c_max;
			d_max = d + block * 2;

			if (cmp(c_max - 1, d_max - 1) <= 0)
			{
				while (c < c_max)
				{
					while (cmp(c, d) > 0)
					{
						*pta++ = *d++;
					}
					*pta++ = *c++;
				}

				while (d < d_max)
					*pta++ = *d++;
			}
			else
			{
				while (d < d_max)
				{
					while (cmp(d, c) > 0)
					{
						*pta++ = *c++;
					}
					*pta++ = *d++;
				}

				while (c < c_max)
					*pta++ = *c++;
			}
			offset += block * 4;
		}
		tail_merge64(array + offset, swap, nmemb - offset, block, NULL);

		block *= 4;
	}
	tail_merge64(array, swap, nmemb, block, NULL);
}

void tail_merge64(long long *array, long long *swap, size_t nmemb, size_t block, CMPFUNC *ignore)
{
	size_t offset;
	register long long *pta, *pts, *c, *c_max, *d, *d_max, *e;

	pts = swap;

	while (block < nmemb)
	{
		for (offset = 0 ; offset + block < nmemb ; offset += block * 2)
		{
			pta = array + offset;

			c_max = pta + block;

			if (cmp(c_max - 1, c_max) <= 0)
			{
				continue;
			}

			if (offset + block * 2 <= nmemb)
				d_max = pta + block * 2;
			else
				d_max = array + nmemb;

			if (cmp(pta, pta + block) <= 0)
			{
				if (offset + block * 2 <= nmemb)
					c_max = pts + block;
				else
					c_max = pts + nmemb - (offset + block);

				d = d_max - 1;
				e = pta + block - 1;

				while (cmp(e, d) <= 0)
				{
					d_max--;
					d--;
					c_max--;
				}

				c = c_max - 1;

				while (c >= pts + 8)
				{
					*c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--;
				}

				while (c >= pts)
				{
					*c-- = *d--;
				}

				c = c_max - 1;
				d = pta + block - 1;
				e = d_max - 1;

				*e-- = *d--;

				while (c >= pts)
				{
					while(cmp(d, c) > 0)
					{
						*e-- = *d--;
					}
					*e-- = *c--;
				}
			}
			else
			{
				if (offset + block * 2 <= nmemb)
					c_max = pts + block;
				else
					c_max = pts + nmemb - (offset + block);

				d = d_max - 1;
				e = pta + block - 1;

				while (cmp(e, d) <= 0)
				{
					d_max--;
					d--;
					c_max--;
				}

				c = c_max - 1;

				while (c >= pts + 8)
				{
					*c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--; *c-- = *d--;
				}

				while (c >= pts)
				{
					*c-- = *d--;
				}

				c = c_max - 1;
				d = pta + block - 1;
				e = d_max - 1;

				*e-- = *d--;

				while (d >= pta)
				{
					while (cmp(d, c) <= 0)
					{
						*e-- = *c--;
					}
					*e-- = *d--;
				}
				while (c >= pts)
				{
					*e-- = *c--;
				}
			}
		}
		block *= 2;
	}
}

void quadsort(void *array, size_t nmemb, size_t size, CMPFUNC *ignore)
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
			if (quad_swap32(array, nmemb, NULL) == 0)
			{
				int swap[64];
				tail_merge32(array, swap, nmemb, 16, NULL);
			}
		}
		else
		{
			if (quad_swap32(array, nmemb, NULL) == 0)
			{
				int *swap = malloc(nmemb * size / 2);

				if (swap == NULL)
				{
					fprintf(stderr, "quadsort(%p,%zu,%zu,%p): malloc() failed: %s\n", array, nmemb, size, NULL, strerror(errno));

					return;
				}

				quad_merge32(array, swap, nmemb, 16, NULL);

				free(swap);
			}
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
			if (quad_swap64(array, nmemb, NULL) == 0)
			{
				long long swap[64];
				tail_merge64(array, swap, nmemb, 16, NULL);
			}
		}
		else
		{
			if (quad_swap64(array, nmemb, NULL) == 0)
			{
				long long *swap = malloc(nmemb * size / 2);

				if (swap == NULL)
				{
					fprintf(stderr, "quadsort(%p,%zu,%zu,%p): malloc() failed: %s\n", array, nmemb, size, NULL, strerror(errno));

					return;
				}
				quad_merge64(array, swap, nmemb, 16, NULL);

				free(swap);
			}
		}
	}
	else
	{
		assert(size == sizeof(int) || size == sizeof(long long));
	}
}

#endif
