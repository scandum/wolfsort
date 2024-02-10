// skipsort 1.2.1.3 - Igor van den Hoven ivdhoven@gmail.com

void FUNC(skip_partition)(VAR *array, VAR *swap, VAR *ptx, VAR *ptp, size_t nmemb, CMPFUNC *cmp);

// Similar to quadsort, but detect both random and reverse order runs

int FUNC(skip_analyze)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	size_t count, span;
	VAR *pta, *pts;
	unsigned char v1, v2, v3, v4, x;
	pta = array;

	count = nmemb / 8;

	while (count--)
	{
		// granular

		v1 = cmp(pta + 0, pta + 1) > 0;
		v2 = cmp(pta + 2, pta + 3) > 0;
		v3 = cmp(pta + 4, pta + 5) > 0;
		v4 = cmp(pta + 6, pta + 7) > 0;

		switch (v1 + v2 * 2 + v3 * 4 + v4 * 8)
		{
			case 0:
				if (cmp(pta + 1, pta + 2) <= 0 && cmp(pta + 3, pta + 4) <= 0 && cmp(pta + 5, pta + 6) <= 0)
				{
					goto ordered;
				}
				pts = pta;
				goto random;

			case 15:
				if (cmp(pta + 1, pta + 2) > 0 && cmp(pta + 3, pta + 4) > 0 && cmp(pta + 5, pta + 6) > 0)
				{
					pts = pta;
					goto reversed;
				}

			default:
				pts = pta;
				goto random;
		}

		random: // random

		pta += 8;

		if (count--)
		{
			v1 = cmp(pta + 0, pta + 1) > 0;
			v2 = cmp(pta + 2, pta + 3) > 0;
			v3 = cmp(pta + 4, pta + 5) > 0;
			v4 = cmp(pta + 6, pta + 7) > 0;

			switch (v1 + v2 * 2 + v3 * 4 + v4 * 8)
			{
				case 0:
					if (cmp(pta + 1, pta + 2) <= 0 && cmp(pta + 3, pta + 4) <= 0 && cmp(pta + 5, pta + 6) <= 0)
					{
						if (count)
						{
							pta += 8;
							if (cmp(pta + 0, pta + 1) <= 0 && cmp(pta + 1, pta + 2) <= 0 && cmp(pta + 2, pta + 3) <= 0 && cmp(pta + 3, pta + 4) <= 0 && cmp(pta + 4, pta + 5) <= 0 && cmp(pta + 5, pta + 6) <= 0 && cmp(pta + 6, pta + 7) <= 0)
							{
								pta -= 8;
								break;
							}
							count--;
						}
					}
					goto randomc;

				case 15:
					if (cmp(pta + 1, pta + 2) > 0 && cmp(pta + 3, pta + 4) > 0 && cmp(pta + 5, pta + 6) > 0)
					{
						break;
					}

				default:
				randomc:
					if (count >= 6)
					{
						count -= 6;
						pta += 48;
					}
					goto random;
			}
			span = (pta - pts);

			if (span <= 96)
			{
				FUNC(tail_swap)(pts, swap, span, cmp);
			}
			else
			{
				FUNC(flux_partition)(pts, swap, pts, swap + span, span, cmp);
			}

			if (v1 | v2 | v3 | v4)
			{
				pts = pta;
				goto reversed;
			}
			pta += 8;
			count--;
			goto ordered;
		}
		span = (pta - pts);

		if (span <= 96)
		{
			FUNC(tail_swap)(pts, swap, span, cmp);
			break;
		}
		if (pts == array)
		{
			FUNC(flux_partition)(array, swap, pts, swap + nmemb, nmemb, cmp);
			return 1;
		}
		FUNC(flux_partition)(pts, swap, pts, swap + span, span, cmp);
		break;

		ordered: // ordered 

		pta += 8;

		if (count--)
		{
			if ((v1 = cmp(pta + 0, pta + 1) > 0) | (v2 = cmp(pta + 2, pta + 3) > 0) | (v3 = cmp(pta + 4, pta + 5) > 0) | (v4 = cmp(pta + 6, pta + 7) > 0))
			{
				pts = pta;
				goto random;
			}
			if (cmp(pta + 1, pta + 2) <= 0 && cmp(pta + 3, pta + 4) <= 0 && cmp(pta + 5, pta + 6) <= 0)
			{
				goto ordered;
			}
			FUNC(quad_swap_merge)(pta, swap, cmp);
			pta += 8;
			continue;
		}
		break;

		reversed: // reversed

		pta += 8;

		if (count--)
		{
			if ((v1 = cmp(pta + 0, pta + 1) <= 0) | (v2 = cmp(pta + 2, pta + 3) <= 0) | (v3 = cmp(pta + 4, pta + 5) <= 0) | (v4 = cmp(pta + 6, pta + 7) <= 0))
			{
				not_reversed:

				x = !v1; swap[0] = pta[v1]; pta[0] = pta[x]; pta[1] = swap[0]; pta += 2;
				x = !v2; swap[0] = pta[v2]; pta[0] = pta[x]; pta[1] = swap[0]; pta += 2;
				x = !v3; swap[0] = pta[v3]; pta[0] = pta[x]; pta[1] = swap[0]; pta += 2;
				x = !v4; swap[0] = pta[v4]; pta[0] = pta[x]; pta[1] = swap[0]; pta -= 6;

				if (cmp(pta + 1, pta + 2) > 0 || cmp(pta + 3, pta + 4) > 0 || cmp(pta + 5, pta + 6) > 0)
				{
					FUNC(quad_swap_merge)(pta, swap, cmp);
				}
			}
			else
			{
				if (cmp(pta - 1, pta) > 0 && cmp(pta + 1, pta + 2) > 0 && cmp(pta + 3, pta + 4) > 0 && cmp(pta + 5, pta + 6) > 0)
				{
					goto reversed;
				}
				goto not_reversed;
			}
			FUNC(quad_reversal)(pts, pta - 1);
			pta += 8;
			continue;
		}

		switch (nmemb % 8)
		{
			case 7: if (cmp(pta + 5, pta + 6) <= 0) break;
			case 6: if (cmp(pta + 4, pta + 5) <= 0) break;
			case 5: if (cmp(pta + 3, pta + 4) <= 0) break;
			case 4: if (cmp(pta + 2, pta + 3) <= 0) break;
			case 3: if (cmp(pta + 1, pta + 2) <= 0) break;
			case 2: if (cmp(pta + 0, pta + 1) <= 0) break;
			case 1: if (cmp(pta - 1, pta + 0) <= 0) break;
			case 0:
				FUNC(quad_reversal)(pts, pta + nmemb % 8 - 1);

				if (pts == array)
				{
					return 1;
				}
				goto reverse_end;
		}
		FUNC(quad_reversal)(pts, pta - 1);
		break;
	}
	FUNC(tail_swap)(pta, swap, nmemb % 8, cmp);

	reverse_end:

	pta = array;

	for (count = nmemb / 32 ; count-- ; pta += 32)
	{
		if (cmp(pta + 7, pta + 8) <= 0 && cmp(pta + 15, pta + 16) <= 0 && cmp(pta + 23, pta + 24) <= 0)
		{
			continue;
		}
		FUNC(parity_merge)(swap, pta, 8, 8, cmp);
		FUNC(parity_merge)(swap + 16, pta + 16, 8, 8, cmp);
		FUNC(parity_merge)(pta, swap, 16, 16, cmp);
	}

	if (nmemb % 32 > 8)
	{
		FUNC(tail_merge)(pta, swap, 32, nmemb % 32, 8, cmp);
	}
	return 0;
}

void FUNC(skipsort)(void *array, size_t nmemb, CMPFUNC *cmp)
{
	VAR *pta = (VAR *) array;

	if (nmemb <= 96)
	{
		VAR swap[nmemb];

		FUNC(tail_swap)(pta, swap, nmemb, cmp);
	}
	else
	{
		VAR *swap = (VAR *) malloc(nmemb * sizeof(VAR));

		if (swap == NULL)
		{
			FUNC(quadsort)(pta, nmemb, cmp);
			return;
		}
		if (FUNC(skip_analyze)(pta, swap, nmemb, nmemb, cmp) == 0)
		{
			FUNC(quad_merge)(pta, swap, nmemb, nmemb, 32, cmp);
		}
		free(swap);
	}
}

void FUNC(skipsort_swap)(VAR *array, VAR *swap, size_t swap_size, size_t nmemb, CMPFUNC *cmp)
{
	if (nmemb <= 96)
	{
		FUNC(tail_swap)(array, swap, nmemb, cmp);
	}
	else if (swap_size < nmemb)
	{
		FUNC(quadsort_swap)(array, swap, swap_size, nmemb, cmp);
	}
	else
	{
		FUNC(skip_analyze)(array, swap, swap_size, nmemb, cmp);
	}
}
