Intro
-----

This document describes a stable adaptive hybrid bucket / quick / merge / drop sort named wolfsort.
The bucket sort, forming the core of wolfsort, is not a comparison sort, so wolfsort can be considered
a member of the radix-sort family. Quicksort and mergesort are well known. Dropsort gained popularity
after it was reinvented as Stalin sort. A [benchmark](https://github.com/scandum/wolfsort#benchmark-for-wolfsort-v1154-dripsort) is available at the bottom.

Why a hybrid?
-------------
While an adaptive merge sort is very fast at sorting ordered data, its inability to effectively
partition is its greatest weakness. A radix-like bucket sort, on the other hand, is unable to take advantage of
sorted data. While quicksort is fast at partitioning, a bucket sort is faster on medium-sized
arrays in the 1K - 1M element range. Dropsort in turn hybridizes surprisingly well with bucket
and sample sorts.

History
-------
Wolfsort 1, codename: quantumsort, started out with the concept that memory is in abundance on
modern systems. I theorized that by allocating 8n memory performance could be increased by allowing
a bucket sort to partition in one pass.

Not all the memory would be used or ever accessed however, which is why I envisioned it as a type
of poor-man's quantum computing. The extra memory only serves to simplify computations. The concept
kind of worked, except that large memory allocations in C can be either very fast or very slow. I
didn't investigate why.

I also learned people don't like it when you use the term quantum computing outside of the proper
context, or perhaps they were upset about wolfsort's voracious appetite for memory. Hence it was named.

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
For optimal performance wolfsort needs to have at least 8 buckets, end up with between 1 and 16 elements
per bucket, so the bucket size is set to hold 8 elements on average. However, the buckets should remain
in the L1 cache, so the maximum number of buckets is set at 65536.

This sets the optimal range for wolfsort between 8 * 8 (64) and 8 * 65536 (524,288) elements. Beyond
the optimal range performance will degrade steadily. Once the average bucket size reaches the threshold
of 18 elements (1,179,648 total elements) the sort becomes less optimal than quicksort, though it retains
a computational advantage for a little while longer. However, by recursing once, wolfsort increases the
optimal range to 1 trillion elements.

By computing the minimum and maximum value in the data distribution, the number of buckets are optimized
further to target the sweet spot.

Dropsort
--------
Dropsort was first proposed as an alternative sorting algorithm by David Morgan in 2006, it makes one pass
and is lossy. The algorithm was reinvented in 2018 as Stalin sort. The concept of dropping hash entries in
a non-lossy manner was independently developed by Marshall Lochbaum in 2018 and is utilized in his 2022
release of rhsort (Robin Hood Sort).

Wolfsort allocates 4n memory to allow some deviancy in the data distribution and minimize bucket overflow.
In the case an element is too deviant and overflows the bucket, it is copied in-place to the input
array. In near-optimal cases this results in a minimal drip, in the worst case it will result in a downpour
of elements being copied to the input array.

While a centrally planned partitioning system has its weaknesses, the worst case is mostly alleviated by using
fluxsort on the deviant elements once partitioning finishes. Fluxsort is adaptive and is generally
strong against distributions where wolfsort is weak.

The overall performance gain from incorporating dropsort into wolfsort is approximately 20%, but can reach
an order of magnitude when the fallback is synergetic with fluxsort. Deviant distributions can deceive
wolfsort for a time, but not a very long time.

Small number sorting
--------------------
Since wolfsort uses auxiliary memory, each partition is stable once partitioning completes. The next
step is to sort the content of each bucket using fluxsort. If the number of elements in a bucket is
below 32, fluxsort defaults to quadsort, which is highly optimized for sorting small arrays using a
combination of branchless parity merges and twice-unguarded insertion.

Once each bucket is sorted, all that remains is merging the two distributions of compliant and deviant
elements, and wolfsort is finished.

Memory overhead
---------------
Wolfsort requires 4n memory for the partitioning process and n / 4 memory (up to a maximum of 65536)
for the buckets.

If not enough memory is available wolfsort falls back on fluxsort, which requires exactly 1n swap memory,
and if that's not sufficient fluxsort falls back on quadsort which can sort in-place. It is an
option to fall back on blitsort instead of quadsort, but since this would be an a-typical case,
and increase dependencies, I didn't implement this.

64 bit integers
---------------
With the advent of fluxsort and crumsort the dominance of radix sorts has been pushed out of 64 bit territory. Increased memory-level-parallelism in future hardware, or algorithmic optimizations, might make radix sorts competitive again for 64 bit types. Wolfsort has a commented-out default to fluxsort.

128 bit floats
--------------
Wolfsort defaults to fluxsort for 128 bit floats. Keep in mind that in the real world you'll typically be sorting tables instead of arrays, so the benchmark isn't indicative of real world performance, as the sort will likely be copying 64 bit pointers instead of 128 bit floats.

God Mode
--------
Wolfsort supports a cheat mode where the sort becomes unstable. This trick was taken from rhsort. Since wolfsort aspires to have some utility as a stable sort, this method is disabled by default, including in the benchmark.

In the benchmark rhsort does use this optimization, but it's only relevant for the random % 100 distribution. For 32 bit random integers rhsort easily beats wolfsort without an unfair advantage.

LLVM
----
When compiling with Clang, quadsort and fluxsort will take advantate of branchless ternary oprations, which gives a 15-30% performance gain. While not an algorithmic improvement, it's relevant to keep in mind, particularly when it comes to LLVM compiled Rust sorts with similar optimizations.

Interface
---------
Wolfsort uses the same interface as qsort, which is described in [man qsort](https://man7.org/linux/man-pages/man3/qsort.3p.html).

Wolfsort also comes with the `wolfsort_prim(void *array, size_t nmemb, size_t size)` function to perform primitive comparisons on arrays of 32 and 64 bit integers. Nmemb is the number of elements, while size should be either `sizeof(int)` or `sizeof(long long)` for signed integers, and `sizeof(int) + 1` or `sizeof(long long) + 1` for unsigned integers. Support for the char and short types can be easily added in wolfsort.h.

Wolfsort can only sort arrays of primitive integers by default. Wolfsort should be able to sort tables with some minor changes, but it'll require a different interface than qsort() provides.

Proof of concept
----------------
Wolfsort is primarily a proof of concept for a hybrid bucket / comparison sort. It only supports non-negative integers.

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
--------
[Piposort](https://github.com/scandum/piposort) is a simplified branchless quadsort with a much smaller code size and complexity while still being very fast. Piposort might be of use to people who want to port quadsort. This is a lot easier when you start out small.

rhsort
------
[rhsort](https://github.com/mlochbaum/rhsort) is a hybrid stable out-of-place counting / radix / drop / insertion sort. It has exceptional performance on random and generic data for medium array sizes.

Ska sort
--------
[Ska sort](https://github.com/skarupke/ska_sort) is an advanced radix sort that can sort strings and floats as well. It offers both an in-place and out-of-place version, but since the out-of-place unstable version is not very competitive with wolfsort, I only benchmark the stable and faster ska_sort_copy variant.

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

Benchmark for Wolfsort v1.2.1.3
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
|  wolfsort |   100000 |   64 | 0.003006 | 0.003063 |         0 |     100 |     random order |
|   skasort |   100000 |   64 | 0.001818 | 0.001842 |         0 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|    rhsort |   100000 |   32 | 0.000706 | 0.000729 |         0 |     100 |     random order |
|  wolfsort |   100000 |   32 | 0.001000 | 0.001026 |         0 |     100 |     random order |
|   skasort |   100000 |   32 | 0.000626 | 0.000640 |         0 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000115 | 0.000118 |         0 |     100 |     random % 100 |
|  wolfsort |   100000 |   32 | 0.000376 | 0.000382 |         0 |     100 |     random % 100 |
|   skasort |   100000 |   32 | 0.000780 | 0.000793 |         0 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000302 | 0.000317 |         0 |     100 |  ascending order |
|  wolfsort |   100000 |   32 | 0.000086 | 0.000088 |         0 |     100 |  ascending order |
|   skasort |   100000 |   32 | 0.000709 | 0.000720 |         0 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000615 | 0.000633 |         0 |     100 |    ascending saw |
|  wolfsort |   100000 |   32 | 0.000379 | 0.000407 |         0 |     100 |    ascending saw |
|   skasort |   100000 |   32 | 0.000624 | 0.000637 |         0 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000591 | 0.000615 |         0 |     100 |       pipe organ |
|  wolfsort |   100000 |   32 | 0.000248 | 0.000258 |         0 |     100 |       pipe organ |
|   skasort |   100000 |   32 | 0.000624 | 0.000639 |         0 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000400 | 0.000420 |         0 |     100 | descending order |
|  wolfsort |   100000 |   32 | 0.000097 | 0.000101 |         0 |     100 | descending order |
|   skasort |   100000 |   32 | 0.000684 | 0.000693 |         0 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000612 | 0.000629 |         0 |     100 |   descending saw |
|  wolfsort |   100000 |   32 | 0.000389 | 0.000393 |         0 |     100 |   descending saw |
|   skasort |   100000 |   32 | 0.000627 | 0.000639 |         0 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000633 | 0.000664 |         0 |     100 |      random tail |
|  wolfsort |   100000 |   32 | 0.000467 | 0.000473 |         0 |     100 |      random tail |
|   skasort |   100000 |   32 | 0.000622 | 0.000636 |         0 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000671 | 0.000685 |         0 |     100 |      random half |
|  wolfsort |   100000 |   32 | 0.000689 | 0.000706 |         0 |     100 |      random half |
|   skasort |   100000 |   32 | 0.000628 | 0.000641 |         0 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.002019 | 0.002052 |         0 |     100 |  ascending tiles |
|  wolfsort |   100000 |   32 | 0.000683 | 0.000691 |         0 |     100 |  ascending tiles |
|   skasort |   100000 |   32 | 0.001096 | 0.001113 |         0 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000837 | 0.000871 |         0 |     100 |     bit reversal |
|  wolfsort |   100000 |   32 | 0.000887 | 0.000928 |         0 |     100 |     bit reversal |
|   skasort |   100000 |   32 | 0.000775 | 0.000782 |         0 |     100 |     bit reversal |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000118 | 0.000123 |         0 |     100 |       random % 4 |
|  wolfsort |   100000 |   32 | 0.000368 | 0.000371 |         0 |     100 |       random % 4 |
|   skasort |   100000 |   32 | 0.000785 | 0.000809 |         0 |     100 |       random % 4 |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.001278 | 0.001465 |         0 |     100 |      semi random |
|  wolfsort |   100000 |   32 | 0.000792 | 0.000811 |         0 |     100 |      semi random |
|   skasort |   100000 |   32 | 0.000805 | 0.000821 |         0 |     100 |      semi random |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.000198 | 0.000202 |         0 |     100 |    random signal |
|  wolfsort |   100000 |   32 | 0.000815 | 0.000829 |         0 |     100 |    random signal |
|   skasort |   100000 |   32 | 0.001099 | 0.001118 |         0 |     100 |    random signal |

</details>

The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. It measures the performance on random data with array sizes
ranging from 10 to 10,000,000. It's generated by running the benchmark using 10000000 0 0 as the argument. The benchmark is weighted, meaning the number of repetitions
halves each time the number of items doubles. A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/radix2.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|    rhsort |       10 |   32 | 0.135095 | 0.137011 |       0.0 |      10 |        random 10 |
|  wolfsort |       10 |   32 | 0.052087 | 0.052986 |       0.0 |      10 |        random 10 |
|   skasort |       10 |   32 | 0.099853 | 0.100198 |       0.0 |      10 |        random 10 |
|           |          |      |          |          |           |         |                  |
|    rhsort |      100 |   32 | 0.069252 | 0.070421 |       0.0 |      10 |       random 100 |
|  wolfsort |      100 |   32 | 0.132208 | 0.132824 |       0.0 |      10 |       random 100 |
|   skasort |      100 |   32 | 0.232007 | 0.232507 |       0.0 |      10 |       random 100 |
|           |          |      |          |          |           |         |                  |
|    rhsort |     1000 |   32 | 0.055916 | 0.056130 |       0.0 |      10 |      random 1000 |
|  wolfsort |     1000 |   32 | 0.101611 | 0.101913 |       0.0 |      10 |      random 1000 |
|   skasort |     1000 |   32 | 0.054757 | 0.055050 |       0.0 |      10 |      random 1000 |
|           |          |      |          |          |           |         |                  |
|    rhsort |    10000 |   32 | 0.057062 | 0.057359 |       0.0 |      10 |     random 10000 |
|  wolfsort |    10000 |   32 | 0.118598 | 0.119373 |       0.0 |      10 |     random 10000 |
|   skasort |    10000 |   32 | 0.059786 | 0.060189 |       0.0 |      10 |     random 10000 |
|           |          |      |          |          |           |         |                  |
|    rhsort |   100000 |   32 | 0.071273 | 0.073310 |       0.0 |      10 |    random 100000 |
|  wolfsort |   100000 |   32 | 0.102639 | 0.103917 |       0.0 |      10 |    random 100000 |
|   skasort |   100000 |   32 | 0.064120 | 0.064615 |       0.0 |      10 |    random 100000 |
|           |          |      |          |          |           |         |                  |
|    rhsort |  1000000 |   32 | 0.181059 | 0.187563 |       0.0 |      10 |   random 1000000 |
|  wolfsort |  1000000 |   32 | 0.146630 | 0.147598 |       0.0 |      10 |   random 1000000 |
|   skasort |  1000000 |   32 | 0.070250 | 0.071571 |       0.0 |      10 |   random 1000000 |
|           |          |      |          |          |           |         |                  |
|    rhsort | 10000000 |   32 | 0.412107 | 0.425066 |         0 |      10 |  random 10000000 |
|  wolfsort | 10000000 |   32 | 0.193120 | 0.200947 |         0 |      10 |  random 10000000 |
|   skasort | 10000000 |   32 | 0.115721 | 0.116621 |         0 |      10 |  random 10000000 |

</details>

Benchmark for Wolfsort v1.2.1.3
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
|  fluxsort |   100000 |  128 | 0.008328 | 0.008424 |         0 |     100 |     random order |
|  gridsort |   100000 |  128 | 0.007823 | 0.007932 |         0 |     100 |     random order |
|  quadsort |   100000 |  128 | 0.008260 | 0.008353 |         0 |     100 |     random order |
|  wolfsort |   100000 |  128 | 0.008330 | 0.008415 |         0 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  fluxsort |   100000 |   64 | 0.001971 | 0.001991 |         0 |     100 |     random order |
|  gridsort |   100000 |   64 | 0.002370 | 0.002398 |         0 |     100 |     random order |
|  quadsort |   100000 |   64 | 0.002230 | 0.002254 |         0 |     100 |     random order |
|  wolfsort |   100000 |   64 | 0.003023 | 0.003068 |         0 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  fluxsort |   100000 |   32 | 0.001868 | 0.001901 |         0 |     100 |     random order |
|  gridsort |   100000 |   32 | 0.002324 | 0.002357 |         0 |     100 |     random order |
|  quadsort |   100000 |   32 | 0.002149 | 0.002174 |         0 |     100 |     random order |
|  wolfsort |   100000 |   32 | 0.000988 | 0.001019 |         0 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000733 | 0.000740 |         0 |     100 |     random % 100 |
|  gridsort |   100000 |   32 | 0.001921 | 0.001941 |         0 |     100 |     random % 100 |
|  quadsort |   100000 |   32 | 0.001627 | 0.001645 |         0 |     100 |     random % 100 |
|  wolfsort |   100000 |   32 | 0.000374 | 0.000378 |         0 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000043 | 0.000044 |         0 |     100 |  ascending order |
|  gridsort |   100000 |   32 | 0.000264 | 0.000271 |         0 |     100 |  ascending order |
|  quadsort |   100000 |   32 | 0.000052 | 0.000053 |         0 |     100 |  ascending order |
|  wolfsort |   100000 |   32 | 0.000087 | 0.000089 |         0 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000305 | 0.000314 |         0 |     100 |    ascending saw |
|  gridsort |   100000 |   32 | 0.000621 | 0.000641 |         0 |     100 |    ascending saw |
|  quadsort |   100000 |   32 | 0.000411 | 0.000417 |         0 |     100 |    ascending saw |
|  wolfsort |   100000 |   32 | 0.000379 | 0.000384 |         0 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000193 | 0.000203 |         0 |     100 |       pipe organ |
|  gridsort |   100000 |   32 | 0.000446 | 0.000486 |         0 |     100 |       pipe organ |
|  quadsort |   100000 |   32 | 0.000252 | 0.000260 |         0 |     100 |       pipe organ |
|  wolfsort |   100000 |   32 | 0.000248 | 0.000259 |         0 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000054 | 0.000055 |         0 |     100 | descending order |
|  gridsort |   100000 |   32 | 0.000284 | 0.000295 |         0 |     100 | descending order |
|  quadsort |   100000 |   32 | 0.000068 | 0.000070 |         0 |     100 | descending order |
|  wolfsort |   100000 |   32 | 0.000097 | 0.000100 |         0 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000315 | 0.000325 |         0 |     100 |   descending saw |
|  gridsort |   100000 |   32 | 0.000652 | 0.000667 |         0 |     100 |   descending saw |
|  quadsort |   100000 |   32 | 0.000440 | 0.000446 |         0 |     100 |   descending saw |
|  wolfsort |   100000 |   32 | 0.000389 | 0.000393 |         0 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000607 | 0.000619 |         0 |     100 |      random tail |
|  gridsort |   100000 |   32 | 0.000847 | 0.000860 |         0 |     100 |      random tail |
|  quadsort |   100000 |   32 | 0.000685 | 0.000694 |         0 |     100 |      random tail |
|  wolfsort |   100000 |   32 | 0.000464 | 0.000471 |         0 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.001074 | 0.001081 |         0 |     100 |      random half |
|  gridsort |   100000 |   32 | 0.001332 | 0.001355 |         0 |     100 |      random half |
|  quadsort |   100000 |   32 | 0.001230 | 0.001243 |         0 |     100 |      random half |
|  wolfsort |   100000 |   32 | 0.000686 | 0.000696 |         0 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000317 | 0.000324 |         0 |     100 |  ascending tiles |
|  gridsort |   100000 |   32 | 0.000665 | 0.000693 |         0 |     100 |  ascending tiles |
|  quadsort |   100000 |   32 | 0.000789 | 0.000802 |         0 |     100 |  ascending tiles |
|  wolfsort |   100000 |   32 | 0.000686 | 0.000693 |         0 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.001714 | 0.001751 |         0 |     100 |     bit reversal |
|  gridsort |   100000 |   32 | 0.002045 | 0.002060 |         0 |     100 |     bit reversal |
|  quadsort |   100000 |   32 | 0.002083 | 0.002100 |         0 |     100 |     bit reversal |
|  wolfsort |   100000 |   32 | 0.000888 | 0.000912 |         0 |     100 |     bit reversal |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.000215 | 0.000223 |         0 |     100 |       random % 4 |
|  gridsort |   100000 |   32 | 0.001283 | 0.001305 |         0 |     100 |       random % 4 |
|  quadsort |   100000 |   32 | 0.001080 | 0.001090 |         0 |     100 |       random % 4 |
|  wolfsort |   100000 |   32 | 0.000369 | 0.000371 |         0 |     100 |       random % 4 |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.001072 | 0.001098 |         0 |     100 |      semi random |
|  gridsort |   100000 |   32 | 0.001355 | 0.001377 |         0 |     100 |      semi random |
|  quadsort |   100000 |   32 | 0.001062 | 0.001074 |         0 |     100 |      semi random |
|  wolfsort |   100000 |   32 | 0.000789 | 0.000803 |         0 |     100 |      semi random |
|           |          |      |          |          |           |         |                  |
|  fluxsort |   100000 |   32 | 0.001079 | 0.001099 |         0 |     100 |    random signal |
|  gridsort |   100000 |   32 | 0.001296 | 0.001306 |         0 |     100 |    random signal |
|  quadsort |   100000 |   32 | 0.001014 | 0.001027 |         0 |     100 |    random signal |
|  wolfsort |   100000 |   32 | 0.000816 | 0.000828 |         0 |     100 |    random signal |

</details>

fluxsort vs gridsort vs quadsort vs wolfsort on 10M elements
------------------------------------------------------------

![Graph](/images/graph2.png)
<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  fluxsort | 10000000 |  128 | 1.242395 | 1.264809 |         0 |      10 |     random order |
|  gridsort | 10000000 |  128 | 1.048748 | 1.110490 |         0 |      10 |     random order |
|  quadsort | 10000000 |  128 | 1.407639 | 1.418088 |         0 |      10 |     random order |
|  wolfsort | 10000000 |  128 | 1.239099 | 1.241608 |         0 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  fluxsort | 10000000 |   64 | 0.317327 | 0.318203 |         0 |      10 |     random order |
|  gridsort | 10000000 |   64 | 0.332430 | 0.334392 |         0 |      10 |     random order |
|  quadsort | 10000000 |   64 | 0.438257 | 0.439139 |         0 |      10 |     random order |
|  wolfsort | 10000000 |   64 | 0.481977 | 0.484055 |         0 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  fluxsort | 10000000 |   32 | 0.269351 | 0.271460 |         0 |      10 |     random order |
|  gridsort | 10000000 |   32 | 0.322099 | 0.323899 |         0 |      10 |     random order |
|  quadsort | 10000000 |   32 | 0.364457 | 0.365617 |         0 |      10 |     random order |
|  wolfsort | 10000000 |   32 | 0.189780 | 0.190911 |         0 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.089973 | 0.090849 |         0 |      10 |     random % 100 |
|  gridsort | 10000000 |   32 | 0.172222 | 0.173147 |         0 |      10 |     random % 100 |
|  quadsort | 10000000 |   32 | 0.248361 | 0.250615 |         0 |      10 |     random % 100 |
|  wolfsort | 10000000 |   32 | 0.086473 | 0.087067 |         0 |      10 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.006437 | 0.006574 |         0 |      10 |  ascending order |
|  gridsort | 10000000 |   32 | 0.032321 | 0.032798 |         0 |      10 |  ascending order |
|  quadsort | 10000000 |   32 | 0.011736 | 0.012125 |         0 |      10 |  ascending order |
|  wolfsort | 10000000 |   32 | 0.010888 | 0.011015 |         0 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.074940 | 0.075525 |         0 |      10 |    ascending saw |
|  gridsort | 10000000 |   32 | 0.067478 | 0.067893 |         0 |      10 |    ascending saw |
|  quadsort | 10000000 |   32 | 0.097133 | 0.098004 |         0 |      10 |    ascending saw |
|  wolfsort | 10000000 |   32 | 0.081797 | 0.082794 |         0 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.064577 | 0.065593 |         0 |      10 |       pipe organ |
|  gridsort | 10000000 |   32 | 0.048932 | 0.049336 |         0 |      10 |       pipe organ |
|  quadsort | 10000000 |   32 | 0.082533 | 0.083781 |         0 |      10 |       pipe organ |
|  wolfsort | 10000000 |   32 | 0.070334 | 0.071158 |         0 |      10 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.009807 | 0.010104 |         0 |      10 | descending order |
|  gridsort | 10000000 |   32 | 0.034583 | 0.034814 |         0 |      10 | descending order |
|  quadsort | 10000000 |   32 | 0.011396 | 0.011639 |         0 |      10 | descending order |
|  wolfsort | 10000000 |   32 | 0.014198 | 0.014544 |         0 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.078279 | 0.079071 |         0 |      10 |   descending saw |
|  gridsort | 10000000 |   32 | 0.069702 | 0.070109 |         0 |      10 |   descending saw |
|  quadsort | 10000000 |   32 | 0.101826 | 0.102801 |         0 |      10 |   descending saw |
|  wolfsort | 10000000 |   32 | 0.085101 | 0.085973 |         0 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.121948 | 0.122561 |         0 |      10 |      random tail |
|  gridsort | 10000000 |   32 | 0.109341 | 0.110117 |         0 |      10 |      random tail |
|  quadsort | 10000000 |   32 | 0.153324 | 0.153797 |         0 |      10 |      random tail |
|  wolfsort | 10000000 |   32 | 0.103558 | 0.104152 |         0 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.181347 | 0.183186 |         0 |      10 |      random half |
|  gridsort | 10000000 |   32 | 0.185691 | 0.186592 |         0 |      10 |      random half |
|  quadsort | 10000000 |   32 | 0.225265 | 0.225897 |         0 |      10 |      random half |
|  wolfsort | 10000000 |   32 | 0.159819 | 0.160569 |         0 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.073673 | 0.074755 |         0 |      10 |  ascending tiles |
|  gridsort | 10000000 |   32 | 0.126309 | 0.126626 |         0 |      10 |  ascending tiles |
|  quadsort | 10000000 |   32 | 0.165332 | 0.167541 |         0 |      10 |  ascending tiles |
|  wolfsort | 10000000 |   32 | 0.093424 | 0.094040 |         0 |      10 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.271679 | 0.272589 |         0 |      10 |     bit reversal |
|  gridsort | 10000000 |   32 | 0.296563 | 0.297984 |         0 |      10 |     bit reversal |
|  quadsort | 10000000 |   32 | 0.369105 | 0.370652 |         0 |      10 |     bit reversal |
|  wolfsort | 10000000 |   32 | 0.251209 | 0.252891 |         0 |      10 |     bit reversal |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.056011 | 0.056552 |         0 |      10 |       random % 4 |
|  gridsort | 10000000 |   32 | 0.191179 | 0.194017 |         0 |      10 |       random % 4 |
|  quadsort | 10000000 |   32 | 0.192466 | 0.193967 |         0 |      10 |       random % 4 |
|  wolfsort | 10000000 |   32 | 0.081668 | 0.082543 |         0 |      10 |       random % 4 |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.054231 | 0.054571 |         0 |      10 |      semi random |
|  gridsort | 10000000 |   32 | 0.146534 | 0.146907 |         0 |      10 |      semi random |
|  quadsort | 10000000 |   32 | 0.197462 | 0.200010 |         0 |      10 |      semi random |
|  wolfsort | 10000000 |   32 | 0.192603 | 0.194365 |         0 |      10 |      semi random |
|           |          |      |          |          |           |         |                  |
|  fluxsort | 10000000 |   32 | 0.173080 | 0.176575 |         0 |      10 |    random signal |
|  gridsort | 10000000 |   32 | 0.137590 | 0.137932 |         0 |      10 |    random signal |
|  quadsort | 10000000 |   32 | 0.180939 | 0.181778 |         0 |      10 |    random signal |
|  wolfsort | 10000000 |   32 | 0.161181 | 0.161714 |         0 |      10 |    random signal |

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
|  blitsort |   100000 |  128 | 0.010864 | 0.010994 |         0 |     100 |     random order |
|  crumsort |   100000 |  128 | 0.008143 | 0.008222 |         0 |     100 |     random order |
|   pdqsort |   100000 |  128 | 0.005954 | 0.006063 |         0 |     100 |     random order |
|  wolfsort |   100000 |  128 | 0.008308 | 0.008396 |         0 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  blitsort |   100000 |   64 | 0.002326 | 0.002354 |         0 |     100 |     random order |
|  crumsort |   100000 |   64 | 0.001835 | 0.001848 |         0 |     100 |     random order |
|   pdqsort |   100000 |   64 | 0.002752 | 0.002806 |         0 |     100 |     random order |
|  wolfsort |   100000 |   64 | 0.003014 | 0.003069 |         0 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  blitsort |   100000 |   32 | 0.002094 | 0.002117 |         0 |     100 |     random order |
|  crumsort |   100000 |   32 | 0.001764 | 0.001779 |         0 |     100 |     random order |
|   pdqsort |   100000 |   32 | 0.002747 | 0.002770 |         0 |     100 |     random order |
|  wolfsort |   100000 |   32 | 0.000983 | 0.001016 |         0 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.000880 | 0.000891 |         0 |     100 |     random % 100 |
|  crumsort |   100000 |   32 | 0.000602 | 0.000641 |         0 |     100 |     random % 100 |
|   pdqsort |   100000 |   32 | 0.000795 | 0.000805 |         0 |     100 |     random % 100 |
|  wolfsort |   100000 |   32 | 0.000376 | 0.000381 |         0 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.000043 | 0.000045 |         0 |     100 |  ascending order |
|  crumsort |   100000 |   32 | 0.000043 | 0.000044 |         0 |     100 |  ascending order |
|   pdqsort |   100000 |   32 | 0.000084 | 0.000088 |         0 |     100 |  ascending order |
|  wolfsort |   100000 |   32 | 0.000086 | 0.000088 |         0 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.000440 | 0.000450 |         0 |     100 |    ascending saw |
|  crumsort |   100000 |   32 | 0.000410 | 0.000419 |         0 |     100 |    ascending saw |
|   pdqsort |   100000 |   32 | 0.003222 | 0.003246 |         0 |     100 |    ascending saw |
|  wolfsort |   100000 |   32 | 0.000379 | 0.000382 |         0 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.000242 | 0.000251 |         0 |     100 |       pipe organ |
|  crumsort |   100000 |   32 | 0.000229 | 0.000243 |         0 |     100 |       pipe organ |
|   pdqsort |   100000 |   32 | 0.002842 | 0.002864 |         0 |     100 |       pipe organ |
|  wolfsort |   100000 |   32 | 0.000249 | 0.000257 |         0 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.000054 | 0.000055 |         0 |     100 | descending order |
|  crumsort |   100000 |   32 | 0.000054 | 0.000055 |         0 |     100 | descending order |
|   pdqsort |   100000 |   32 | 0.000190 | 0.000198 |         0 |     100 | descending order |
|  wolfsort |   100000 |   32 | 0.000097 | 0.000100 |         0 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.000452 | 0.000466 |         0 |     100 |   descending saw |
|  crumsort |   100000 |   32 | 0.000421 | 0.000431 |         0 |     100 |   descending saw |
|   pdqsort |   100000 |   32 | 0.004200 | 0.004245 |         0 |     100 |   descending saw |
|  wolfsort |   100000 |   32 | 0.000383 | 0.000402 |         0 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.000782 | 0.000829 |         0 |     100 |      random tail |
|  crumsort |   100000 |   32 | 0.000714 | 0.000755 |         0 |     100 |      random tail |
|   pdqsort |   100000 |   32 | 0.002638 | 0.002759 |         0 |     100 |      random tail |
|  wolfsort |   100000 |   32 | 0.000463 | 0.000483 |         0 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.001210 | 0.001275 |         0 |     100 |      random half |
|  crumsort |   100000 |   32 | 0.001063 | 0.001096 |         0 |     100 |      random half |
|   pdqsort |   100000 |   32 | 0.002738 | 0.002780 |         0 |     100 |      random half |
|  wolfsort |   100000 |   32 | 0.000685 | 0.000712 |         0 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.001105 | 0.001278 |         0 |     100 |  ascending tiles |
|  crumsort |   100000 |   32 | 0.001393 | 0.001435 |         0 |     100 |  ascending tiles |
|   pdqsort |   100000 |   32 | 0.002367 | 0.002398 |         0 |     100 |  ascending tiles |
|  wolfsort |   100000 |   32 | 0.000682 | 0.000689 |         0 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.001956 | 0.001988 |         0 |     100 |     bit reversal |
|  crumsort |   100000 |   32 | 0.001762 | 0.001794 |         0 |     100 |     bit reversal |
|   pdqsort |   100000 |   32 | 0.002731 | 0.002758 |         0 |     100 |     bit reversal |
|  wolfsort |   100000 |   32 | 0.000890 | 0.000921 |         0 |     100 |     bit reversal |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.000328 | 0.000341 |         0 |     100 |       random % 4 |
|  crumsort |   100000 |   32 | 0.000206 | 0.000216 |         0 |     100 |       random % 4 |
|   pdqsort |   100000 |   32 | 0.000382 | 0.000391 |         0 |     100 |       random % 4 |
|  wolfsort |   100000 |   32 | 0.000367 | 0.000378 |         0 |     100 |       random % 4 |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.001209 | 0.001244 |         0 |     100 |      semi random |
|  crumsort |   100000 |   32 | 0.000309 | 0.000319 |         0 |     100 |      semi random |
|   pdqsort |   100000 |   32 | 0.000479 | 0.000500 |         0 |     100 |      semi random |
|  wolfsort |   100000 |   32 | 0.000791 | 0.000828 |         0 |     100 |      semi random |
|           |          |      |          |          |           |         |                  |
|  blitsort |   100000 |   32 | 0.001893 | 0.001926 |         0 |     100 |    random signal |
|  crumsort |   100000 |   32 | 0.001714 | 0.001742 |         0 |     100 |    random signal |
|   pdqsort |   100000 |   32 | 0.002950 | 0.002976 |         0 |     100 |    random signal |
|  wolfsort |   100000 |   32 | 0.000814 | 0.000834 |         0 |     100 |    random signal |

</details>

blitsort vs crumsort vs pdqsort vs wolfsort on 10M elements
-----------------------------------------------------------
Blitsort uses 512 elements of auxiliary memory, crumsort 512, pdqsort 64, and wolfsort 100000000.

![Graph](/images/graph4.png)
<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  blitsort | 10000000 |  128 | 2.172622 | 2.191956 |         0 |      10 |     random order |
|  crumsort | 10000000 |  128 | 1.134328 | 1.135821 |         0 |      10 |     random order |
|   pdqsort | 10000000 |  128 | 0.805620 | 0.808041 |         0 |      10 |     random order |
|  wolfsort | 10000000 |  128 | 1.237174 | 1.238863 |         0 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  blitsort | 10000000 |   64 | 0.434356 | 0.443134 |         0 |      10 |     random order |
|  crumsort | 10000000 |   64 | 0.250065 | 0.251453 |         0 |      10 |     random order |
|   pdqsort | 10000000 |   64 | 0.359586 | 0.360388 |         0 |      10 |     random order |
|  wolfsort | 10000000 |   64 | 0.480904 | 0.482835 |         0 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|  blitsort | 10000000 |   32 | 0.332071 | 0.339524 |         0 |      10 |     random order |
|  crumsort | 10000000 |   32 | 0.231584 | 0.232056 |         0 |      10 |     random order |
|   pdqsort | 10000000 |   32 | 0.347793 | 0.348437 |         0 |      10 |     random order |
|  wolfsort | 10000000 |   32 | 0.189250 | 0.189762 |         0 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.126792 | 0.128439 |         0 |      10 |     random % 100 |
|  crumsort | 10000000 |   32 | 0.060683 | 0.061353 |         0 |      10 |     random % 100 |
|   pdqsort | 10000000 |   32 | 0.079284 | 0.079891 |         0 |      10 |     random % 100 |
|  wolfsort | 10000000 |   32 | 0.086577 | 0.087157 |         0 |      10 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.006581 | 0.006784 |         0 |      10 |  ascending order |
|  crumsort | 10000000 |   32 | 0.006690 | 0.006801 |         0 |      10 |  ascending order |
|   pdqsort | 10000000 |   32 | 0.011712 | 0.011851 |         0 |      10 |  ascending order |
|  wolfsort | 10000000 |   32 | 0.010958 | 0.011520 |         0 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.070514 | 0.071260 |         0 |      10 |    ascending saw |
|  crumsort | 10000000 |   32 | 0.064829 | 0.066035 |         0 |      10 |    ascending saw |
|   pdqsort | 10000000 |   32 | 0.560995 | 0.561774 |         0 |      10 |    ascending saw |
|  wolfsort | 10000000 |   32 | 0.081644 | 0.082279 |         0 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.041220 | 0.041924 |         0 |      10 |       pipe organ |
|  crumsort | 10000000 |   32 | 0.039335 | 0.040018 |         0 |      10 |       pipe organ |
|   pdqsort | 10000000 |   32 | 0.363633 | 0.364187 |         0 |      10 |       pipe organ |
|  wolfsort | 10000000 |   32 | 0.070536 | 0.071400 |         0 |      10 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.010271 | 0.010549 |         0 |      10 | descending order |
|  crumsort | 10000000 |   32 | 0.010254 | 0.010499 |         0 |      10 | descending order |
|   pdqsort | 10000000 |   32 | 0.023129 | 0.023708 |         0 |      10 | descending order |
|  wolfsort | 10000000 |   32 | 0.014583 | 0.015316 |         0 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.073410 | 0.074402 |         0 |      10 |   descending saw |
|  crumsort | 10000000 |   32 | 0.068284 | 0.069154 |         0 |      10 |   descending saw |
|   pdqsort | 10000000 |   32 | 0.942142 | 0.958606 |         0 |      10 |   descending saw |
|  wolfsort | 10000000 |   32 | 0.085338 | 0.086014 |         0 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.124089 | 0.130327 |         0 |      10 |      random tail |
|  crumsort | 10000000 |   32 | 0.103030 | 0.104337 |         0 |      10 |      random tail |
|   pdqsort | 10000000 |   32 | 0.337862 | 0.342594 |         0 |      10 |      random tail |
|  wolfsort | 10000000 |   32 | 0.103381 | 0.108048 |         0 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.191479 | 0.193036 |         0 |      10 |      random half |
|  crumsort | 10000000 |   32 | 0.146732 | 0.147742 |         0 |      10 |      random half |
|   pdqsort | 10000000 |   32 | 0.342803 | 0.343424 |         0 |      10 |      random half |
|  wolfsort | 10000000 |   32 | 0.159515 | 0.160787 |         0 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.182256 | 0.190378 |         0 |      10 |  ascending tiles |
|  crumsort | 10000000 |   32 | 0.188875 | 0.195063 |         0 |      10 |  ascending tiles |
|   pdqsort | 10000000 |   32 | 0.285777 | 0.286996 |         0 |      10 |  ascending tiles |
|  wolfsort | 10000000 |   32 | 0.093709 | 0.094315 |         0 |      10 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.324983 | 0.326345 |         0 |      10 |     bit reversal |
|  crumsort | 10000000 |   32 | 0.230872 | 0.231599 |         0 |      10 |     bit reversal |
|   pdqsort | 10000000 |   32 | 0.343915 | 0.344677 |         0 |      10 |     bit reversal |
|  wolfsort | 10000000 |   32 | 0.250331 | 0.251319 |         0 |      10 |     bit reversal |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.061197 | 0.062058 |         0 |      10 |       random % 4 |
|  crumsort | 10000000 |   32 | 0.030134 | 0.030564 |         0 |      10 |       random % 4 |
|   pdqsort | 10000000 |   32 | 0.043492 | 0.043673 |         0 |      10 |       random % 4 |
|  wolfsort | 10000000 |   32 | 0.081548 | 0.082020 |         0 |      10 |       random % 4 |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.066686 | 0.067764 |         0 |      10 |      semi random |
|  crumsort | 10000000 |   32 | 0.045479 | 0.046088 |         0 |      10 |      semi random |
|   pdqsort | 10000000 |   32 | 0.060253 | 0.060612 |         0 |      10 |      semi random |
|  wolfsort | 10000000 |   32 | 0.190505 | 0.191946 |         0 |      10 |      semi random |
|           |          |      |          |          |           |         |                  |
|  blitsort | 10000000 |   32 | 0.272456 | 0.274928 |         0 |      10 |    random signal |
|  crumsort | 10000000 |   32 | 0.224115 | 0.225966 |         0 |      10 |    random signal |
|   pdqsort | 10000000 |   32 | 0.382742 | 0.384505 |         0 |      10 |    random signal |
|  wolfsort | 10000000 |   32 | 0.160946 | 0.161769 |         0 |      10 |    random signal |

</details>
