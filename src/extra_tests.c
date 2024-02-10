#ifdef QUAD_DEBUG

	// random % 4

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = rand() % 4;
	}
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, 0, "random % 4", sizeof(VAR), cmp_int);

	// semi random

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = rand() % 8 / 7 * rand();
	}
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, 0, "semi random", sizeof(VAR), cmp_int);

	// random signal

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		if (cnt < mem / 2)
		{
			r_array[cnt] = cnt + rand() % 16;
		}
		else
		{
			r_array[cnt] = mem - cnt + rand() % 16;
		}
	}
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, 0, "random signal", sizeof(VAR), cmp_int);

	// exponential

	for (cnt = 0 ; cnt < mem ; cnt++)
	{
		r_array[cnt] = (size_t) (cnt * cnt) % 10000; //(1 << 30);
	}
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, 0, "exponential", sizeof(VAR), cmp_int);

	// random fragments -- Make array 92% sorted

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}
	quadsort(r_array + quad0, quad1 / 100 * 98, sizeof(VAR), cmp_int);
	quadsort(r_array + quad1, quad1 / 100 * 98, sizeof(VAR), cmp_int);
	quadsort(r_array + half1, quad1 / 100 * 98, sizeof(VAR), cmp_int);
	quadsort(r_array + span3, quad1 / 100 * 98, sizeof(VAR), cmp_int);

	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "chaos fragments", sizeof(VAR), cmp_int);

	// Make array 12% sorted, this tends to make timsort/powersort slower than fully random

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}
	quadsort(r_array + quad0 / 1, quad1 * 2 / 100, sizeof(VAR), cmp_int);
	quadsort(r_array + quad1 / 2, quad1 * 2 / 100, sizeof(VAR), cmp_int);
	quadsort(r_array + quad1 / 1, quad1 * 2 / 100, sizeof(VAR), cmp_int);
	quadsort(r_array + half1 / 1, quad1 * 2 / 100, sizeof(VAR), cmp_int);
	quadsort(r_array + span3 / 2, quad1 * 2 / 100, sizeof(VAR), cmp_int);
	quadsort(r_array + span3 / 1, quad1 * 2 / 100, sizeof(VAR), cmp_int);

	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "order fragments", sizeof(VAR), cmp_int);

	// Make array 95% generic

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		if (rand() % 20 == 0)
		{
			r_array[cnt] = rand();
		}
		else
		{
			r_array[cnt] = 1000000000;
		}
	}
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "95% generic", sizeof(VAR), cmp_int);

	// Three saws

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}
	quadsort(r_array, max / 3, sizeof(VAR), cmp_int);
	quadsort(r_array + max / 3, max / 3, sizeof(VAR), cmp_int);
	quadsort(r_array + max / 3 * 2, max / 3, sizeof(VAR), cmp_int);

	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "three saws", sizeof(VAR), cmp_int);

	// various combinations of reverse and ascending order data
/*
	for (cnt = 0 ; cnt < max ; cnt++) r_array[cnt] = rand();
	quadsort(r_array + quad0, half1, sizeof(VAR), cmp_int);
	quadsort(r_array + half1, half2, sizeof(VAR), cmp_int);
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "aaaaa aaaaa", sizeof(VAR), cmp_int);

	for (cnt = 0 ; cnt < max ; cnt++) r_array[cnt] = rand();
	quadsort(r_array + quad1 / 2, nmemb - quad1 / 2, sizeof(VAR), cmp_int);
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "raaaaaaaaaa", sizeof(VAR), cmp_int);

	size_t span2 = quad2 + quad3 + quad4;

	for (cnt = 0 ; cnt < max ; cnt++) r_array[cnt] = rand();
	quadsort(r_array + quad1, span2, sizeof(VAR), cmp_int);
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "rr aaaaaaaa", sizeof(VAR), cmp_int);

	for (cnt = 0 ; cnt < max ; cnt++) r_array[cnt] = rand();
	quadsort(r_array + quad0, quad1, sizeof(VAR), cmp_int);
	quadsort(r_array + half1, half2, sizeof(VAR), cmp_int);
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "aa rr aaaaa", sizeof(VAR), cmp_int);

	for (cnt = 0 ; cnt < max ; cnt++) r_array[cnt] = rand();
	quadsort(r_array + quad0, half1, sizeof(VAR), cmp_int);
	quadsort(r_array + span3, quad4, sizeof(VAR), cmp_int);
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "aaaaa rr aa", sizeof(VAR), cmp_int);

	for (cnt = 0 ; cnt < max ; cnt++) r_array[cnt] = rand();
	quadsort(r_array + quad0, nmemb, sizeof(VAR), cmp_int);
	qsort(r_array + quad0, half1, sizeof(VAR), cmp_rev);
	qsort(r_array + half1, half2, sizeof(VAR), cmp_rev);
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "rrrrr rrrrr", sizeof(VAR), cmp_int);

	for (cnt = 0 ; cnt < max ; cnt++) r_array[cnt] = rand();
	quadsort(r_array + quad0, nmemb, sizeof(VAR), cmp_int);
	qsort(r_array + quad0, quad1, sizeof(VAR), cmp_rev);
	qsort(r_array + quad1, quad2, sizeof(VAR), cmp_rev);
	qsort(r_array + half1, quad3, sizeof(VAR), cmp_rev);
	qsort(r_array + span3, quad4, sizeof(VAR), cmp_rev);
	run_test(a_array, r_array, v_array, max, max, samples, repetitions, repetitions, "rr rr rr rr", sizeof(VAR), cmp_int);
*/
#endif
