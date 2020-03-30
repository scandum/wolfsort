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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <algorithm>
#include "wolfsort.h"
//#include "timsort.h"
//#include "pdqsort.h"

//#include "skensort.h"

//#include "pdqsort.h" // https://github.com/orlp/pdqsort/blob/master/pdqsort.h
//#include "timsort.h" // https://github.com/timsort/cpp-TimSort/blob/master/include/gfx/timsort.hpp

#define cmp(a,b) (*(a) > *(b))

typedef int CMPFUNC (const void *a, const void *b);
typedef void SRTFUNC(void *array, size_t nmemb, size_t size, CMPFUNC *ignore);

// benchmarking utilities


// Must prevent inlining so the benchmark is fair.
// Remove __attribute__ ((noinline)) and counter++ for full throttle.

long long counter;

__attribute__ ((noinline)) int cmp_int(const void * a, const void * b)
{
	counter++;

	return *(int *) a - *(int *) b;
}

__attribute__ ((noinline)) int cmp_stable(const void * a, const void * b)
{
	counter++;

	return *(int *) a / 1000 - *(int *) b / 1000;
}

__attribute__ ((noinline)) int cmp_long(const void * a, const void * b)
{
	counter++;

	return *(long long *) a - *(long long *) b;
}

int cmp_str(const void * a, const void * b)
{
	return strcmp(*(const char **) a, *(const char **) b);
}

int cmp_float(const void * a, const void * b)
{
	return *(float *) a - *(float *) b;
}


long long utime()
{
	struct timeval now_time;

	gettimeofday(&now_time, NULL);

	return now_time.tv_sec * 1000000LL + now_time.tv_usec;
}

unsigned long long randval;
unsigned short randkey;

void seed_rand(unsigned long long seed)
{
	srand(seed);

	randval = 0;
	randkey = 0;
}

int generate_rand()
{
	static short randlist[] = { 635, 202,  47, 229, 311, 319, 512, 456, 645, 496, 230, 548, 680, 785,  95, 577,  10, 990, 215, 120, 316, 602, 281, 521, 498, 871, 688,  83, 789,  15, 952, 851, 499, 618, 168, 623, 946, 810, 326, 174, 586, 204, 400, 193, 619,  99, 382, 975, 935, 867, 756, 146, 509, 664, 155, 121, 790, 798, 924, 809, 648, 848, 704, 761, 578, 219, 211, 379, 272, 823, 860,  62, 331, 689, 796, 365,  64, 537, 129, 106, 507, 563, 514, 394,  80, 978, 890, 642, 757, 397, 413, 862, 726, 124, 329, 395,  26, 765, 727, 245, 784, 422,  69, 253, 665, 711, 484,1000, 274,  75, 754,   8, 900, 296, 136, 742,  23, 112,  86, 604, 740, 583, 687, 251, 831, 295, 483, 728, 780, 930, 703, 628, 772, 134, 712, 290, 363, 559, 438, 541,  35, 152, 127, 141, 934,  88, 226, 461, 692, 959, 614, 116, 323, 891, 607,  19, 377, 344,  51, 409, 300, 841, 519, 367,  44, 844, 543, 824, 699, 238, 643, 529, 317, 861,  30, 575, 600, 629, 621, 589, 314, 947, 273, 615, 854, 915,  97, 598, 327, 225, 366, 717, 405, 579, 534,  42, 939, 555, 746,  56,  -1, 768, 622, 749, 639, 503, 123, 364, 440, 582, 686, 111, 592, 696,  54, 685,  91,  84, 815, 213,  58, 806, 228, 481, 487, 426, 655, 974, 189,  48, 305, 567, 408, 139, 337, 195, 666, 950, 849, 502, 510, 896, 508, 353, 762, 200, 962, 801, 807,  74, 562, 335, 817, 656, 658, 268,  93, 332, 743, 953, 275, 362, 993,   9, 439, 304, 721, 786, 347, 695, 932, 956,  39, 661, 386, 147, 170, 999, 886, 125, 209, 977, 778, 417, 166, 553, 585, 128, 863, 430, 828, 760, 971, 374, 613, 958, 114, 271, 187, 167, 533, 735, 130, 117, 470, 899, 183, 544, 855, 471, 856,  17, 280, 513, 581, 766, 410, 217, 800, 165, 858, 574, 938, 457, 928, 398, 516, 819,  57, 826, 526, 911, 278,  78, 682, 460, 916, 944, 348, 773, 840, 700, 907, 455, 587,  98, 803, 632, 732, 452, 158, 485, 774, 875, 926, 320, 820,  31, 976, 336, 731, 545, 451, 110, 898, 775,  65, 431, 432, 143, 196, 449, 429, 105, 651, 558, 324,  40, 887, 670, 266, 294,  94, 720, 465, 985, 612, 564, 435, 473, 433, 736,  36, 151, 659, 118, 620, 436, 684, 162,  81, 406, 877, 552,  29, 292, 706, 286, 637, 741, 265, 423, 897, 838, 835, 631, 690, 869, 968, 488, 532, 244, 538, 325, 909, 918, 171, 825, 965, 210, 556, 355, 994, 821, 771, 942,  18, 142, 179, 207, 258, 984, 252, 298, 970,   5, 960, 590, 961, 748, 458,  14, 234, 747, 178, 107, 415, 561, 176, 986, 378, 315, 352, 328, 239,  77,  37, 357, 262,  25, 650, 906, 992, 881, 599, 836, 805, 199, 626, 842, 233, 933, 674,  90, 421, 160, 387, 948, 719, 192, 657, 913, 420, 401, 257, 845, 763, 235, 609, 360, 549, 669, 852, 540, 425, 843, 859, 437, 921, 227, 865, 223, 901, 963, 678, 610, 660, 243, 812, 249,  76, 282, 480, 321, 447, 291, 232, 216, 725,   2, 949, 254,  66, 551, 109, 853, 722, 770,  50, 799, 969, 462, 181, 570,  85, 649, 568,  87,  41,  55, 531, 905, 873, 389, 270, 910, 381, 222, 634, 565, 153, 888, 157,  12, 697, 369, 385, 857, 596,  59, 528, 597, 987, 611, 834, 205, 191, 115, 312, 776, 246, 520, 497, 522, 751, 662, 370, 515, 638, 804, 995, 560, 203, 135, 482, 264, 633, 866, 444,  13, 795, 554,   6, 755, 788, 943, 988, 248, 240, 293, 966, 654, 428, 412, 917, 603, 955, 730, 138, 122, 399, 500, 467, 892, 104, 707, 698, 375, 716, 874, 380, 330, 880, 624, 277, 997, 212,  28, 414, 672, 769, 256, 595, 530, 705, 302, 702,  16, 713, 103, 448, 150, 889,  68, 576, 102, 914, 403, 505, 967, 572, 920, 868, 486, 476,  34,  79, 945, 182, 802, 679, 126, 536, 723, 715, 830,  67, 644, 523, 427, 673, 208, 677, 829, 464, 359, 338, 388, 940, 701, 504, 954, 535, 601, 557, 101, 864, 185, 354, 474, 133, 169, 145, 832, 310, 653, 816, 671, 833,  96, 214, 989, 964, 220, 753, 511, 902, 284, 606, 361, 839, 744, 402,  89, 738, 376, 495, 494, 259,  20, 847, 255,   3, 925, 883, 148, 779, 903, 588, 927, 303, 759, 285, 391,  45, 190,  32, 973, 113, 781, 797, 591, 822, 188, 368, 221,   4, 683, 491,  63, 782, 694, 161, 247, 893, 418, 313, 652, 501, 724, 392, 159, 573,  61,  82, 922, 267, 709, 524, 617, 792, 373,   7, 269, 149, 184, 627, 383, 100, 814, 137, 459, 827, 198, 396, 289, 108, 342, 729, 941, 493, 371, 206, 393, 341, 472, 983, 550, 318, 640, 641, 919, 951, 580, 894, 339, 131, 878, 197, 908, 156, 132, 468, 384, 201,  43, 681, 419, 982, 998, 154, 475, 279, 593, 739, 236, 787,  27,  92, 767, 119, 818, 569, 301, 346, 636, 885, 605, 306, 180, 340, 663,  22, 441, 584, 646, 693, 837, 708, 691, 194, 322, 527, 808, 443, 811, 283,  71, 566, 542, 453, 676, 261, 608, 794, 980, 518, 224, 675, 616, 263, 434, 241, 349, 260, 850, 309, 870, 490, 594, 358, 630, 390,  53,  46, 173, 299, 846, 625, 163,  49, 164, 463, 469, 745, 547,  11, 750, 979, 172, 218, 758,  60, 478, 345, 175, 186, 546, 287,  52, 237, 876, 404, 424,  72, 783, 350, 912, 144, 177, 454, 667, 872, 710, 351, 981, 334, 479, 416,  33, 996, 882, 937, 442, 242, 929, 450, 571, 733, 647, 506,  38,  24, 718, 297,  70, 525, 764, 445,  73, 714, 791, 250, 466, 752, 668, 140, 923, 489, 813, 477, 936, 972, 777, 879, 231, 793, 991, 904, 895,  21, 931, 308, 884, 492, 446, 737, 957, 372, 407, 517, 734, 307, 288, 343, 333, 276, 539, 356, 411 };

	return 1000 + randlist[++randkey % 1000];
}

void test_sort(void *array, void *unsorted, void *valid, int minimum, int maximum, int samples, int repetitions, SRTFUNC *srt, const char *name, const char *desc, size_t size, CMPFUNC *cmpf)
{
	long long start, end, total, best, average;
	size_t rep, sam, max;
	long long *ptla = array;
	int *pta = array, *ptv = valid, cnt;

	best = average = 0;

	for (sam = 0 ; sam < samples ; sam++)
	{
		total = 0;

		max = minimum;

		for (rep = 0 ; rep < repetitions ; rep++)
		{
			memcpy(array, unsorted, max * size);

			counter = 0;

			if (max == 10 && sam == 0 && rep == 0)
				printf("\e[1;31m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n",
					pta[0], pta[1], pta[2], pta[3], pta[4],
					pta[5], pta[6], pta[7], pta[8], pta[9]);

			start = utime();

			switch (*name)
			{
				case 'q':
					quadsort(array, max, size, NULL);
					break;

				case 'w':
					wolfsort(array, max, size, NULL);
					break;

				case 's':
					if (size == sizeof(int))
					{
						std::stable_sort(pta, pta + max);
					}
					else
					{
						std::stable_sort(ptla, ptla + max);
					}
					break;

				case 'p':
#ifdef PDQSORT_H
					if (size == sizeof(int))
					{
						pdqsort(pta, pta + max);
					}
					else
					{
						pdqsort(ptla, ptla + max);
					}
					break;
#else
					return;
#endif

				case 't':
#ifdef GFX_TIMSORT_HPP
					if (size == sizeof(int))
					{
						gfx::timsort(pta, pta + max);
					}
					else
					{
						gfx::timsort(ptla, ptla + max);
					}
					break;
#else
					return;
#endif
			}

			end = utime();

			if (max == 10 && sam == 0 && rep == 0)
				printf("\e[1;32m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n",
					pta[0], pta[1], pta[2], pta[3], pta[4],
					pta[5], pta[6], pta[7], pta[8], pta[9]);

			total += end - start;

			if (minimum < maximum)
			{
				max++;

				if (max > maximum)
				{
					max = minimum;
				}
			}

		}

		if (!best || total < best)
		{
			best = total;
		}
		average += total;
	}

	if (repetitions == 0)
	{
		return;
	}

	average /= samples;

	if (cmpf == cmp_stable)
	{
		for (cnt = 1 ; cnt < maximum ; cnt++)
		{
			if (pta[cnt - 1] > pta[cnt])
			{
				if (maximum == 1000)
				{
					printf("%17s: sorted %7d i%ds in %f seconds. KO: %5lld (un%s)\n", name, maximum, (int) size * 8, best / 1000000.0, counter, desc);
				}
				else if (maximum == 1000000)
				{
					printf("%17s: sorted %7d i%ds in %f seconds. MO: %10lld (un%s)\n", name, maximum, (int) size * 8, best / 1000000.0, counter, desc);
				}
				else
				{
					printf("%17s: sorted %7d i%ds in %f seconds. (un%s)\n", name, maximum, (int) size * 8, best / 1000000.0, desc);
				}
				return;
			}
		}
	}

	if (*name == 'q')
	{
		if (!strcmp(desc, "random order"))
		{
			printf("%s\n", "|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |");
			printf("%s\n", "| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |");
		}
		else
		{
			printf("%s\n", "|           |          |      |          |          |             |                  |");
		}
	}

	if (counter)
	{
		printf("|%10s | %8d |  i%d | %f | %f | %11lld | %16s |\n", name, maximum, (int) size * 8, best / 1000000.0, average / 1000000.0, counter, desc);
	}
	else
	{
		printf("|%10s | %8d |  i%d | %f | %f | %11s | %16s |\n", name, maximum, (int) size * 8, best / 1000000.0, average / 1000000.0, " ", desc);
	}

	if (minimum != maximum)
	{
		return;
	}

	for (cnt = 1 ; cnt < max ; cnt++)
	{
		if (size == sizeof(int))
		{
			if (pta[cnt - 1] > pta[cnt])
			{
				printf("%17s: not properly sorted at index %d. (%d vs %d\n", name, cnt, pta[cnt - 1], pta[cnt]);
				break;
			}
			if (pta[cnt - 1] == pta[cnt])
			{
//				printf("%17s: Found a repeat value at index %d. (%d)\n", name, cnt, pta[cnt]);
			}
		}
		else if (size == sizeof(long long))
		{
			if (ptla[cnt - 1] > ptla[cnt])
			{
				printf("%17s: not properly sorted at index %d. (%lld vs %lld\n", name, cnt, ptla[cnt - 1], ptla[cnt]);
				break;
			}
		}
	}

	for (cnt = 1 ; cnt < max ; cnt++)
	{
		if (pta[cnt] != ptv[cnt])
		{
			printf("         validate: array[%d] != valid[%d]. (%d vs %d\n", cnt, cnt, pta[cnt], ptv[cnt]);
			break;
		}
	}
}

void validate()
{
	int seed = time(NULL);
	int cnt, val, max = 2000000;

	int *a_array, *r_array, *v_array;

	seed_rand(seed);

	a_array = (int *) malloc(max * sizeof(int));
	r_array = (int *) malloc(max * sizeof(int));
	v_array = (int *) malloc(max * sizeof(int));

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = generate_rand();
	}


	for (cnt = 1 ; cnt < 100 ; cnt++)
	{
		memcpy(a_array, r_array, max * sizeof(int));
		memcpy(v_array, r_array, max * sizeof(int));

		quadsort(a_array, cnt, sizeof(int), cmp_int);
		qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val])
			{
				printf("\e[1;31mvalidate rand: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
				return;
			}

			if (a_array[val] != v_array[val])
			{
				printf("\e[1;31mvalidate rand: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
				return;
			}
		}
	}

	// ascending saw

	for (cnt = 0 ; cnt < 1000 ; cnt++)
	{
		r_array[cnt] = rand();
	}

        quadsort(r_array + max / 4 * 0, max / 4, sizeof(int), cmp_int);
        quadsort(r_array + max / 4 * 1, max / 4, sizeof(int), cmp_int);
        quadsort(r_array + max / 4 * 2, max / 4, sizeof(int), cmp_int);
        quadsort(r_array + max / 4 * 3, max / 4, sizeof(int), cmp_int);

        for (cnt = 1 ; cnt < 1000 ; cnt += 7)
	{
		memcpy(a_array, r_array, max * sizeof(int));
	        memcpy(v_array, r_array, max * sizeof(int));

	        quadsort(a_array, cnt, sizeof(int), cmp_int);
	        qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val])
			{
				printf("\e[1;31mvalidate ascending saw: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
				return;
			}

			if (a_array[val] != v_array[val])
			{
				printf("\e[1;31mvalidate ascending saw: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
				return;
			}
		}
	}

        // descending saw

        for (cnt = 0 ; cnt < 1000 ; cnt++)
        {
                r_array[cnt] = (max - cnt - 1) % 100000;
        }

        for (cnt = 1 ; cnt < 1000 ; cnt += 7)
	{
		memcpy(a_array, r_array, max * sizeof(int));
	        memcpy(v_array, r_array, max * sizeof(int));

	        quadsort(a_array, cnt, sizeof(int), cmp_int);
	        qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val])
			{
				printf("\e[1;31mvalidate descending saw: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
				return;
			}

			if (a_array[val] != v_array[val])
			{
				printf("\e[1;31mvalidate descending saw: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
				return;
			}
		}
	}

	// random tail

	for (cnt = 0 ; cnt < max * 3 / 4 ; cnt++)
	{
		r_array[cnt] = cnt;
	}

	for (cnt = max * 3 / 4 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

        for (cnt = 1 ; cnt < 1000 ; cnt += 7)
	{
		memcpy(a_array, r_array, max * sizeof(int));
	        memcpy(v_array, r_array, max * sizeof(int));

	        quadsort(a_array, cnt, sizeof(int), cmp_int);
	        qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val])
			{
				printf("\e[1;31mvalidate rand tail: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
				return;
			}

			if (a_array[val] != v_array[val])
			{
				printf("\e[1;31mvalidate rand tail: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
				return;
			}
		}
	}

	free(a_array);
	free(r_array);
	free(v_array);
}


int main(int argc, char **argv)
{
	int max = 100000;
	int samples = 100;
	int repetitions = 1;

	size_t cnt, rnd;
	int *a_array, *r_array, *v_array;
	long long *la_array, *lr_array, *lv_array;

	if (argc >= 1 && argv[1] && *argv[1])
	{
		max = atoi(argv[1]);
	}

	if (argc >= 2 && argv[2] && *argv[2])
	{
		samples = atoi(argv[2]);
	}

	if (argc >= 3 && argv[3] && *argv[3])
	{
		repetitions = atoi(argv[3]);
	}

	validate();

	rnd = time(NULL);

	// 64 bit

	la_array = (long long *) malloc(max * sizeof(long long));
	lr_array = (long long *) malloc(max * sizeof(long long));
	lv_array = (long long *) malloc(max * sizeof(long long));

	if (la_array == NULL || lr_array == NULL || lv_array == NULL)
	{
		printf("main(%d,%d,%d): malloc: %s\n", max, samples, repetitions, strerror(errno));

		return 0;
	}

	// random 

	seed_rand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		lr_array[cnt] = rand();
		lr_array[cnt] += (unsigned long long) rand() << 32ULL;
	}

	memcpy(lv_array, lr_array, max * sizeof(long long));
	quadsort(lv_array, max, sizeof(long long), cmp_long);

	test_sort(la_array, lr_array, lv_array, max, max, samples, repetitions, quadsort,        "quadsort",       "random order", sizeof(long long), cmp_long);
	test_sort(la_array, lr_array, lv_array, max, max, samples, repetitions, qsort,           "stablesort",        "random order", sizeof(long long), cmp_long);
	test_sort(la_array, lr_array, lv_array, max, max, samples, repetitions, qsort,           "timsort",        "random order", sizeof(long long), cmp_long);
	test_sort(la_array, lr_array, lv_array, max, max, samples, repetitions, qsort,           "pdqsort",        "random order", sizeof(long long), cmp_long);
	test_sort(la_array, lr_array, lv_array, max, max, samples, repetitions, qsort,           "wolfsort",        "random order", sizeof(long long), cmp_long);

	printf("\n");

	// 32 bit

	a_array = (int *) malloc(max * sizeof(int));
	r_array = (int *) malloc(max * sizeof(int));
	v_array = (int *) malloc(max * sizeof(int));


	// random

	seed_rand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = max != 1000 ? rand() : generate_rand();
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, quadsort,        "quadsort",       "random order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "stablesort",        "random order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "timsort",        "random order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "pdqsort",        "random order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "wolfsort",        "random order", sizeof(int), cmp_int);

//	printf("\n");

	// ascending

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = cnt;
	}

        memcpy(v_array, r_array, max * sizeof(int));
        memcpy(r_array, v_array, max * sizeof(int));

	quadsort(v_array, max, sizeof(int), cmp_int);

	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, quadsort,        "quadsort",       "ascending", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "stablesort",     "ascending", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "timsort",        "ascending", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "pdqsort",        "ascending", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "wolfsort",        "ascending", sizeof(int), cmp_int);

//	printf("\n");

	// ascending saw

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = max != 1000 ? rand() : generate_rand();
	}

        memcpy(v_array, r_array, max * sizeof(int));
        quadsort(v_array + max / 4 * 0, max / 4, sizeof(int), cmp_int);
        quadsort(v_array + max / 4 * 1, max / 4, sizeof(int), cmp_int);
        quadsort(v_array + max / 4 * 2, max / 4, sizeof(int), cmp_int);
        quadsort(v_array + max / 4 * 3, max / 4, sizeof(int), cmp_int);
        memcpy(r_array, v_array, max * sizeof(int));

	quadsort(v_array, max, sizeof(int), cmp_int);

	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, quadsort,        "quadsort",       "ascending saw", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "stablesort",     "ascending saw", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "timsort",        "ascending saw", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "pdqsort",        "ascending saw", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "wolfsort",        "ascending saw", sizeof(int), cmp_int);

//	printf("\n");

	// uniform

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = max != 1000 ? rand() % 100 : generate_rand() % 100;
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, quadsort,        "quadsort",        "generic order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "stablesort",         "generic order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "timsort",         "generic order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "pdqsort",         "generic order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "wolfsort",         "generic order", sizeof(int), cmp_int);


//	printf("\n");

        // descending

        for (cnt = 0 ; cnt < max ; cnt++)
        {
                r_array[cnt] = (max - cnt);
        }

        memcpy(v_array, r_array, max * sizeof(int));
        quadsort(v_array, max, sizeof(int), cmp_int);

	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, quadsort,        "quadsort",        "descending order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "stablesort",         "descending order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "timsort",         "descending order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "pdqsort",         "descending order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "wolfsort",         "descending order", sizeof(int), cmp_int);

//	printf("\n");

        // descending saw

        for (cnt = 0 ; cnt < max ; cnt++)
        {
                r_array[cnt] = (max - cnt - 1) % 10000;
        }

        memcpy(v_array, r_array, max * sizeof(int));
        quadsort(v_array, max, sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, quadsort,        "quadsort",        "descending saw", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "stablesort",      "descending saw", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "timsort",         "descending saw", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "pdqsort",         "descending saw", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "wolfsort",         "descending saw", sizeof(int), cmp_int);

//	printf("\n");

	// random tail

	seed_rand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		if (max > 1000)
		{
			r_array[cnt] = rand();
		}
		else
		{
			r_array[cnt] = generate_rand();
		}
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max * 3 / 4, sizeof(int), cmp_int);
	memcpy(r_array, v_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, quadsort,        "quadsort",        "random tail", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "stablesort",         "random tail", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "timsort",         "random tail", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "pdqsort",         "random tail", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "wolfsort",         "random tail", sizeof(int), cmp_int);

//	printf("\n");

	seed_rand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		if (max > 1000)
		{
			r_array[cnt] = rand();
		}
		else
		{
			r_array[cnt] = generate_rand();
		}
	}

        memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max / 2, sizeof(int), cmp_int);
//      quadsort(v_array + max / 2, max / 2, sizeof(int), cmp_int);

	memcpy(r_array, v_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, quadsort,        "quadsort",        "random half", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "stablesort",           "random half", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "timsort",           "random half", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "pdqsort",           "random half", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "wolfsort",           "random half", sizeof(int), cmp_int);

//	printf("\n");

	// wave ?

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		if (cnt % 2)
		{
			r_array[cnt] = 16777216 + cnt;
		}
		else
		{
			r_array[cnt] = 33554432 + cnt;
		}
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, quadsort,        "quadsort",       "wave order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "stablesort",        "wave order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "timsort",          "wave order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "pdqsort",          "wave order", sizeof(int), cmp_int);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "wolfsort",          "wave order", sizeof(int), cmp_int);

//	printf("\n");

	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, quadsort,        "quadsort",       "stable", sizeof(int), cmp_stable);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "stablesort",        "stable", sizeof(int), cmp_stable);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "timsort",        "stable", sizeof(int), cmp_stable);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "pdqsort",        "stable", sizeof(int), cmp_stable);
	test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort,           "wolfsort",        "stable", sizeof(int), cmp_stable);

//	printf("\n");

	if (repetitions > 0)
	{
		return 0;
	}

	if (max >= 16)
	{
		// random range

		seed_rand(rnd);

		for (cnt = 0 ; cnt < 4 ; cnt++)
		{
			r_array[cnt] = generate_rand();
		}

		memcpy(v_array, r_array, 8 * sizeof(int));
		quadsort(v_array, 8, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 1, 4, 500, 100, quadsort,        "quadsort",          "random 1-4", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1, 4, 500, 100, qsort,           "stablesort",        "random 1-4", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1, 4, 500, 100, qsort,           "timsort",           "random 1-4", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1, 4, 500, 100, qsort,           "pdqsort",           "random 1-4", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1, 4, 500, 100, qsort,           "wolfsort",           "random 1-4", sizeof(int), cmp_int);

	//	printf("\n");

		seed_rand(rnd);

		for (cnt = 0 ; cnt < 8 ; cnt++)
		{
			r_array[cnt] = generate_rand();
		}

		memcpy(v_array, r_array, 8 * sizeof(int));
		quadsort(v_array, 8, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 5, 8, 500, 100, quadsort,        "quadsort",          "random 5-8", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 5, 8, 500, 100, qsort,           "stablesort",        "random 5-8", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 5, 8, 500, 100, qsort,           "timsort",           "random 5-8", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 5, 8, 500, 100, qsort,           "pdqsort",           "random 5-8", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 5, 8, 500, 100, qsort,           "wolfsort",           "random 5-8", sizeof(int), cmp_int);

	//	printf("\n");

		seed_rand(rnd);

		for (cnt = 0 ; cnt < 16 ; cnt++)
		{
			r_array[cnt] = generate_rand();
		}

		memcpy(v_array, r_array, 16 * sizeof(int));
		quadsort(v_array, 16, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 9, 15, 500, 100, quadsort,        "quadsort",          "random 9-15", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 9, 15, 500, 100, qsort,           "stablesort",        "random 9-15", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 9, 15, 500, 100, qsort,           "timsort",           "random 9-15", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 9, 15, 500, 100, qsort,           "pdqsort",           "random 9-15", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 9, 15, 500, 100, qsort,           "wolfsort",           "random 9-15", sizeof(int), cmp_int);

	//	printf("\n");

		seed_rand(rnd);

		for (cnt = 0 ; cnt < 16 ; cnt++)
		{
			lr_array[cnt] = generate_rand();
		}

		memcpy(lv_array, lr_array, 16 * sizeof(long long));
		quadsort(lv_array, 16, sizeof(long long), cmp_long);

		test_sort(la_array, lr_array, lv_array, 1, 15, 500, 100, quadsort,        "quadsort",          "random 1-15", sizeof(long long), cmp_long);
		test_sort(la_array, lr_array, lv_array, 1, 15, 500, 100, qsort,           "stablesort",        "random 1-15", sizeof(long long), cmp_long);
		test_sort(la_array, lr_array, lv_array, 1, 15, 500, 100, qsort,           "timsort",           "random 1-15", sizeof(long long), cmp_long);
		test_sort(la_array, lr_array, lv_array, 1, 15, 500, 100, qsort,           "pdqsort",           "random 1-15", sizeof(long long), cmp_long);
		test_sort(la_array, lr_array, lv_array, 1, 15, 500, 100, qsort,           "wolfsort",           "random 1-15", sizeof(long long), cmp_long);

	}

//	printf("\n");

	if (max >= 64)
	{
		// random range

		seed_rand(rnd);

		for (cnt = 0 ; cnt < 64 ; cnt++)
		{
			r_array[cnt] = generate_rand();
		}

		memcpy(v_array, r_array, 64 * sizeof(int));
		quadsort(v_array, 64, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 16, 63, 500, 100, quadsort,        "quadsort",        "random 16-63", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 16, 63, 500, 100, qsort,           "stablesort",         "random 16-63", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 16, 63, 500, 100, qsort,           "timsort",           "random 16-63", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 16, 63, 500, 100, qsort,           "pdqsort",           "random 16-63", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 16, 63, 500, 100, qsort,           "wolfsort",           "random 16-63", sizeof(int), cmp_int);
	}

//	printf("\n");

	if (max >= 128)
	{
		// random range

		seed_rand(rnd);

		for (cnt = 0 ; cnt < 128 ; cnt++)
		{
			r_array[cnt] = generate_rand();
		}

		memcpy(v_array, r_array, 128 * sizeof(int));
		quadsort(v_array, 128, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 64, 127, 500, 100, quadsort,        "quadsort",        "random 64-127", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 64, 127, 500, 100, qsort,           "stablesort",         "random 64-127", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 64, 127, 500, 100, qsort,           "timsort",           "random 64-127", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 64, 127, 500, 100, qsort,           "pdqsort",           "random 64-127", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 64, 127, 500, 100, qsort,           "wolfsort",           "random 64-127", sizeof(int), cmp_int);
	}

//	printf("\n");

	if (max >= 256)
	{
		// random range

		seed_rand(rnd);

		for (cnt = 0 ; cnt < 256 ; cnt++)
		{
			r_array[cnt] = generate_rand();
		}

		memcpy(v_array, r_array, 256 * sizeof(int));
		quadsort(v_array, 256, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 127, 255, 500, 128, quadsort,        "quadsort",        "random 128-255", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 127, 255, 500, 128, qsort,           "stablesort",         "random 128-255", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 127, 255, 500, 128, qsort,           "timsort",           "random 128-255", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 127, 255, 500, 128, qsort,           "pdqsort",           "random 128-255", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 127, 255, 500, 128, qsort,           "wolfsort",           "random 128-255", sizeof(int), cmp_int);
	}

//	printf("\n");

	if (max >= 512)
	{
		// random range

		seed_rand(rnd);

		for (cnt = 0 ; cnt < 512 ; cnt++)
		{
			r_array[cnt] = generate_rand();
		}

		memcpy(v_array, r_array, 512 * sizeof(int));
		quadsort(v_array, 512, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 256, 511, 500, 256, quadsort,        "quadsort",        "random 256-511", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 256, 511, 500, 256, qsort,           "stablesort",         "random 256-511", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 256, 511, 500, 256, qsort,           "timsort",           "random 256-511", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 256, 511, 500, 256, qsort,           "pdqsort",           "random 256-511", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 256, 511, 500, 256, qsort,           "wolfsort",           "random 256-511", sizeof(int), cmp_int);
	}

//	printf("\n");

	if (max >= 1024)
	{
		// random range

		seed_rand(rnd);

		for (cnt = 0 ; cnt < 1024 ; cnt++)
		{
			r_array[cnt] = generate_rand();
		}

		memcpy(v_array, r_array, 1024 * sizeof(int));
		quadsort(v_array, 1024, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 512, 1023, 50, 512, quadsort,        "quadsort",        "random 512-1023", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 512, 1023, 50, 512, qsort,           "stablesort",         "random 512-1023", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 512, 1023, 50, 512, qsort,           "timsort",           "random 512-1023", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 512, 1023, 50, 512, qsort,           "pdqsort",           "random 512-1023", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 512, 1023, 50, 512, qsort,           "wolfsort",           "random 512-1023", sizeof(int), cmp_int);

	//	printf("\n");

		seed_rand(rnd);

		for (cnt = 0 ; cnt < 1024 ; cnt++)
		{
			r_array[cnt] = generate_rand();
		}

		memcpy(v_array, r_array, 1024 * sizeof(int));
		quadsort(v_array, 1024, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 1, 1023, 50, 1024, quadsort,        "quadsort",        "random 1-1023", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1, 1023, 50, 1024, qsort,           "stablesort",         "random 1-1023", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1, 1023, 50, 1024, qsort,           "timsort",           "random 1-1023", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1, 1023, 50, 1024, qsort,           "pdqsort",           "random 1-1023", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1, 1023, 50, 1024, qsort,           "wolfsort",           "random 1-1023", sizeof(int), cmp_int);

	}

//	printf("\n");

	if (max >= 2048)
	{
		// random range

		seed_rand(rnd);

		for (cnt = 0 ; cnt < 2048 ; cnt++)
		{
			r_array[cnt] = generate_rand();
		}

		memcpy(v_array, r_array, 2048 * sizeof(int));
		quadsort(v_array, 2048, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 1024, 2047, 50, 1024, quadsort,        "quadsort",          "random 1024-2047", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1024, 2047, 50, 1024, qsort,           "stablesort",        "random 1024-2047", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1024, 2047, 50, 1024, qsort,           "timsort",           "random 1024-2047", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1024, 2047, 50, 1024, qsort,           "pdqsort",           "random 1024-2047", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1024, 2047, 50, 1024, qsort,           "wolfsort",           "random 1024-2047", sizeof(int), cmp_int);
	}

//	printf("\n");

	if (max >= 4096)
	{
		// random range

		seed_rand(rnd);

		for (cnt = 0 ; cnt < 4096 ; cnt++)
		{
			r_array[cnt] = generate_rand();
		}

		memcpy(v_array, r_array, 4096 * sizeof(int));
		quadsort(v_array, 4096, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 2048, 4095, 20, 2048, quadsort,        "quadsort",          "random 2048-4095", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 2048, 4095, 20, 2048, qsort,           "stablesort",        "random 2048-4095", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 2048, 4095, 20, 2048, qsort,           "timsort",           "random 2048-4095", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 2048, 4095, 20, 2048, qsort,           "pdqsort",           "random 2048-4095", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 2048, 4095, 20, 2048, qsort,           "wolfsort",           "random 2048-4095", sizeof(int), cmp_int);
	}

//	printf("\n");

	free(la_array);
	free(lr_array);
	free(lv_array);

	free(a_array);
	free(r_array);
	free(v_array);

	return 0;
}
