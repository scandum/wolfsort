/*
	Copyright (C) 2014-2022 Igor van den Hoven ivdhoven@gmail.com
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
	crumsort 1.1.5.2
*/

#define CRUM_AUX 512
#define CRUM_OUT  24

size_t FUNC(crum_analyze)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	char loop, dist;
	size_t cnt, balance = 0, streaks = 0;
	VAR *pta, *ptb, tmp;

	pta = array;

	for (cnt = nmemb ; cnt > 16 ; cnt -= 16)
	{
		for (dist = 0, loop = 16 ; loop ; loop--)
		{
			dist += cmp(pta, pta + 1) > 0; pta++;
		}
		streaks += (dist == 0) | (dist == 16);
		balance += dist;
	}

	while (--cnt)
	{
		balance += cmp(pta, pta + 1) > 0;
		pta++;
	}

	if (balance == 0)
	{
		return 1;
	}

	if (balance == nmemb - 1)
	{
		pta = array;
		ptb = array + nmemb;

		cnt = nmemb / 2;

		do
		{
			tmp = *pta; *pta++ = *--ptb; *ptb = tmp;
		}
		while (--cnt);

		return 1;
	}

	if (streaks >= nmemb / 24)
//	if (streaks >= nmemb / 32)
	{
		FUNC(quadsort_swap)(array, swap, swap_size, nmemb, cmp);

		return 1;
	}
	return 0;
}

VAR FUNC(crum_median_of_sqrt)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	VAR *pta, *pts, *ptx;
	size_t cnt, sqrt, div;

	sqrt = nmemb > 262144 ? 256 : 128;

	div = nmemb / sqrt;

	pta = pts = ptx = array + rand() % sqrt;

	for (cnt = 0 ; cnt < sqrt ; cnt++)
	{
		swap[0] = *pts; *pts++ = *pta; *pta = swap[0];

		pta += div;
	}
	FUNC(quadsort_swap)(ptx, swap, swap_size, sqrt, cmp);

	return ptx[sqrt / 2];
}

VAR FUNC(crum_median_of_five)(VAR *array, size_t v0, size_t v1, size_t v2, size_t v3, size_t v4, CMPFUNC *cmp)
{
	VAR swap[6], *pta;
	size_t x, y, z;

	swap[2] = array[v0];
	swap[3] = array[v1];
	swap[4] = array[v2];
	swap[5] = array[v3];

	pta = swap + 2;

	x = cmp(pta, pta + 1) > 0; y = !x; swap[0] = pta[y]; pta[0] = pta[x]; pta[1] = swap[0]; pta += 2;
	x = cmp(pta, pta + 1) > 0; y = !x; swap[0] = pta[y]; pta[0] = pta[x]; pta[1] = swap[0]; pta -= 2;
	x = cmp(pta, pta + 2) > 0; y = !x; swap[0] = pta[0]; swap[1] = pta[2]; pta[0] = swap[x]; pta[2] = swap[y]; pta++;
	x = cmp(pta, pta + 2) > 0; y = !x; swap[0] = pta[0]; swap[1] = pta[2]; pta[0] = swap[x]; pta[2] = swap[y];

	pta[2] = array[v4];

	x = cmp(pta, pta + 1) > 0;
	y = cmp(pta, pta + 2) > 0;
	z = cmp(pta + 1, pta + 2) > 0;

	return pta[(x == y) + (y ^ z)];
}

VAR FUNC(crum_median_of_twentyfive)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	VAR swap[5];
	size_t div = nmemb / 64;

	swap[0] = FUNC(crum_median_of_five)(array, div *  4, div *  1, div *  2, div *  8, div * 10, cmp);
	swap[1] = FUNC(crum_median_of_five)(array, div * 16, div * 12, div * 14, div * 18, div * 20, cmp);
	swap[2] = FUNC(crum_median_of_five)(array, div * 32, div * 24, div * 30, div * 34, div * 38, cmp);
	swap[3] = FUNC(crum_median_of_five)(array, div * 48, div * 42, div * 44, div * 50, div * 52, cmp);
	swap[4] = FUNC(crum_median_of_five)(array, div * 60, div * 54, div * 56, div * 62, div * 63, cmp);

	return FUNC(crum_median_of_five)(swap, 0, 1, 2, 3, 4, cmp);
}

size_t FUNC(crum_median_of_three)(VAR *array, size_t v0, size_t v1, size_t v2, CMPFUNC *cmp)
{
	size_t v[3] = {v0, v1, v2};
	char x, y, z;

	x = cmp(array + v0, array + v1) > 0;
	y = cmp(array + v0, array + v2) > 0;
	z = cmp(array + v1, array + v2) > 0;

	return v[(x == y) + (y ^ z)];
}

VAR FUNC(crum_median_of_nine)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	size_t x, y, z, div = nmemb / 16;

	x = FUNC(crum_median_of_three)(array, div * 2, div * 1, div * 4, cmp);
	y = FUNC(crum_median_of_three)(array, div * 8, div * 6, div * 10, cmp);
	z = FUNC(crum_median_of_three)(array, div * 14, div * 12, div * 15, cmp);

	return array[FUNC(crum_median_of_three)(array, x, y, z, cmp)];
}

// As per suggestion by Marshall Lochbaum to improve generic data handling

size_t FUNC(fulcrum_reverse_partition)(VAR *array, VAR *swap, VAR *ptx, VAR *piv, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	size_t cnt, val, i, m = 0;
	VAR *ptl, *ptr, *pta, *tpa;

	if (nmemb <= swap_size)
	{
		cnt = nmemb / 8;

		do for (i = 8 ; i ; i--)
		{
			val = cmp(piv, ptx) > 0; swap[-m] = array[m] = *ptx++; m += val; swap++;
		}
		while (--cnt);

		for (cnt = nmemb % 8 ; cnt ; cnt--)
		{
			val = cmp(piv, ptx) > 0; swap[-m] = array[m] = *ptx++; m += val; swap++;
		}
		memcpy(array + m, swap - nmemb, (nmemb - m) * sizeof(VAR));

		return m;
	}

	memcpy(swap, array, 16 * sizeof(VAR));
	memcpy(swap + 16, array + nmemb - 16, 16 * sizeof(VAR));

	ptl = array;
	ptr = array + nmemb - 1;

	pta = array + 16;
	tpa = array + nmemb - 17;

	cnt = nmemb / 16 - 2;

	while (1)
	{
		if (pta - ptl - m <= 16)
		{
			if (cnt-- == 0) break;

			for (i = 16 ; i ; i--)
			{
				val = cmp(piv, pta) > 0; ptl[m] = ptr[m] = *pta++; m += val; ptr--;
			}
		}
		if (pta - ptl - m > 16)
		{
			if (cnt-- == 0) break;

			for (i = 16 ; i ; i--)
			{
				val = cmp(piv, tpa) > 0; ptl[m] = ptr[m] = *tpa--; m += val; ptr--;
			}
		}
	}

	if (pta - ptl - m <= 16)
	{
		for (cnt = nmemb % 16 ; cnt ; cnt--)
		{
			val = cmp(piv, pta) > 0; ptl[m] = ptr[m] = *pta++; m += val; ptr--;
		}
	}
	else
	{
		for (cnt = nmemb % 16 ; cnt ; cnt--)
		{
			val = cmp(piv, tpa) > 0; ptl[m] = ptr[m] = *tpa--; m += val; ptr--;
		}
	}
	pta = swap;

	for (cnt = 32 ; cnt ; cnt--)
	{
		val = cmp(piv, pta) > 0; ptl[m] = ptr[m] = *pta++; m += val; ptr--;
	}
	return m;
}

size_t FUNC(fulcrum_default_partition)(VAR *array, VAR *swap, VAR *ptx, VAR *piv, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	size_t cnt, val, i, m = 0;
	VAR *ptl, *ptr, *pta, *tpa;

	if (nmemb <= swap_size)
	{
		cnt = nmemb / 8;

		do for (i = 8 ; i ; i--)
		{
			val = cmp(ptx, piv) <= 0; swap[-m] = array[m] = *ptx++; m += val; swap++;
		}
		while (--cnt);

		for (cnt = nmemb % 8 ; cnt ; cnt--)
		{
			val = cmp(ptx, piv) <= 0; swap[-m] = array[m] = *ptx++; m += val; swap++;
		}

		memcpy(array + m, swap - nmemb, sizeof(VAR) * (nmemb - m));

		return m;
	}

	memcpy(swap, array, 16 * sizeof(VAR));
	memcpy(swap + 16, array + nmemb - 16, 16 * sizeof(VAR));

	ptl = array;
	ptr = array + nmemb - 1;

	pta = array + 16;
	tpa = array + nmemb - 17;

	cnt = nmemb / 16 - 2;

	while (1)
	{
		if (pta - ptl - m <= 16)
		{
			if (cnt-- == 0) break;

			for (i = 16 ; i ; i--)
			{
				val = cmp(pta, piv) <= 0; ptl[m] = ptr[m] = *pta++; m += val; ptr--;
			}
		}
		if (pta - ptl - m > 16)
		{
			if (cnt-- == 0) break;

			for (i = 16 ; i ; i--)
			{
				val = cmp(tpa, piv) <= 0; ptl[m] = ptr[m] = *tpa--; m += val; ptr--;
			}
		}
	}

	if (pta - ptl - m <= 16)
	{
		for (cnt = nmemb % 16 ; cnt ; cnt--)
		{
			val = cmp(pta, piv) <= 0; ptl[m] = ptr[m] = *pta++; m += val; ptr--;
		}
	}
	else
	{
		for (cnt = nmemb % 16 ; cnt ; cnt--)
		{
			val = cmp(tpa, piv) <= 0; ptl[m] = ptr[m] = *tpa--; m += val; ptr--;
		}
	}
	pta = swap;

	for (cnt = 32 ; cnt ; cnt--)
	{
		val = cmp(pta, piv) <= 0; ptl[m] = ptr[m] = *pta++; m += val; ptr--;
	}
	return m;
}

void FUNC(fulcrum_partition)(VAR *array, VAR *swap, VAR *ptx, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	size_t a_size = 0, s_size;
	static VAR old;
	VAR piv;

	while (1)
	{
		if (nmemb <= 4096)
		{
			piv = FUNC(crum_median_of_nine)(array, nmemb, cmp);
		}
		else if (nmemb <= 65536)
		{
			piv = FUNC(crum_median_of_twentyfive)(array, nmemb, cmp);
		}
		else
		{
			piv = FUNC(crum_median_of_sqrt)(array, swap, swap_size, nmemb, cmp);
		}

		if (a_size && cmp(&old, &piv) <= 0)
		{
			a_size = FUNC(fulcrum_reverse_partition)(array, swap, array, &piv, swap_size, nmemb, cmp);
			s_size = nmemb - a_size;

			if (s_size <= a_size / 16 || a_size <= CRUM_OUT)
			{
				return FUNC(quadsort_swap)(array, swap, swap_size, a_size, cmp);
			}
			else
			{
				return FUNC(fulcrum_partition)(array, swap, array, swap_size, a_size, cmp);
			}
		}

		a_size = FUNC(fulcrum_default_partition)(array, swap, array, &piv, swap_size, nmemb, cmp);
		s_size = nmemb - a_size;

		if (a_size <= s_size / 16 || s_size <= CRUM_OUT)
		{
			if (s_size == 0)
			{
				a_size = FUNC(fulcrum_reverse_partition)(array, swap, array, &piv, swap_size, a_size, cmp);
				s_size = nmemb - a_size;

				if (s_size <= a_size / 16 || a_size <= CRUM_OUT)
				{
					return FUNC(quadsort_swap)(array, swap, swap_size, a_size, cmp);
				}
				else
				{
					return FUNC(fulcrum_partition)(array, swap, array, swap_size, a_size, cmp);
				}
			}
			FUNC(quadsort_swap)(array + a_size, swap, swap_size, s_size, cmp);
		}
		else
		{
			FUNC(fulcrum_partition)(array + a_size, swap, array + a_size, swap_size, s_size, cmp);
		}

		if (s_size <= a_size / 16 || a_size <= CRUM_OUT)
		{
			return FUNC(quadsort_swap)(array, swap, swap_size, a_size, cmp);
		}
		nmemb = a_size;
		old = piv;
	}
}

void FUNC(crumsort)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb < 32)
	{
		return FUNC(tail_swap)(array, nmemb, cmp);
	}
#if CRUM_AUX
	size_t swap_size = CRUM_AUX;
#else
	size_t swap_size = 32;

	while (swap_size * swap_size <= nmemb)
	{
		swap_size *= 4;
	}
#endif
	VAR swap[swap_size];

	if (FUNC(crum_analyze)(array, swap, swap_size, nmemb, cmp) == 0)
	{
		FUNC(fulcrum_partition)(array, swap, array, swap_size, nmemb, cmp);
	}
}

void FUNC(crumsort_swap)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb < 32)
	{
		FUNC(tail_swap)(array, nmemb, cmp);
	}
	else if (FUNC(crum_analyze)(array, swap, swap_size, nmemb, cmp) == 0)
	{
		FUNC(fulcrum_partition)(array, swap, array, swap_size, nmemb, cmp);
	}
}