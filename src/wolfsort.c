// wolfsort 1.2.1.3 - Igor van den Hoven ivdhoven@gmail.com

//#define GODMODE 

#ifdef GODMODE // inspired by rhsort, technically unstable.

void FUNC(unstable_count)(VAR *array, size_t nmemb, size_t buckets, VAR min, CMPFUNC *cmp)
{
	VAR *pta;
	size_t index;
	size_t *count = (size_t *) calloc(sizeof(size_t), buckets), loop;

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

inline void FUNC(wolf_unguarded_insert)(VAR *array, size_t offset, size_t nmemb, CMPFUNC *cmp)
{
	VAR key, *pta, *end;
	size_t i, top, x, y;

	for (i = offset ; i < nmemb ; i++)
	{
		pta = end = array + i;

		if (cmp(--pta, end) <= 0)
		{
			continue;
		}

		key = *end;

		if (cmp(array + 1, &key) > 0)
		{
			top = i - 1;

			do
			{
				*end-- = *pta--;
			}
			while (--top);

			*end-- = key;
		}
		else
		{
			do
			{
				*end-- = *pta--;
				*end-- = *pta--;
			}
			while (cmp(pta, &key) > 0);

			end[0] = end[1];
			end[1] = key;
		}
		x = cmp(end, end + 1) > 0; y = !x; key = end[y]; end[0] = end[x]; end[1] = key;
	}
}

void FUNC(wolfsort_swap)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp);

void FUNC(wolf_partition)(VAR *array, VAR *aux, size_t aux_size, size_t nmemb, VAR min, VAR max, CMPFUNC *cmp)
{
	VAR *swap, *pta, *pts, *ptd, range, moduler;
	size_t index, cnt, loop, dmemb, buckets;
	unsigned int *count, limit;

	if (nmemb < 32)
	{
		return FUNC(quadsort)(array, nmemb, cmp);
	}

	range = max - min;

	if (range >> 16 == 0 || (size_t) range <= nmemb / 4)
	{
		buckets = range + 1;
		moduler = 1;
	}
	else
	{
		buckets = nmemb <= 4 * 65536 ? nmemb / 4 : 1024;

		for (moduler = 4 ; (size_t) moduler <= range / buckets ; moduler *= 2) {}

		buckets = range / moduler + 1;
	}

	limit = (nmemb / buckets) * 4;

	count = (unsigned int *) calloc(sizeof(int), buckets);

	swap = aux;

	if (limit * buckets > aux_size)
	{
		swap = (VAR *) malloc(limit * buckets * sizeof(VAR));
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
				FUNC(wolfsort_swap)(pta, swap, limit + pts - swap, cnt, cmp);
			}
			pta += cnt;
		}
		pts += limit;
	}

	if (dmemb)
	{
		FUNC(fluxsort_swap)(ptd, swap, dmemb, dmemb, cmp);

		FUNC(partial_backward_merge)(array, swap, nmemb, nmemb, nmemb - dmemb, cmp);
	}
	if (limit * buckets > aux_size)
	{
		free(swap);
	}
	free(count);
}

void FUNC(wolf_minmax)(VAR *min, VAR *max, VAR *pta, VAR *ptb, VAR *ptc, VAR *ptd, CMPFUNC *cmp)
{
	if (cmp(min, pta) > 0) *min = *pta;
	if (cmp(pta, max) > 0) *max = *pta;
	if (cmp(min, ptb) > 0) *min = *ptb;
	if (cmp(ptb, max) > 0) *max = *ptb;
	if (cmp(min, ptc) > 0) *min = *ptc;
	if (cmp(ptc, max) > 0) *max = *ptc;
	if (cmp(min, ptd) > 0) *min = *ptd;
	if (cmp(ptd, max) > 0) *max = *ptd;
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
	cnt = nmemb / 256; // switch to quadsort if more than 50% ordered
#else
	cnt = nmemb / 512; // switch to quadsort if more than 25% ordered
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
			FUNC(cross_merge)(swap + half1, array + half1, quad3, quad4, cmp);
		}
	}
	else
	{
		FUNC(cross_merge)(swap, array, quad1, quad2, cmp);

		if (cmp(ptc, ptc + 1) <= 0)
		{
			memcpy(swap + half1, array + half1, half2 * sizeof(VAR));
		}
		else
		{
			FUNC(cross_merge)(swap + half1, ptb + 1, quad3, quad4, cmp);
		}
	}
	FUNC(cross_merge)(array, swap, half1, half2, cmp);
}

void FUNC(wolfsort)(void *array, size_t nmemb, CMPFUNC *cmp)
{
	VAR *pta = (VAR *) array;

	if (nmemb <= 132)
	{
		FUNC(quadsort)(pta, nmemb, cmp);
	}
	else
	{
		VAR *swap = (VAR *) malloc(nmemb * sizeof(VAR));

		if (swap == NULL)
		{
			FUNC(quadsort)(pta, nmemb, cmp);
			return;
		}

		FUNC(wolf_analyze)(pta, swap, nmemb, nmemb, cmp);

		free(swap);
	}
}

void FUNC(wolfsort_swap)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb <= 132)
	{
		FUNC(quadsort_swap)(array, swap, nmemb, nmemb, cmp);
	}
	else
	{
		FUNC(wolf_analyze)(array, swap, swap_size, nmemb, cmp);
	}
}
