Intro
-----

This document describes a stable adaptive hybrid radix / quick / merge / drop sort named wolfsort. Dropsort gained popularity after it was reinvented as Stalin sort. A [benchmark](https://github.com/scandum/wolfsort#benchmark-for-wolfsort-v1154-dripsort) is available at the bottom.

Why a hybrid?
-------------
While an adaptive merge sort is very fast at sorting ordered data, its inability to effectively
partition is its greatest weakness. Radix sort on the other hand is unable to take advantage of
sorted data. While quicksort is fast at partitioning, a radix sort is faster on medium-sized
arrays in the 1K - 1M element range. Dropsort in turn hybradizes surprisingly well with radix
and sample sorts.

History
-------
Wolfsort 1, codename: quantumsort, started out with the concept that memory is in abundance on
modern systems. I theorized that by allocating 8n memory performance could be increased by allowing
a radix sort to partition in one pass.

Not all the memory would be used or ever accessed however, which is why I envisioned it as a type
of poor-man's quantum computing. The extra memory only serves to simplify computations. The concept
kind of worked, except that large memory allocations in C can be either very fast or very slow. I
didn't investigate why.

Wolfsort 2, codename: flowsort, is when I reinvented counting sort. Instead of making 1 pass and
using extra memory to deal with fluctuations in the data, flowsort makes one pass to calculate the
bucket sizes, then makes a second pass to neatly fill the buckets.

Wolfsort 3, codename: dripsort, was inspired by the work of M. Lochbaum on [rhsort](https://github.com/mlochbaum/rhsort)
to use a method similar to dropsort to deal with bucket overflow, and to calculate the minimum and
maximum value to optimize for distributions with a small range of values. Dripsort once again makes
one pass and uses around 4n memory to deal with fluctuations in the data. Compared to v1 this is a
50% reduction in memory allocation, while at the same time significantly increasing robustness.

Analyzer
--------
Wolfsort uses the same analyzer as [fluxsort](https://github.com/scandum/fluxsort) to sort fully
in-order and fully reverse-order distributions in n comparisons. The array is split into 4 segments
for which a measure of presortedness is calculated. Mostly ordered segments are sorted with
[quadsort](https://github.com/scandum/quadsort), while mostly random segments are sorted with wolfsort.

In addition, the minimum and maximum value in the distribution is obtained.

Setting the bucket size
-----------------------
Wolfsort operates like a typical lazy radix sort by defaulting to 256 buckets and dividing each unsigned
32 bit integer by 16777216.

For optimal performance wolfsort needs to end up with between 1 and 4 elements per bucket, so the
bucket size is increased until the average bucket holds 2 or 3 elements. However, the buckets should
remain in the L1 cache, so the growth is stopped when the number of buckets reaches 65536.

This sets the optimal range for wolfsort between 2 * 256 (512) and 4 * 65536 (262144) elements. Beyond
the optimal range performance will degrade steadily. Once the average bucket size reaches the threshold
of 18 elements (1179648 total elements) the sort becomes less optimal than quicksort, though it retains
a computational advantage for a little while longer.

By computing the minimum and maximum value in the data distribution, the number of buckets are optimized
further to target the sweet spot.

Dropsort
--------
Dropsort was first proposed as an alternative sorting algorithm by David Morgan in 2006, it makes one pass
and is lossy. The algorithm was reinvented in 2018 as Stalin sort. The use of dropsort in
combination with a radix sort was introduced in 2022 by rhsort (Robin Hood Sort) and was in turn inspired
by Robin Hood Hashing.

Wolfsort allocates 4n memory to allow some deviancy in the data distribution and minimize bucket overflow.
In the case an element is too deviant and overflows the bucket, it is copied in-place to the input
array. In near-optimal cases this results in a minimal drip, in the worst case it will result in a downpour
of elements being copied to the input array.

While a centrally planned partitioning system has its weaknesses, the worst case is mostly alleviated by using
fluxsort on the deviant elements once partitioning finishes. Fluxsort is broadly adaptive and is often
strong against distributions where wolfsort is weak.

Small number sorting
--------------------
Since wolfsort uses auxiliary memory, each partition is stable once partitioning completes. The next
step is to sort the content of each bucket using fluxsort. If the number of elements in a bucket is
below 24, fluxsort defaults to quadsort, which is highly optimized for sorting small arrays using a
combination of branchless parity merges and twice-unguarded insertion.

Once each bucket is sorted, all that remains is merging the two distributions of compliant and deviant
elements, and wolfsort is finished.

Memory overhead
---------------
Wolfsort requires 4n memory for the partitioning process and O(sqrt n) memory for the buckets.

If not enough memory is available wolfsort falls back on fluxsort, which requires exactly 1n swap memory,
and if that's not sufficient fluxsort falls back on quadsort which can sort in-place. It is an
option to fall back on blitsort instead of quadsort, but since this would be an a-typical case,
and increase dependencies, I didn't implement this.

64 bit integers
---------------
With the advent of fluxsort and crumsort the dominance of radix sorts has been pushed out of 64 bit territory. Increased memory-level-parallelism in future hardware, or algorithmic optimizations, might make radix sorts feasible again for 64 bit types.

God Mode
--------
Wolfsort supports a cheat mode where the sort becomes unstable. This trick was taken from rhsort. Since wolfsort aspires to have some utility as a stable sort, this method is disabled by default, including in the benchmark.

In the benchmark rhsort does use this optimization, but it's only relevant for the random % 100 distribution. For 32 bit random integers rhsort easily beats wolfsort without an unfair advantage.

Interface
---------
Wolfsort uses the same interface as qsort, which is described in [man qsort](https://man7.org/linux/man-pages/man3/qsort.3p.html).

Wolfsort also comes with the `wolfsort_prim(void *array, size_t nmemb, size_t size)` function to perform primitive comparisons on arrays of 32 and 64 bit integers. Nmemb is the number of elements, while size should be either `sizeof(int)` or `sizeof(long long)` for signed integers, and `sizeof(int) + 1` or `sizeof(long long) + 1` for unsigned integers. Support for the char and short types can be easily added in wolfsort.h.

Wolfsort can only sort arrays of primitive integers by default, it should be able to sort tables with some minor changes, but it'll require a different interface than qsort() provides.

Proof of concept
----------------
Wolfsort is primarily a proof of concept for a hybrid radix / comparison sort. It only supports non-negative integers.

I'll briefly mention other sorting algorithms listed in the benchmark code / graphs. They can all be considered the fastest algorithms currently available in their particular class.

Blitsort
--------
[Blitsort](https://github.com/scandum/blitsort) is a hybrid in-place stable adaptive rotate quick / merge sort.

Crumsort
--------
[Crumsort](https://github.com/scandum/crumsort) is a hybrid in-place unstable adaptive quick / rotate merge sort.

Quadsort
--------
[Quadsort](https://github.com/scandum/quadsort) is an adaptive mergesort. It supports rotations as a fall-back to sort in-place. It has very good performance when it comes to sorting tables and generally outperforms timsort.

Gridsort
--------
[Gridsort](https://github.com/scandum/gridsort) is a stable comparison sort which stores data in a 2 dimensional self-balancing grid. It has some interesting properties and was the fastest comparison sort for random data for a brief period of time.

Fluxsort
--------
[Fluxsort](https://github.com/scandum/fluxsort) is a hybrid stable branchless out-of-place quick / merge sort.

Piposort
[Piposort](https://github.com/scandum/piposort) is a simplified branchless quadsort with a much smaller code size and complexity while still being very fast. Piposort might be of use to people who want to port quadsort. This is a lot easier when you start out small.

rhsort
------
[rhsort](https://github.com/mlochbaum/rhsort) is a hybrid stable out-of-place counting / radix / drop / insertion sort. It has exceptional performance on random and generic data for medium array sizes.

Ska sort
--------
[Ska sort](https://github.com/skarupke/ska_sort) is an advanced radix sort. It offers both an in-place and out-of-place version, but since the out-of-place unstable version is not very competitive with wolfsort, I only benchmark the stable and faster ska_sort_copy variant.

Big O
-----
```
                 ┌───────────────────────┐┌────────────────────┐
                 │comparisons            ││swap memory         │
┌───────────────┐├───────┬───────┬───────┤├──────┬──────┬──────┤┌──────┐┌─────────┐┌─────────┐┌─────────┐
│name           ││min    │avg    │max    ││min   │avg   │max   ││stable││partition││adaptive ││compares │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│blitsort       ││n      │n log n│n log n││1     │1     │1     ││yes   ││yes      ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│crumsort       ││n      │n log n│n log n││1     │1     │1     ││no    ││yes      ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│fluxsort       ││n      │n log n│n log n││n     │n     │n     ││yes   ││yes      ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│gridsort       ││n      │n log n│n log n││n     │n     │n     ││yes   ││yes      ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│quadsort       ││n      │n log n│n log n││1     │n     │n     ││yes   ││no       ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│wolfsort       ││n      │n log n│n log n││n     │n     │n     ││yes   ││yes      ││yes      ││hybrid   │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│rhsort         ││n      │n log n│n log n││n     │n     │n     ││yes   ││yes      ││semi     ││hybrid   │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│skasort_copy   ││n k    │n k    │n k    ││n     │n     │n     ││yes   ││yes      ││no       ││no       │
└───────────────┘└───────┴───────┴───────┘└──────┴──────┴──────┘└──────┘└─────────┘└─────────┘└─────────┘
```

Benchmark for Wolfsort v1.1.5.4 (dripsort)
-------------------------------

rhsort vs wolfsort vs ska_sort_copy on 100K elements
----------------------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1) on 100,000 32 bit integers.
The source code was compiled using g++ -O3 -fpermissive bench.c. All comparisons are inlined through the cmp macro.
A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/radix1.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|    rhsort |   100000 |   32 | 0.000680 | 0.000708 |         0 |     100 |     random order |
|  wolfsort |   100000 |   32 | 0.000965 | 0.000983 |         0 |     100 |     random order |
|   skasort |   100000 |   32 | 0.000617 | 0.000622 |         0 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000119 | 0.000122 |         0 |     100 |     random % 100 |
|  wolfsort |   100000 |   32 | 0.000370 | 0.000374 |         0 |     100 |     random % 100 |
|   skasort |   100000 |   32 | 0.000752 | 0.000759 |         0 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000301 | 0.000322 |         0 |     100 |  ascending order |
|  wolfsort |   100000 |   32 | 0.000088 | 0.000090 |         0 |     100 |  ascending order |
|   skasort |   100000 |   32 | 0.000691 | 0.000713 |         0 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000592 | 0.000618 |         0 |     100 |    ascending saw |
|  wolfsort |   100000 |   32 | 0.000371 | 0.000382 |         0 |     100 |    ascending saw |
|   skasort |   100000 |   32 | 0.000617 | 0.000650 |         0 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000572 | 0.000583 |         0 |     100 |       pipe organ |
|  wolfsort |   100000 |   32 | 0.000248 | 0.000250 |         0 |     100 |       pipe organ |
|   skasort |   100000 |   32 | 0.000618 | 0.000625 |         0 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000396 | 0.000410 |         0 |     100 | descending order |
|  wolfsort |   100000 |   32 | 0.000098 | 0.000099 |         0 |     100 | descending order |
|   skasort |   100000 |   32 | 0.000669 | 0.000680 |         0 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000593 | 0.000603 |         0 |     100 |   descending saw |
|  wolfsort |   100000 |   32 | 0.000468 | 0.000476 |         0 |     100 |   descending saw |
|   skasort |   100000 |   32 | 0.000620 | 0.000658 |         0 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000614 | 0.000638 |         0 |     100 |      random tail |
|  wolfsort |   100000 |   32 | 0.000463 | 0.000478 |         0 |     100 |      random tail |
|   skasort |   100000 |   32 | 0.000618 | 0.000639 |         0 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000643 | 0.000677 |         0 |     100 |      random half |
|  wolfsort |   100000 |   32 | 0.000668 | 0.000691 |         0 |     100 |      random half |
|   skasort |   100000 |   32 | 0.000618 | 0.000646 |         0 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.002240 | 0.002316 |         0 |     100 |  ascending tiles |
|  wolfsort |   100000 |   32 | 0.000684 | 0.000703 |         0 |     100 |  ascending tiles |
|   skasort |   100000 |   32 | 0.001109 | 0.001131 |         0 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000813 | 0.000849 |         0 |     100 |     bit reversal |
|  wolfsort |   100000 |   32 | 0.000760 | 0.000848 |         0 |     100 |     bit reversal |
|   skasort |   100000 |   32 | 0.000753 | 0.000767 |         0 |     100 |     bit reversal |

</details>

The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. It measures the performance on random data with array sizes
ranging from 10 to 10,000,000. It's generated by running the benchmark using 10000000 0 0 as the argument. The benchmark is weighted, meaning the number of repetitions
halves each time the number of items doubles. A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/radix2.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|    rhsort |       10 |   32 | 0.131513 | 0.134111 |       0.0 |      10 |        random 10 |
|  wolfsort |       10 |   32 | 0.051008 | 0.051118 |       0.0 |      10 |        random 10 |
|   skasort |       10 |   32 | 0.100410 | 0.105244 |       0.0 |      10 |        random 10 |
|           |          |      |          |          |           |         |                  |
|    rhsort |      100 |   32 | 0.067045 | 0.067871 |       0.0 |      10 |       random 100 |
|  wolfsort |      100 |   32 | 0.110760 | 0.111562 |       0.0 |      10 |       random 100 |
|   skasort |      100 |   32 | 0.232155 | 0.232376 |       0.0 |      10 |       random 100 |
|           |          |      |          |          |           |         |                  |
|    rhsort |     1000 |   32 | 0.054274 | 0.054540 |       0.0 |      10 |      random 1000 |
|  wolfsort |     1000 |   32 | 0.095062 | 0.095328 |       0.0 |      10 |      random 1000 |
|   skasort |     1000 |   32 | 0.056759 | 0.056964 |       0.0 |      10 |      random 1000 |
|           |          |      |          |          |           |         |                  |
|    rhsort |    10000 |   32 | 0.055843 | 0.056260 |       0.0 |      10 |     random 10000 |
|  wolfsort |    10000 |   32 | 0.092480 | 0.092637 |       0.0 |      10 |     random 10000 |
|   skasort |    10000 |   32 | 0.058090 | 0.058317 |       0.0 |      10 |     random 10000 |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.069535 | 0.070230 |       0.0 |      10 |    random 100000 |
|  wolfsort |   100000 |   32 | 0.098330 | 0.098880 |       0.0 |      10 |    random 100000 |
|   skasort |   100000 |   32 | 0.062893 | 0.063044 |       0.0 |      10 |    random 100000 |
|           |          |      |          |          |           |         |                  |
|    rhsort |  1000000 |   32 | 0.178629 | 0.184670 |       0.0 |      10 |   random 1000000 |
|  wolfsort |  1000000 |   32 | 0.150146 | 0.152040 |       0.0 |      10 |   random 1000000 |
|   skasort |  1000000 |   32 | 0.068434 | 0.069190 |       0.0 |      10 |   random 1000000 |
|           |          |      |          |          |           |         |                  |
|    rhsort | 10000000 |   32 | 0.373823 | 0.417923 |         0 |      10 |  random 10000000 |
|  wolfsort | 10000000 |   32 | 0.427918 | 0.428926 |         0 |      10 |  random 10000000 |
|   skasort | 10000000 |   32 | 0.114597 | 0.115324 |         0 |      10 |  random 10000000 |

</details>

Benchmark for Wolfsort v1.1.5.3 (flowsort)
-------------------------------

fluxsort vs gridsort vs quadsort vs wolfsort on 100K elements
-------------------------------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using g++ -O3 -fpermissive bench.c. All comparisons are inlined through the cmp macro.
A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph1.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  fluxsort |   100000 |   64 | 0.001884 | 0.001906 |         1 |     100 |     random order |
|  gridsort |   100000 |   64 | 0.002815 | 0.002839 |         1 |     100 |     random order |
|  quadsort |   100000 |   64 | 0.002742 | 0.002762 |         1 |     100 |     random order |
|  wolfsort |   100000 |   64 | 0.001880 | 0.001905 |         1 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  fluxsort |   100000 |   32 | 0.001807 | 0.001824 |         1 |     100 |     random order |
|  gridsort |   100000 |   32 | 0.002785 | 0.002847 |         1 |     100 |     random order |
|  quadsort |   100000 |   32 | 0.002680 | 0.002704 |         1 |     100 |     random order |
|  wolfsort |   100000 |   32 | 0.001124 | 0.001135 |         1 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000682 | 0.000697 |         1 |     100 |     random % 100 |
|  gridsort |   100000 |   32 | 0.002347 | 0.002411 |         1 |     100 |     random % 100 |
|  quadsort |   100000 |   32 | 0.002328 | 0.002346 |         1 |     100 |     random % 100 |
|  wolfsort |   100000 |   32 | 0.000687 | 0.000702 |         1 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000046 | 0.000046 |         1 |     100 |  ascending order |
|  gridsort |   100000 |   32 | 0.000280 | 0.000284 |         1 |     100 |  ascending order |
|  quadsort |   100000 |   32 | 0.000069 | 0.000069 |         1 |     100 |  ascending order |
|  wolfsort |   100000 |   32 | 0.000052 | 0.000053 |         1 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000813 | 0.000823 |         1 |     100 |    ascending saw |
|  gridsort |   100000 |   32 | 0.000857 | 0.000881 |         1 |     100 |    ascending saw |
|  quadsort |   100000 |   32 | 0.000780 | 0.000789 |         1 |     100 |    ascending saw |
|  wolfsort |   100000 |   32 | 0.001043 | 0.001049 |         1 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000372 | 0.000377 |         1 |     100 |       pipe organ |
|  gridsort |   100000 |   32 | 0.000464 | 0.000475 |         1 |     100 |       pipe organ |
|  quadsort |   100000 |   32 | 0.000339 | 0.000344 |         1 |     100 |       pipe organ |
|  wolfsort |   100000 |   32 | 0.000377 | 0.000384 |         1 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000057 | 0.000058 |         1 |     100 | descending order |
|  gridsort |   100000 |   32 | 0.000264 | 0.000269 |         1 |     100 | descending order |
|  quadsort |   100000 |   32 | 0.000056 | 0.000056 |         1 |     100 | descending order |
|  wolfsort |   100000 |   32 | 0.000063 | 0.000063 |         1 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000814 | 0.000828 |         1 |     100 |   descending saw |
|  gridsort |   100000 |   32 | 0.000857 | 0.000872 |         1 |     100 |   descending saw |
|  quadsort |   100000 |   32 | 0.000781 | 0.000790 |         1 |     100 |   descending saw |
|  wolfsort |   100000 |   32 | 0.001041 | 0.001052 |         1 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000930 | 0.000941 |         1 |     100 |      random tail |
|  gridsort |   100000 |   32 | 0.001042 | 0.001053 |         1 |     100 |      random tail |
|  quadsort |   100000 |   32 | 0.000899 | 0.000911 |         1 |     100 |      random tail |
|  wolfsort |   100000 |   32 | 0.001022 | 0.001032 |         1 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.001628 | 0.001646 |         1 |     100 |      random half |
|  gridsort |   100000 |   32 | 0.001674 | 0.001688 |         1 |     100 |      random half |
|  quadsort |   100000 |   32 | 0.001594 | 0.001605 |         1 |     100 |      random half |
|  wolfsort |   100000 |   32 | 0.001113 | 0.001121 |         1 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.001172 | 0.001192 |         1 |     100 |  ascending tiles |
|  gridsort |   100000 |   32 | 0.000715 | 0.000728 |         1 |     100 |  ascending tiles |
|  quadsort |   100000 |   32 | 0.000885 | 0.000899 |         1 |     100 |  ascending tiles |
|  wolfsort |   100000 |   32 | 0.001179 | 0.001205 |         1 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.001639 | 0.001686 |         1 |     100 |     bit reversal |
|  gridsort |   100000 |   32 | 0.002222 | 0.002257 |         1 |     100 |     bit reversal |
|  quadsort |   100000 |   32 | 0.002338 | 0.002379 |         1 |     100 |     bit reversal |
|  wolfsort |   100000 |   32 | 0.001399 | 0.001418 |         1 |     100 |     bit reversal |

</details>

fluxsort vs gridsort vs quadsort vs wolfsort on 10M elements
------------------------------------------------------------

![Graph](/images/graph2.png)
<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  fluxsort | 10000000 |   64 | 0.308943 | 0.335428 |         1 |       3 |     random order |
|  gridsort | 10000000 |   64 | 0.455605 | 0.482164 |         1 |       3 |     random order |
|  quadsort | 10000000 |   64 | 0.472495 | 0.473274 |         1 |       3 |     random order |
|  wolfsort | 10000000 |   64 | 0.312488 | 0.315160 |         1 |       3 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  fluxsort | 10000000 |   32 | 0.259171 | 0.261448 |         1 |      10 |     random order |
|  gridsort | 10000000 |   32 | 0.355227 | 0.357552 |         1 |      10 |     random order |
|  quadsort | 10000000 |   32 | 0.413495 | 0.414651 |         1 |      10 |     random order |
|  wolfsort | 10000000 |   32 | 0.229678 | 0.230089 |         1 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.097939 | 0.099613 |         1 |      10 |     random % 100 |
|  gridsort | 10000000 |   32 | 0.176730 | 0.178361 |         1 |      10 |     random % 100 |
|  quadsort | 10000000 |   32 | 0.394085 | 0.394782 |         1 |      10 |     random % 100 |
|  wolfsort | 10000000 |   32 | 0.097527 | 0.099378 |         1 |      10 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.006999 | 0.007147 |         1 |      10 |  ascending order |
|  gridsort | 10000000 |   32 | 0.033335 | 0.033666 |         1 |      10 |  ascending order |
|  quadsort | 10000000 |   32 | 0.011839 | 0.012158 |         1 |      10 |  ascending order |
|  wolfsort | 10000000 |   32 | 0.007024 | 0.007177 |         1 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.106033 | 0.106387 |         1 |      10 |    ascending saw |
|  gridsort | 10000000 |   32 | 0.089285 | 0.089569 |         1 |      10 |    ascending saw |
|  quadsort | 10000000 |   32 | 0.102307 | 0.102624 |         1 |      10 |    ascending saw |
|  wolfsort | 10000000 |   32 | 0.176965 | 0.177436 |         1 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.078222 | 0.079080 |         1 |      10 |       pipe organ |
|  gridsort | 10000000 |   32 | 0.052793 | 0.053039 |         1 |      10 |       pipe organ |
|  quadsort | 10000000 |   32 | 0.074502 | 0.074878 |         1 |      10 |       pipe organ |
|  wolfsort | 10000000 |   32 | 0.078383 | 0.078891 |         1 |      10 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.010252 | 0.010461 |         1 |      10 | descending order |
|  gridsort | 10000000 |   32 | 0.031824 | 0.032061 |         1 |      10 | descending order |
|  quadsort | 10000000 |   32 | 0.010015 | 0.010226 |         1 |      10 | descending order |
|  wolfsort | 10000000 |   32 | 0.010319 | 0.011173 |         1 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.105708 | 0.106228 |         1 |      10 |   descending saw |
|  gridsort | 10000000 |   32 | 0.088798 | 0.089277 |         1 |      10 |   descending saw |
|  quadsort | 10000000 |   32 | 0.102057 | 0.102854 |         1 |      10 |   descending saw |
|  wolfsort | 10000000 |   32 | 0.177001 | 0.177721 |         1 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.168678 | 0.170819 |         1 |      10 |      random tail |
|  gridsort | 10000000 |   32 | 0.128310 | 0.128682 |         1 |      10 |      random tail |
|  quadsort | 10000000 |   32 | 0.164986 | 0.165363 |         1 |      10 |      random tail |
|  wolfsort | 10000000 |   32 | 0.180367 | 0.180880 |         1 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.259989 | 0.260365 |         1 |      10 |      random half |
|  gridsort | 10000000 |   32 | 0.218096 | 0.218445 |         1 |      10 |      random half |
|  quadsort | 10000000 |   32 | 0.256074 | 0.256747 |         1 |      10 |      random half |
|  wolfsort | 10000000 |   32 | 0.225742 | 0.226761 |         1 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.190243 | 0.202370 |         1 |      10 |  ascending tiles |
|  gridsort | 10000000 |   32 | 0.126200 | 0.126526 |         1 |      10 |  ascending tiles |
|  quadsort | 10000000 |   32 | 0.166375 | 0.168735 |         1 |      10 |  ascending tiles |
|  wolfsort | 10000000 |   32 | 0.189927 | 0.196767 |         1 |      10 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.238367 | 0.246001 |         1 |      10 |     bit reversal |
|  gridsort | 10000000 |   32 | 0.314635 | 0.315373 |         1 |      10 |     bit reversal |
|  quadsort | 10000000 |   32 | 0.393338 | 0.394130 |         1 |      10 |     bit reversal |
|  wolfsort | 10000000 |   32 | 0.277224 | 0.278359 |         1 |      10 |     bit reversal |

</details>


blitsort vs crumsort vs pdqsort vs wolfsort on 100K elements
-------------------------------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using g++ -O3 -fpermissive bench.c. All comparisons are inlined through the cmp macro.
A table with the best and average time in seconds can be uncollapsed below the bar graph.

Blitsort uses 512 elements of auxiliary memory, crumsort 512, pdqsort 64, and wolfsort 100000.
![Graph](/images/graph3.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  blitsort |   100000 |   64 | 0.002385 | 0.002498 |         1 |     100 |     random order |
|  crumsort |   100000 |   64 | 0.001879 | 0.001905 |         1 |     100 |     random order |
|   pdqsort |   100000 |   64 | 0.002690 | 0.002711 |         1 |     100 |     random order |
|  wolfsort |   100000 |   64 | 0.001900 | 0.001924 |         1 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  blitsort |   100000 |   32 | 0.002236 | 0.002328 |         1 |     100 |     random order |
|  crumsort |   100000 |   32 | 0.001811 | 0.001834 |         1 |     100 |     random order |
|   pdqsort |   100000 |   32 | 0.002692 | 0.002714 |         1 |     100 |     random order |
|  wolfsort |   100000 |   32 | 0.001117 | 0.001126 |         1 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.001067 | 0.001126 |         1 |     100 |     random % 100 |
|  crumsort |   100000 |   32 | 0.000615 | 0.000632 |         1 |     100 |     random % 100 |
|   pdqsort |   100000 |   32 | 0.000774 | 0.000785 |         1 |     100 |     random % 100 |
|  wolfsort |   100000 |   32 | 0.000676 | 0.000694 |         1 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.000046 | 0.000048 |         1 |     100 |  ascending order |
|  crumsort |   100000 |   32 | 0.000046 | 0.000046 |         1 |     100 |  ascending order |
|   pdqsort |   100000 |   32 | 0.000084 | 0.000087 |         1 |     100 |  ascending order |
|  wolfsort |   100000 |   32 | 0.000052 | 0.000052 |         1 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.000989 | 0.001000 |         1 |     100 |    ascending saw |
|  crumsort |   100000 |   32 | 0.000989 | 0.000998 |         1 |     100 |    ascending saw |
|   pdqsort |   100000 |   32 | 0.003357 | 0.003386 |         1 |     100 |    ascending saw |
|  wolfsort |   100000 |   32 | 0.001039 | 0.001046 |         1 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.000479 | 0.000484 |         1 |     100 |       pipe organ |
|  crumsort |   100000 |   32 | 0.000478 | 0.000496 |         1 |     100 |       pipe organ |
|   pdqsort |   100000 |   32 | 0.002847 | 0.002871 |         1 |     100 |       pipe organ |
|  wolfsort |   100000 |   32 | 0.000375 | 0.000379 |         1 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.000057 | 0.000057 |         1 |     100 | descending order |
|  crumsort |   100000 |   32 | 0.000057 | 0.000057 |         1 |     100 | descending order |
|   pdqsort |   100000 |   32 | 0.000187 | 0.000191 |         1 |     100 | descending order |
|  wolfsort |   100000 |   32 | 0.000062 | 0.000065 |         1 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.000991 | 0.001001 |         1 |     100 |   descending saw |
|  crumsort |   100000 |   32 | 0.000992 | 0.001002 |         1 |     100 |   descending saw |
|   pdqsort |   100000 |   32 | 0.003345 | 0.003370 |         1 |     100 |   descending saw |
|  wolfsort |   100000 |   32 | 0.001040 | 0.001046 |         1 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.001257 | 0.001275 |         1 |     100 |      random tail |
|  crumsort |   100000 |   32 | 0.001256 | 0.001270 |         1 |     100 |      random tail |
|   pdqsort |   100000 |   32 | 0.002585 | 0.002607 |         1 |     100 |      random tail |
|  wolfsort |   100000 |   32 | 0.001017 | 0.001028 |         1 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.002146 | 0.002205 |         1 |     100 |      random half |
|  crumsort |   100000 |   32 | 0.001832 | 0.001851 |         1 |     100 |      random half |
|   pdqsort |   100000 |   32 | 0.002676 | 0.002700 |         1 |     100 |      random half |
|  wolfsort |   100000 |   32 | 0.001107 | 0.001120 |         1 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.001366 | 0.001378 |         1 |     100 |  ascending tiles |
|  crumsort |   100000 |   32 | 0.001392 | 0.001411 |         1 |     100 |  ascending tiles |
|   pdqsort |   100000 |   32 | 0.002323 | 0.002346 |         1 |     100 |  ascending tiles |
|  wolfsort |   100000 |   32 | 0.001154 | 0.001163 |         1 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.002049 | 0.002151 |         1 |     100 |     bit reversal |
|  crumsort |   100000 |   32 | 0.001788 | 0.001814 |         1 |     100 |     bit reversal |
|   pdqsort |   100000 |   32 | 0.002669 | 0.002691 |         1 |     100 |     bit reversal |
|  wolfsort |   100000 |   32 | 0.001404 | 0.001412 |         1 |     100 |     bit reversal |

</details>

blitsort vs crumsort vs pdqsort vs wolfsort on 10M elements
-----------------------------------------------------------
Blitsort uses 512 elements of auxiliary memory, crumsort 512, pdqsort 64, and wolfsort 100000000.

![Graph](/images/graph4.png)
<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  blitsort | 10000000 |   32 | 0.431147 | 0.446532 |         1 |      10 |     random order |
|  crumsort | 10000000 |   32 | 0.235650 | 0.239112 |         1 |      10 |     random order |
|   pdqsort | 10000000 |   32 | 0.341341 | 0.343518 |         1 |      10 |     random order |
|  wolfsort | 10000000 |   32 | 0.230182 | 0.234245 |         1 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.222452 | 0.229082 |         1 |      10 |     random % 100 |
|  crumsort | 10000000 |   32 | 0.067306 | 0.067984 |         1 |      10 |     random % 100 |
|   pdqsort | 10000000 |   32 | 0.080865 | 0.081336 |         1 |      10 |     random % 100 |
|  wolfsort | 10000000 |   32 | 0.097294 | 0.099145 |         1 |      10 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.007126 | 0.007305 |         1 |      10 |  ascending order |
|  crumsort | 10000000 |   32 | 0.007130 | 0.007358 |         1 |      10 |  ascending order |
|   pdqsort | 10000000 |   32 | 0.011447 | 0.011598 |         1 |      10 |  ascending order |
|  wolfsort | 10000000 |   32 | 0.007215 | 0.007344 |         1 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.164120 | 0.164952 |         1 |      10 |    ascending saw |
|  crumsort | 10000000 |   32 | 0.164057 | 0.164723 |         1 |      10 |    ascending saw |
|   pdqsort | 10000000 |   32 | 0.493845 | 0.494221 |         1 |      10 |    ascending saw |
|  wolfsort | 10000000 |   32 | 0.183191 | 0.183485 |         1 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.083955 | 0.084424 |         1 |      10 |       pipe organ |
|  crumsort | 10000000 |   32 | 0.083940 | 0.084308 |         1 |      10 |       pipe organ |
|   pdqsort | 10000000 |   32 | 0.371342 | 0.372002 |         1 |      10 |       pipe organ |
|  wolfsort | 10000000 |   32 | 0.081505 | 0.082062 |         1 |      10 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.010336 | 0.010594 |         1 |      10 | descending order |
|  crumsort | 10000000 |   32 | 0.010348 | 0.010538 |         1 |      10 | descending order |
|   pdqsort | 10000000 |   32 | 0.022450 | 0.023052 |         1 |      10 | descending order |
|  wolfsort | 10000000 |   32 | 0.010451 | 0.011243 |         1 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.162735 | 0.163629 |         1 |      10 |   descending saw |
|  crumsort | 10000000 |   32 | 0.162815 | 0.163586 |         1 |      10 |   descending saw |
|   pdqsort | 10000000 |   32 | 0.602871 | 0.603748 |         1 |      10 |   descending saw |
|  wolfsort | 10000000 |   32 | 0.183247 | 0.183620 |         1 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.244109 | 0.245424 |         1 |      10 |      random tail |
|  crumsort | 10000000 |   32 | 0.244336 | 0.245258 |         1 |      10 |      random tail |
|   pdqsort | 10000000 |   32 | 0.327116 | 0.327682 |         1 |      10 |      random tail |
|  wolfsort | 10000000 |   32 | 0.187413 | 0.187889 |         1 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.369266 | 0.371572 |         1 |      10 |      random half |
|  crumsort | 10000000 |   32 | 0.238968 | 0.239880 |         1 |      10 |      random half |
|   pdqsort | 10000000 |   32 | 0.336266 | 0.336881 |         1 |      10 |      random half |
|  wolfsort | 10000000 |   32 | 0.233998 | 0.234678 |         1 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.220400 | 0.222216 |         1 |      10 |  ascending tiles |
|  crumsort | 10000000 |   32 | 0.192702 | 0.193740 |         1 |      10 |  ascending tiles |
|   pdqsort | 10000000 |   32 | 0.280164 | 0.280676 |         1 |      10 |  ascending tiles |
|  wolfsort | 10000000 |   32 | 0.184983 | 0.196855 |         1 |      10 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.416927 | 0.428903 |         1 |      10 |     bit reversal |
|  crumsort | 10000000 |   32 | 0.233557 | 0.234727 |         1 |      10 |     bit reversal |
|   pdqsort | 10000000 |   32 | 0.336091 | 0.336796 |         1 |      10 |     bit reversal |
|  wolfsort | 10000000 |   32 | 0.261455 | 0.262815 |         1 |      10 |     bit reversal |

</details>
