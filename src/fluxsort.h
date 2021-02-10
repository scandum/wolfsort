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

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
	fluxsort 1.1.3.3
*/

#ifndef FLUXSORT_H
#define FLUXSORT_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#define cmp(a,b) (*(a) > *(b))

typedef int CMPFUNC (const void *a, const void *b);

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

#define PIVOTS 256

size_t run, last;

size_t flux_binary_search32(int *array, int key, CMPFUNC *cmp)
{
	size_t mid, top;
	int *base = array;

	if (run == 0)
	{
		top = PIVOTS;
	}
	else
	{
		top = 1;
		base += last;

		if (cmp(base, &key) <= 0)
		{
			int *roof = array + PIVOTS;
	
			while (1)
			{
				if (base + top >= roof)
				{
					top = roof - base;
					break;
				}
	
				base += top;
	
				if (cmp(base, &key) > 0)
				{
					base -= top;
					break;
				}
				top *= 2;
			}
		}
		else
		{
			while (1)
			{
				if (base - top <= array)
				{
					top = base - array;
					base = array;
					break;
				}
	
				base -= top;
	
				if (cmp(&key, base) > 0)
				{
					break;
				}
				top *= 2;
			}
		}
	}

	while (top > 1)
	{
		mid = top / 2;

		if (cmp(base + mid, &key) <= 0)
		{
			base += mid;
		}
		top -= mid;
	}

	top = base - array;

	run = last == top;

	return last = top;
}


size_t flux_swap32(int *array, int *swap, unsigned char *hash, size_t nmemb, size_t stage, CMPFUNC *cmp)
{
	unsigned char *pth;
	size_t cnt, size[PIVOTS] = { 0 };
	int base[PIVOTS], *bptr[PIVOTS], *pta, *pte, *pts;

	pte = array + nmemb;

	for (cnt = 0 ; cnt < PIVOTS ; cnt++)
	{
		base[cnt] = array[cnt * nmemb / PIVOTS];
	}

	quadsort(base, PIVOTS, sizeof(int), cmp);

	run = last = 0;
	pth = hash;

	for (pta = array ; pta < pte ; pta++)
	{
		size[*pth++ = flux_binary_search32(base, *pta, cmp)]++;
	}

	pts = swap;

	for (cnt = 0 ; cnt < PIVOTS ; cnt++)
	{
		bptr[cnt] = pts;

		pts += size[cnt];
	}

	pth = hash;

	for (pta = array ; pta < pte ; pta++)
	{
		*bptr[*pth++]++ = *pta;
	}

	pta = array;
	pts = swap;
	pth = hash;

	for (cnt = 0 ; cnt < PIVOTS ; cnt++)
	{
		nmemb = size[cnt];

		if (stage == 0)
		{
			if (nmemb < 1024)
			{
				memcpy(pta, pts, nmemb * sizeof(int));
				quadsort_swap32(pta, pts, nmemb, sizeof(int), cmp);
			}
			else
			{
				flux_swap32(pts, pta, pth, nmemb, 1, cmp);
			}
		}
		else
		{
			quadsort_swap32(pts, pta, nmemb, sizeof(int), cmp);
		}
		pta += nmemb;
		pts += nmemb;
		pth += nmemb;
	}
	return 1;
}


// This is a duplicate of the 32 bit version and the only difference is that
// each instance of 'int' has been changed to 'long long'. It's a bit
// unorthodox, but it does allow for string sorting on both 32 and 64 bit
// systems with (hopefully) optimal optimization.

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

size_t flux_binary_search64(long long *array, long long key, CMPFUNC *cmp)
{
	size_t mid, top;
	long long *base = array;

	if (run == 0)
	{
		top = PIVOTS;
	}
	else
	{
		top = 1;
		base += last;

		if (cmp(base, &key) <= 0)
		{
			long long *roof = array + PIVOTS;
	
			while (1)
			{
				if (base + top >= roof)
				{
					top = roof - base;
					break;
				}
	
				base += top;
	
				if (cmp(base, &key) > 0)
				{
					base -= top;
					break;
				}
				top *= 2;
			}
		}
		else
		{
			while (1)
			{
				if (base - top <= array)
				{
					top = base - array;
					base = array;
					break;
				}
	
				base -= top;
	
				if (cmp(&key, base) > 0)
				{
					break;
				}
				top *= 2;
			}
		}
	}

	while (top > 1)
	{
		mid = top / 2;

		if (cmp(base + mid, &key) <= 0)
		{
			base += mid;
		}
		top -= mid;
	}

	top = base - array;

	run = last == top;

	return last = top;
}

size_t flux_swap64(long long *array, long long *swap, unsigned char *hash, size_t nmemb, size_t stage, CMPFUNC *cmp)
{
	unsigned char *pth;
	size_t cnt, size[PIVOTS] = { 0 };
	long long base[PIVOTS], *bptr[PIVOTS], *pta, *pte, *pts;

	pte = array + nmemb;

	for (cnt = 0 ; cnt < PIVOTS ; cnt++)
	{
		base[cnt] = array[cnt * nmemb / PIVOTS];
	}

	quadsort(base, PIVOTS, sizeof(long long), cmp);

	run = last = 0;
	pth = hash;

	for (pta = array ; pta < pte ; pta++)
	{
		size[*pth++ = flux_binary_search64(base, *pta, cmp)]++;
	}

	pts = swap;

	for (cnt = 0 ; cnt < PIVOTS ; cnt++)
	{
		bptr[cnt] = pts;

		pts += size[cnt];
	}

	pth = hash;

	for (pta = array ; pta < pte ; pta++)
	{
		*bptr[*pth++]++ = *pta;
	}

	pta = array;
	pts = swap;
	pth = hash;

	for (cnt = 0 ; cnt < PIVOTS ; cnt++)
	{
		nmemb = size[cnt];

		if (stage == 0)
		{
			if (nmemb < 1024)
			{
				memcpy(pta, pts, nmemb * sizeof(long long));
				quadsort_swap64(pta, pts, nmemb, sizeof(long long), cmp);
			}
			else
			{
				flux_swap64(pts, pta, pth, nmemb, 1, cmp);
			}
		}
		else
		{
			quadsort_swap64(pts, pta, nmemb, sizeof(long long), cmp);
		}
		pta += nmemb;
		pts += nmemb;
		pth += nmemb;
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
//┌────────────────────────────────────────────────────────────────────┐//
//│███████┐██┐     ██┐   ██┐██┐  ██┐███████┐ ██████┐ ██████┐ ████████┐ │//
//│██┌────┘██│     ██│   ██│└██┐██┌┘██┌────┘██┌───██┐██┌──██┐└──██┌──┘ │//
//│█████┐  ██│     ██│   ██│ └███┌┘ ███████┐██│   ██│██████┌┘   ██│    │//
//│██┌──┘  ██│     ██│   ██│ ██┌██┐ └────██│██│   ██│██┌──██┐   ██│    │//
//│██│     ███████┐└██████┌┘██┌┘ ██┐███████│└██████┌┘██│  ██│   ██│    │//
//│└─┘     └──────┘ └─────┘ └─┘  └─┘└──────┘ └─────┘ └─┘  └─┘   └─┘    │//
//└────────────────────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////////////////////

void fluxsort(void *array, size_t nmemb, size_t size, CMPFUNC *cmp)
{
	if (nmemb < 1024)
	{
		return quadsort(array, nmemb, size, cmp);
	}

	if (size == sizeof(int))
	{
		int *swap = malloc(nmemb * size + nmemb * sizeof(unsigned char));

		if (swap == NULL)
		{
			fprintf(stderr, "fluxsort(%p,%zu,%zu): malloc() failed: %s\n", array, nmemb, size, strerror(errno));

			return;
		}		

		flux_swap32(array, swap, (unsigned char *) (swap + nmemb), nmemb, 0, cmp);

		free(swap);
	}
	else if (size == sizeof(long long))
	{
		long long *swap = malloc(nmemb * size + nmemb * sizeof(unsigned char));

		if (swap == NULL)
		{
			fprintf(stderr, "fluxsort(%p,%zu,%zu): malloc() failed: %s\n", array, nmemb, size, strerror(errno));

			return;
		}

		flux_swap64(array, swap, (unsigned char *) (swap + nmemb), nmemb, 0, cmp);

		free(swap);
	}
	else
	{
		assert(size == sizeof(int) || size == sizeof(long long));
	}
}

#endif
