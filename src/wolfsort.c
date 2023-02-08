/*
        Copyright (C) 2014-2023 Igor van den Hoven ivdhoven@gmail.com
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
	wolfsort 1.1.5.4
*/


//#define GODMODE 

#ifdef GODMODE // inspired by rhsort, technically unstable. Wolfsort likes stables.

void FUNC(unstable_count)(VAR *array, size_t nmemb, size_t buckets, VAR min, CMPFUNC *cmp)
{
	VAR *pta;
	size_t index;
	size_t *count = calloc(sizeof(size_t), buckets), loop;

	pta = array;

	for (index = nmemb / 16 ; index ; index--)
	{
		for (loop = 16 ; loop ; loop--)
		{
			count[*pta++ - min]++;
		}
	}

	for (index = nmemb % 16 ; index ; index--)
	{
		count[*pta++ - min]++;
	}

	pta = array;

	for (index = 0 ; index < buckets ; index++)
	{
		for (loop = count[index] ; loop ; loop--)
		{
			*pta++ = index + min;
		}
	}

	free(count);

	return;
}
#endif

void FUNC(wolf_partition)(VAR *array, VAR *aux, size_t aux_size, size_t nmemb, VAR min, VAR max, CMPFUNC *cmp)
{
	VAR *swap, *pta, *pts, *ptd, range, moduler;
	size_t index, cnt, loop, dmemb, buckets;
	unsigned short *count, limit;

	if (nmemb < 32)
	{
		return FUNC(quadsort)(array, nmemb, cmp);
	}

	range = max - min;

	if (range < 65536 || range <= nmemb / 4)
	{
		buckets = range + 1;
		moduler = 1;
	}
	else
	{
		buckets = nmemb > 8 * 65536 ? 65536 : nmemb / 8;
		moduler = range / buckets + 1;
	}

	limit = (nmemb / buckets) * 4;

	count = calloc(sizeof(short), buckets);

	swap = aux;

	if (limit * buckets > aux_size)
	{
		swap = malloc(limit * buckets * sizeof(VAR));
	}

	if (count == NULL || swap == NULL)
	{
		if (count)
		{
			free(count);
		}
		FUNC(fluxsort_swap)(array, aux, aux_size, nmemb, cmp);
		return;
	}

	ptd = pta = array;

	for (loop = nmemb ; loop ; loop--)
	{
		max = *pta++;

		index = (unsigned int) (max - min) / moduler;

		if (count[index] < limit)
		{
			swap[index * limit + count[index]++] = max;
			continue;
		}
		// The element doesn't fit, so we drop it to the main array. Inspired by rhsort.
		*ptd++ = max;
	}

	dmemb = ptd - array;

	if (dmemb)
	{
		ptd = array + nmemb - dmemb;

		memmove(ptd, array, dmemb * sizeof(VAR));
	}
	pta = array;
	pts = swap;

	for (index = 0 ; index < buckets ; index++)
	{
		cnt = count[index];

		if (cnt)
		{
			memcpy(pta, pts, cnt * sizeof(VAR));

			if (moduler > 1)
			{
				FUNC(fluxsort_swap)(pta, pts, cnt, cnt, cmp);
			}
			pta += cnt;
		}
		pts += limit;
	}

	if (dmemb)
	{
		FUNC(fluxsort_swap)(ptd, swap, dmemb, dmemb, cmp);

		if (dmemb < nmemb - dmemb)
		{
			FUNC(partial_backward_merge)(array, swap, nmemb, nmemb - dmemb, cmp);
		}
		else
		{
			FUNC(partial_forward_merge)(array, swap, nmemb, nmemb - dmemb, cmp);
		}
	}
	if (limit * buckets > aux_size)
	{
		free(swap);
	}
	free(count);
}

void FUNC(wolf_minmax)(VAR *min, VAR *max, VAR *pta, VAR *ptb, VAR *ptc, VAR *ptd, CMPFUNC *cmp)
{
	if (cmp(min, pta) > 0) *min = *pta; else if (cmp(pta, max) > 0) *max = *pta;
	if (cmp(min, ptb) > 0) *min = *ptb; else if (cmp(ptb, max) > 0) *max = *ptb;
	if (cmp(min, ptc) > 0) *min = *ptc; else if (cmp(ptc, max) > 0) *max = *ptc;
	if (cmp(min, ptd) > 0) *min = *ptd; else if (cmp(ptd, max) > 0) *max = *ptd;
}

void FUNC(wolf_analyze)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	unsigned char loop, asum, bsum, csum, dsum;
	unsigned int astreaks, bstreaks, cstreaks, dstreaks;
	size_t quad1, quad2, quad3, quad4, half1, half2;
	size_t cnt, abalance, bbalance, cbalance, dbalance;
	VAR min, max, *pta, *ptb, *ptc, *ptd;

	half1 = nmemb / 2;
	quad1 = half1 / 2;
	quad2 = half1 - quad1;
	half2 = nmemb - half1;
	quad3 = half2 / 2;
	quad4 = half2 - quad3;

	min = max = array[nmemb - 1];

	pta = array;
	ptb = array + quad1;
	ptc = array + half1;
	ptd = array + half1 + quad3;

	astreaks = bstreaks = cstreaks = dstreaks = 0;
	abalance = bbalance = cbalance = dbalance = 0;

	for (cnt = nmemb ; cnt > 132 ; cnt -= 128)
	{
		for (asum = bsum = csum = dsum = 0, loop = 32 ; loop ; loop--)
		{
			FUNC(wolf_minmax)(&min, &max, pta, ptb, ptc, ptd, cmp);

			asum += cmp(pta, pta + 1) > 0; pta++;
			bsum += cmp(ptb, ptb + 1) > 0; ptb++;
			csum += cmp(ptc, ptc + 1) > 0; ptc++;
			dsum += cmp(ptd, ptd + 1) > 0; ptd++;
		}
		abalance += asum; astreaks += (asum == 0) | (asum == 32);
		bbalance += bsum; bstreaks += (bsum == 0) | (bsum == 32);
		cbalance += csum; cstreaks += (csum == 0) | (csum == 32);
		dbalance += dsum; dstreaks += (dsum == 0) | (dsum == 32);
	}

	for ( ; cnt > 7 ; cnt -= 4)
	{
		FUNC(wolf_minmax)(&min, &max, pta, ptb, ptc, ptd, cmp);

		abalance += cmp(pta, pta + 1) > 0; pta++;
		bbalance += cmp(ptb, ptb + 1) > 0; ptb++;
		cbalance += cmp(ptc, ptc + 1) > 0; ptc++;
		dbalance += cmp(ptd, ptd + 1) > 0; ptd++;
	}

	if (quad1 < quad2)
	{
		if (cmp(&min, ptb) > 0) min = *ptb; else if (cmp(ptb, &max) > 0) max = *ptb;
		bbalance += cmp(ptb, ptb + 1) > 0; ptb++;
	}
	if (quad1 < quad3)
	{
		if (cmp(&min, ptc) > 0) min = *ptc; else if (cmp(ptc, &max) > 0) max = *ptc;
		cbalance += cmp(ptc, ptc + 1) > 0; ptc++;
	}
	if (quad1 < quad4)
	{
		if (cmp(&min, ptd) > 0) min = *ptd; else if (cmp(ptd, &max) > 0) max = *ptd;
		dbalance += cmp(ptd, ptd + 1) > 0; ptd++;
	}
	FUNC(wolf_minmax)(&min, &max, pta, ptb, ptc, ptd, cmp);

	cnt = abalance + bbalance + cbalance + dbalance;

	if (cnt == 0)
	{
		if (cmp(pta, pta + 1) <= 0 && cmp(ptb, ptb + 1) <= 0 && cmp(ptc, ptc + 1) <= 0)
		{
			return;
		}
	}

#ifdef GODMODE
	{
		VAR range = max - min;

		if (range < 65536 || range <= nmemb / 4)
		{
			FUNC(unstable_count)(array, nmemb, range + 1, min, cmp);
			return;
		}
	}
#endif

	asum = quad1 - abalance == 1;
	bsum = quad2 - bbalance == 1;
	csum = quad3 - cbalance == 1;
	dsum = quad4 - dbalance == 1;

	if (asum | bsum | csum | dsum)
	{
		unsigned char span1 = (asum && bsum) * (cmp(pta, pta + 1) > 0);
		unsigned char span2 = (bsum && csum) * (cmp(ptb, ptb + 1) > 0);
		unsigned char span3 = (csum && dsum) * (cmp(ptc, ptc + 1) > 0);

		switch (span1 | span2 * 2 | span3 * 4)
		{
			case 0: break;
			case 1: FUNC(quad_reversal)(array, ptb);   abalance = bbalance = 0; break;
			case 2: FUNC(quad_reversal)(pta + 1, ptc); bbalance = cbalance = 0; break;
			case 3: FUNC(quad_reversal)(array, ptc);   abalance = bbalance = cbalance = 0; break;
			case 4: FUNC(quad_reversal)(ptb + 1, ptd); cbalance = dbalance = 0; break;
			case 5: FUNC(quad_reversal)(array, ptb);
				FUNC(quad_reversal)(ptb + 1, ptd); abalance = bbalance = cbalance = dbalance = 0; break;
			case 6: FUNC(quad_reversal)(pta + 1, ptd); bbalance = cbalance = dbalance = 0; break;
			case 7: FUNC(quad_reversal)(array, ptd); return;
		}

		if (asum && abalance) {FUNC(quad_reversal)(array,   pta); abalance = 0;}
		if (bsum && bbalance) {FUNC(quad_reversal)(pta + 1, ptb); bbalance = 0;}
		if (csum && cbalance) {FUNC(quad_reversal)(ptb + 1, ptc); cbalance = 0;}
		if (dsum && dbalance) {FUNC(quad_reversal)(ptc + 1, ptd); dbalance = 0;}
	}

#ifdef cmp
	cnt = nmemb / 256; // switch to quadsort if at least 50% ordered
#else
	cnt = nmemb / 512; // switch to quadsort if at least 25% ordered
#endif
	asum = astreaks > cnt;
	bsum = bstreaks > cnt;
	csum = cstreaks > cnt;
	dsum = dstreaks > cnt;

#ifndef cmp
	if (quad1 > QUAD_CACHE)
	{
		asum = bsum = csum = dsum = 1;
	}
#endif
	switch (asum + bsum * 2 + csum * 4 + dsum * 8)
	{
		case 0:
			FUNC(wolf_partition)(array, swap, swap_size, nmemb, min, max, cmp);
			return;
		case 1:
			if (abalance) FUNC(quadsort_swap)(array, swap, swap_size, quad1, cmp);
			FUNC(wolf_partition)(pta + 1, swap, swap_size, quad2 + half2, min, max, cmp);
			break;
		case 2:
			FUNC(wolf_partition)(array, swap, swap_size, quad1, min, max, cmp);
			if (bbalance) FUNC(quadsort_swap)(pta + 1, swap, swap_size, quad2, cmp);
			FUNC(wolf_partition)(ptb + 1, swap, swap_size, half2, min, max, cmp);
			break;
		case 3:
			if (abalance) FUNC(quadsort_swap)(array, swap, swap_size, quad1, cmp);
			if (bbalance) FUNC(quadsort_swap)(pta + 1, swap, swap_size, quad2, cmp);
			FUNC(wolf_partition)(ptb + 1, swap, swap_size, half2, min, max, cmp);
			break;
		case 4:
			FUNC(wolf_partition)(array, swap, swap_size, half1, min, max, cmp);
			if (cbalance) FUNC(quadsort_swap)(ptb + 1, swap, swap_size, quad3, cmp);
			FUNC(wolf_partition)(ptc + 1, swap, swap_size, quad4, min, max, cmp);
			break;
		case 8:
			FUNC(wolf_partition)(array, swap, swap_size, half1 + quad3, min, max, cmp);
			if (dbalance) FUNC(quadsort_swap)(ptc + 1, swap, swap_size, quad4, cmp);
			break;
		case 9:
			if (abalance) FUNC(quadsort_swap)(array, swap, swap_size, quad1, cmp);
			FUNC(wolf_partition)(pta + 1, swap, swap_size, quad2 + quad3, min, max, cmp);
			if (dbalance) FUNC(quadsort_swap)(ptc + 1, swap, swap_size, quad4, cmp);
			break;
		case 12:
			FUNC(wolf_partition)(array, swap, swap_size, half1, min, max, cmp);
			if (cbalance) FUNC(quadsort_swap)(ptb + 1, swap, swap_size, quad3, cmp);
			if (dbalance) FUNC(quadsort_swap)(ptc + 1, swap, swap_size, quad4, cmp);
			break;
		case 5:
		case 6:
		case 7:
		case 10:
		case 11:
		case 13:
		case 14:
		case 15:
			if (asum)
			{
				if (abalance) FUNC(quadsort_swap)(array, swap, swap_size, quad1, cmp);
			}
			else FUNC(wolf_partition)(array, swap, swap_size, quad1, min, max, cmp);
			if (bsum)
			{
				if (bbalance) FUNC(quadsort_swap)(pta + 1, swap, swap_size, quad2, cmp);
			}
			else FUNC(wolf_partition)(pta + 1, swap, swap_size, quad2, min, max, cmp);
			if (csum)
			{
				if (cbalance) FUNC(quadsort_swap)(ptb + 1, swap, swap_size, quad3, cmp);
			}
			else FUNC(wolf_partition)(ptb + 1, swap, swap_size, quad3, min, max, cmp);
			if (dsum)
			{
				if (dbalance) FUNC(quadsort_swap)(ptc + 1, swap, swap_size, quad4, cmp);
			}
			else FUNC(wolf_partition)(ptc + 1, swap, swap_size, quad4, min, max, cmp);
			break;
	}

	if (cmp(pta, pta + 1) <= 0)
	{
		memcpy(swap, array, half1 * sizeof(VAR));

		if (cmp(ptc, ptc + 1) <= 0)
		{
			if (cmp(ptb, ptb + 1) <= 0)
			{
				return;
			}
			memcpy(swap + half1, array + half1, half2 * sizeof(VAR));
		}
		else
		{
			FUNC(galloping_merge)(swap + half1, array + half1, quad3, quad4, cmp);
		}
	}
	else
	{
		FUNC(galloping_merge)(swap, array, quad1, quad2, cmp);

		if (cmp(ptc, ptc + 1) <= 0)
		{
			memcpy(swap + half1, array + half1, half2 * sizeof(VAR));
		}
		else
		{
			FUNC(galloping_merge)(swap + half1, ptb + 1, quad3, quad4, cmp);
		}
	}
	FUNC(galloping_merge)(array, swap, half1, half2, cmp);
}

void FUNC(wolfsort)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb <= 132)
	{
		if (nmemb <= 24)
		{
			FUNC(tail_swap)(array, nmemb, cmp);
		}
		else
		{
			VAR swap[nmemb];

			FUNC(flux_partition)(array, swap, array, swap + nmemb, nmemb, cmp);
		}
	}
	else
	{
		VAR *swap = malloc(nmemb * sizeof(VAR));

		if (swap == NULL)
		{
			FUNC(quadsort)(array, nmemb, cmp);
			return;
		}

		FUNC(wolf_analyze)(array, swap, nmemb, nmemb, cmp);

		free(swap);
	}
}
