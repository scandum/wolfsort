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
	quadsort 1.1.3.1
*/

#ifndef QUADSORT_CPY_H
#define QUADSORT_CPY_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

typedef int CMPFUNC (const void *a, const void *b);

#define cmp(a,b) (*(a) > *(b))


#define tail_swap_two_cpy(dest, array, swap) \
{ \
	if (cmp(array, array + 1) > 0) \
	{ \
		dest[0] = array[1]; dest[1] = array[0]; \
	} \
	else \
	{ \
		dest[0] = array[0]; dest[1] = array[1]; \
	} \
}

#define tail_swap_three_cpy(dest, array, swap) \
{ \
	if (cmp(array, array + 1) > 0) \
	{ \
		if (cmp(array, array + 2) <= 0) \
		{ \
			dest[0] = array[1]; dest[1] = array[0]; dest[2] = array[2]; \
		} \
		else if (cmp(array + 1, array + 2) > 0) \
		{ \
			dest[0] = array[2]; dest[1] = array[1]; dest[2] = array[0]; \
		} \
		else \
		{ \
			dest[0] = array[1]; dest[1] = array[2]; dest[2] = array[0]; \
		} \
	} \
	else if (cmp(array + 1, array + 2) > 0) \
	{ \
		if (cmp(array, array + 2) > 0) \
		{ \
			dest[0] = array[2]; dest[1] = array[0]; dest[2] = array[1]; \
		} \
		else  \
		{ \
			dest[0] = array[0]; dest[1] = array[2]; dest[2] = array[1]; \
		} \
	} \
	else \
	{ \
			dest[0] = array[0]; dest[1] = array[1]; dest[2] = array[2]; \
	} \
}

#define tail_swap_four_cpy(dest, array, swap) \
{ \
	if (cmp(array, array + 1) > 0) \
	{ \
		if (cmp(array, array + 2) <= 0) \
		{ \
			dest[0] = array[1]; dest[1] = array[0]; dest[2] = array[2]; \
		} \
		else if (cmp(array + 1, array + 2) > 0) \
		{ \
			dest[0] = array[2]; dest[1] = array[1]; dest[2] = array[0]; \
		} \
		else \
		{ \
			dest[0] = array[1]; dest[1] = array[2]; dest[2] = array[0]; \
		} \
	} \
	else if (cmp(array + 1, array + 2) > 0) \
	{ \
		if (cmp(array, array + 2) > 0) \
		{ \
			dest[0] = array[2]; dest[1] = array[0]; dest[2] = array[1]; \
		} \
		else  \
		{ \
			dest[0] = array[0]; dest[1] = array[2]; dest[2] = array[1]; \
		} \
	} \
	else \
	{ \
			dest[0] = array[0]; dest[1] = array[1]; dest[2] = array[2]; \
	} \
 \
	if (cmp(dest + 1, array + 3) > 0) \
	{ \
		if (cmp(dest, array + 3) > 0) \
		{ \
			dest[3] = dest[2]; dest[2] = dest[1]; dest[1] = dest[0]; dest[0] = array[3]; \
		} \
		else \
		{ \
			dest[3] = dest[2]; dest[2] = dest[1]; dest[1] = array[3]; \
		} \
	} \
	else if (cmp(dest + 2, array + 3) > 0) \
	{ \
		dest[3] = dest[2]; dest[2] = array[3]; \
	} \
	else \
	{ \
		dest[3] = array[3]; \
	} \
}

///////////////////////////////////////////////////////
// ┌────────────────────────────────────────────────┐//
// │   ██████┐ ██████┐    ██████┐ ██████┐████████┐  │//
// │   └────██┐└────██┐   ██┌──██┐└─██┌─┘└──██┌──┘  │//
// │    █████┌┘ █████┌┘   ██████┌┘  ██│     ██│     │//
// │    └───██┐██┌───┘    ██┌──██┐  ██│     ██│     │//
// │   ██████┌┘███████┐   ██████┌┘██████┐   ██│     │//
// │   └─────┘ └──────┘   └─────┘ └─────┘   └─┘     │//
// └────────────────────────────────────────────────┘//
///////////////////////////////////////////////////////

void guarded_insert32_cpy(int *array, int key, unsigned char nmemb, CMPFUNC *ignore)
{
	int *pta, *tpa;
	unsigned char top;

	pta = array + nmemb;
	tpa = pta--;

	while (nmemb--)
	{
		if (cmp(pta, &key) <= 0)
		{
			break;
		}
		*tpa-- = *pta--;
	}
	*tpa = key;
}


void tail_swap32_cpy(int *dest, int *array, unsigned char nmemb, CMPFUNC *ignore)
{
	int swap;
	register unsigned char cnt;

	switch (nmemb)
	{
		case 0:
			return;
		case 1:
			*dest = *array;
			return;
		case 2:
			tail_swap_two_cpy(dest, array, swap);
			return;
		case 3:
			tail_swap_three_cpy(dest, array, swap);
			return;
		case 4:
			tail_swap_four_cpy(dest, array, swap);
			return;
	}
	tail_swap_four_cpy(dest, array, swap);

	for (cnt = 4 ; cnt < nmemb ; cnt++)
	{
		guarded_insert32_cpy(dest, array[cnt], cnt, ignore);
	}
	return;
}

void tail_merge32_cpy(int *dest, int *array, size_t nmemb, size_t block, CMPFUNC *ignore)
{
	register int *pta, *ptb, *ptd, *ptm;

	ptd = dest;

	pta = array;
	ptb = array + block;
	ptm = array + nmemb;

	if (cmp(ptb - 1, ptb) <= 0)
	{
		while (pta + 8 < ptm)
		{
			*ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++;
			*ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++;
		}
		while (pta < ptm)
		{
			*ptd++ = *pta++;
		}
		return;
	}

	if (cmp(ptb - 1, ptm - 1) <= 0)
	{
		ptm = array + block;

		while (pta < ptm)
		{
			while (cmp(pta, ptb) > 0)
			{
				*ptd++ = *ptb++;
			}
			*ptd++ = *pta++;
		}
		ptm = array + nmemb;

		while (ptb < ptm)
		{
			*ptd++ = *ptb++;
		}
	}
	else
	{
		while (ptb < ptm)
		{
			while (cmp(pta, ptb) <= 0)
			{
				*ptd++ = *pta++;
			}
			*ptd++ = *ptb++;
		}
		ptm = array + block;

		while (pta < ptm)
		{
			*ptd++ = *pta++;
		}
	}
}

///////////////////////////////////////////////////////
// ┌────────────────────────────────────────────────┐//
// │    █████┐ ██┐  ██┐   ██████┐ ██████┐████████┐  │//
// │   ██┌───┘ ██│  ██│   ██┌──██┐└─██┌─┘└──██┌──┘  │//
// │   ██████┐ ███████│   ██████┌┘  ██│     ██│     │//
// │   ██┌──██┐└────██│   ██┌──██┐  ██│     ██│     │//
// │   └█████┌┘     ██│   ██████┌┘██████┐   ██│     │//
// │    └────┘      └─┘   └─────┘ └─────┘   └─┘     │//
// └────────────────────────────────────────────────┘//
///////////////////////////////////////////////////////

void guarded_insert64_cpy(long long *array, long long key, unsigned char nmemb, CMPFUNC *ignore)
{
	long long *pta, *tpa;
	unsigned char top;

	pta = array + nmemb;
	tpa = pta--;

	while (nmemb--)
	{
		if (cmp(pta, &key) <= 0)
		{
			break;
		}
		*tpa-- = *pta--;
	}
	*tpa = key;
}


void tail_swap64_cpy(long long *dest, long long *array, unsigned char nmemb, CMPFUNC *ignore)
{
	long long swap;
	register unsigned char cnt;

	switch (nmemb)
	{
		case 0:
			return;
		case 1:
			*dest = *array;
			return;
		case 2:
			tail_swap_two_cpy(dest, array, swap);
			return;
		case 3:
			tail_swap_three_cpy(dest, array, swap);
			return;
		case 4:
			tail_swap_four_cpy(dest, array, swap);
			return;
	}
	tail_swap_four_cpy(dest, array, swap);

	for (cnt = 4 ; cnt < nmemb ; cnt++)
	{
		guarded_insert64_cpy(dest, array[cnt], cnt, ignore);
	}
	return;
}

void tail_merge64_cpy(long long *dest, long long *array, size_t nmemb, size_t block, CMPFUNC *ignore)
{
	register long long *pta, *ptb, *ptd, *ptm;

	ptd = dest;

	pta = array;
	ptb = array + block;
	ptm = array + nmemb;

	if (cmp(ptb - 1, ptb) <= 0)
	{
		while (pta + 8 < ptm)
		{
			*ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++;
			*ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++;
		}
		while (pta < ptm)
		{
			*ptd++ = *pta++;
		}
		return;
	}

	if (cmp(ptb - 1, ptm - 1) <= 0)
	{
		ptm = array + block;

		while (pta < ptm)
		{
			while (cmp(pta, ptb) > 0)
			{
				*ptd++ = *ptb++;
			}
			*ptd++ = *pta++;
		}
		ptm = array + nmemb;

		while (ptb < ptm)
		{
			*ptd++ = *ptb++;
		}
	}
	else
	{
		while (ptb < ptm)
		{
			while (cmp(pta, ptb) <= 0)
			{
				*ptd++ = *pta++;
			}
			*ptd++ = *ptb++;
		}
		ptm = array + block;

		while (pta < ptm)
		{
			*ptd++ = *pta++;
		}
	}
}

#endif
