Intro
-----

This document describes a stable adaptive hybrid radix / quicksort / merge sort named wolfsort.

Why a hybrid?
-------------
While an adaptive merge sort is very fast at sorting ordered data, its inability to effectively
partition is its greatest weakness. Radix sort on the other hand is unable to take advantage of
sorted data. While quicksort is fast at partitioning, radix sort is faster on large arrays.

Fluxsort
--------
Wolfsort uses [fluxsort](https://github.com/scandum/fluxsort "fluxsort") for sorting partitioned
arrays. Fluxsort is a stable hybrid quicksort / mergesort which sorts random data two times faster than
merge sort, while ordered data is sorted up to an order of magnitude faster.

Detecting whether the array is worth partitioning
-------------------------------------------------
Wolfsort operates like a typical radix sort by creating 256 buckets and dividing each unsigned
32 bit integer by 16777216. Without any optimizations this would multiply the memory overhead
by 256 times. Wolfsort solves this problem by running over the data once to find out the final
size of each bucket, it then only needs to multiply the memory once and perform one more run
to finish partitioning.

If the bucket distribution is sub-optimal the radix sort is aborted and fluxsort is ran instead.
While this may seem wasteful it doesn't take much time in practice.

Partition in a way that is beneficial to comparison sorts
---------------------------------------------------------
Because this approach is the equivalent of an in-place MSD Radix sort the 256 buckets are
in order once partitioning completes. The next step is to sort the content of each bucket
using fluxsort and wolfsort will be finished.

Memory overhead
---------------
Wolfsort requires O(n) memory for the partitioning process and O(sqrt n) memory for the buckets.

If not enough memory is available wolfsort falls back on fluxsort which requires n swap memory,
and if that's not sufficient fluxsort falls back on quadsort which requires n / 2 swap memory.

Proof of concept
----------------
Wolfsort is primarily a proof of concept, currently it only properly supports unsigned 32 and
64 bit integers.

I'll briefly mention other sorting algorithms listed in the benchmark code / graphs.

Blitsort
--------
Blitsort is an in-place adaptive rotation mergesort.

Quadsort
--------
Quadsort is an adaptive mergesort.

Gridsort
--------
Gridsort is a stable comparison sort which stores data in a 2 dimensional self-balancing grid.

Fluxsort
--------
Fluxsort is a stable branchless out-of-place quicksort.

Big O
-----
```cobol
                 ┌───────────────────────┐┌────────────────────┐
                 │comparisons            ││swap memory         │
┌───────────────┐├───────┬───────┬───────┤├──────┬──────┬──────┤┌──────┐┌─────────┐┌─────────┐┌─────────┐
│name           ││min    │avg    │max    ││min   │avg   │max   ││stable││partition││adaptive ││compares │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│fluxsort       ││n      │n log n│n log n││n     │n     │n     ││yes   ││yes      ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│gridsort       ││n      │n log n│n log n││n     │n     │n     ││yes   ││yes      ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│introsort      ││n log n│n log n│n log n││1     │1     │1     ││no    ││yes      ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│mergesort      ││n log n│n log n│n log n││n     │n     │n     ││yes   ││no       ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│octosort       ││n      │n log n│n log n││1     │1     │1     ││yes   ││no       ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│quadsort       ││n      │n log n│n log n││1     │n     │n     ││yes   ││no       ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│quicksort      ││n      │n log n│n²     ││1     │1     │1     ││no    ││yes      ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│timsort        ││n      │n log n│n log n││n     │n     │n     ││yes   ││no       ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│wolfsort       ││n      │n log n│n log n││n     │n     │n     ││yes   ││yes      ││yes      ││no       │
└───────────────┘└───────┴───────┴───────┘└──────┴──────┴──────┘└──────┘└─────────┘└─────────┘└─────────┘
```

Benchmarks
----------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using g++ -O3 -fpermissive bench.c. All comparisons are inlined through the cmp macro.
Each test was ran 10 times with the best run and average reported.

![Graph](/graph1.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|   timsort |   100000 |   64 | 0.007759 | 0.007792 |         1 |     100 |     random order |
|stablesort |   100000 |   64 | 0.006066 | 0.006102 |         1 |     100 |     random order |
|  quadsort |   100000 |   64 | 0.005470 | 0.005504 |         1 |     100 |     random order |
|  gridsort |   100000 |   64 | 0.004243 | 0.004280 |         1 |     100 |     random order |
|  fluxsort |   100000 |   64 | 0.002480 | 0.002489 |         1 |     100 |     random order |
|  wolfsort |   100000 |   64 | 0.002172 | 0.002182 |         1 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|   timsort |   100000 |   32 | 0.007628 | 0.007679 |         1 |     100 |     random order |
|stablesort |   100000 |   32 | 0.006053 | 0.006078 |         1 |     100 |     random order |
|  quadsort |   100000 |   32 | 0.005408 | 0.005442 |         1 |     100 |     random order |
|  gridsort |   100000 |   32 | 0.004207 | 0.004259 |         1 |     100 |     random order |
|  fluxsort |   100000 |   32 | 0.002317 | 0.002335 |         1 |     100 |     random order |
|  wolfsort |   100000 |   32 | 0.001302 | 0.001317 |         1 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|   timsort |   100000 |   32 | 0.000045 | 0.000045 |         1 |     100 |  ascending order |
|stablesort |   100000 |   32 | 0.000675 | 0.000687 |         1 |     100 |  ascending order |
|  quadsort |   100000 |   32 | 0.000052 | 0.000052 |         1 |     100 |  ascending order |
|  gridsort |   100000 |   32 | 0.000353 | 0.000365 |         1 |     100 |  ascending order |
|  fluxsort |   100000 |   32 | 0.000037 | 0.000038 |         1 |     100 |  ascending order |
|  wolfsort |   100000 |   32 | 0.000043 | 0.000044 |         1 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|   timsort |   100000 |   32 | 0.000850 | 0.000864 |         1 |     100 |    ascending saw |
|stablesort |   100000 |   32 | 0.001365 | 0.001390 |         1 |     100 |    ascending saw |
|  quadsort |   100000 |   32 | 0.000799 | 0.000810 |         1 |     100 |    ascending saw |
|  gridsort |   100000 |   32 | 0.001278 | 0.001291 |         1 |     100 |    ascending saw |
|  fluxsort |   100000 |   32 | 0.000825 | 0.000834 |         1 |     100 |    ascending saw |
|  wolfsort |   100000 |   32 | 0.001197 | 0.001211 |         1 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|   timsort |   100000 |   32 | 0.004633 | 0.004657 |         1 |     100 |    generic order |
|stablesort |   100000 |   32 | 0.002995 | 0.003014 |         1 |     100 |    generic order |
|  quadsort |   100000 |   32 | 0.002480 | 0.002490 |         1 |     100 |    generic order |
|  gridsort |   100000 |   32 | 0.001369 | 0.001385 |         1 |     100 |    generic order |
|  fluxsort |   100000 |   32 | 0.001004 | 0.001007 |         1 |     100 |    generic order |
|  wolfsort |   100000 |   32 | 0.001012 | 0.001020 |         1 |     100 |    generic order |
|           |          |      |          |          |           |         |                  |
|   timsort |   100000 |   32 | 0.000101 | 0.000103 |         1 |     100 | descending order |
|stablesort |   100000 |   32 | 0.000897 | 0.000912 |         1 |     100 | descending order |
|  quadsort |   100000 |   32 | 0.000047 | 0.000047 |         1 |     100 | descending order |
|  gridsort |   100000 |   32 | 0.000381 | 0.000385 |         1 |     100 | descending order |
|  fluxsort |   100000 |   32 | 0.000048 | 0.000050 |         1 |     100 | descending order |
|  wolfsort |   100000 |   32 | 0.000054 | 0.000057 |         1 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|   timsort |   100000 |   32 | 0.000474 | 0.000491 |         1 |     100 |   descending saw |
|stablesort |   100000 |   32 | 0.001050 | 0.001067 |         1 |     100 |   descending saw |
|  quadsort |   100000 |   32 | 0.000324 | 0.000333 |         1 |     100 |   descending saw |
|  gridsort |   100000 |   32 | 0.000755 | 0.000770 |         1 |     100 |   descending saw |
|  fluxsort |   100000 |   32 | 0.000348 | 0.000354 |         1 |     100 |   descending saw |
|  wolfsort |   100000 |   32 | 0.000353 | 0.000367 |         1 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|   timsort |   100000 |   32 | 0.002006 | 0.002028 |         1 |     100 |      random tail |
|stablesort |   100000 |   32 | 0.002059 | 0.002113 |         1 |     100 |      random tail |
|  quadsort |   100000 |   32 | 0.001449 | 0.001459 |         1 |     100 |      random tail |
|  gridsort |   100000 |   32 | 0.001335 | 0.001344 |         1 |     100 |      random tail |
|  fluxsort |   100000 |   32 | 0.001471 | 0.001481 |         1 |     100 |      random tail |
|  wolfsort |   100000 |   32 | 0.001004 | 0.001012 |         1 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|   timsort |   100000 |   32 | 0.004034 | 0.004060 |         1 |     100 |      random half |
|stablesort |   100000 |   32 | 0.003545 | 0.003573 |         1 |     100 |      random half |
|  quadsort |   100000 |   32 | 0.002869 | 0.002882 |         1 |     100 |      random half |
|  gridsort |   100000 |   32 | 0.002367 | 0.002391 |         1 |     100 |      random half |
|  fluxsort |   100000 |   32 | 0.002078 | 0.002094 |         1 |     100 |      random half |
|  wolfsort |   100000 |   32 | 0.001174 | 0.001186 |         1 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|   timsort |   100000 |   32 | 0.000757 | 0.000853 |         1 |     100 |  ascending tiles |
|stablesort |   100000 |   32 | 0.000956 | 0.000962 |         1 |     100 |  ascending tiles |
|  quadsort |   100000 |   32 | 0.000662 | 0.000701 |         1 |     100 |  ascending tiles |
|  gridsort |   100000 |   32 | 0.000811 | 0.000853 |         1 |     100 |  ascending tiles |
|  fluxsort |   100000 |   32 | 0.000684 | 0.000694 |         1 |     100 |  ascending tiles |
|  wolfsort |   100000 |   32 | 0.000691 | 0.000733 |         1 |     100 |  ascending tiles |


</details>

![Graph](/graph2.png)
<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|   timsort |        4 |   32 | 0.002853 | 0.002892 |    131072 |     100 |         random 4 |
|stablesort |        4 |   32 | 0.005420 | 0.005455 |    131072 |     100 |         random 4 |
|  quadsort |        4 |   32 | 0.002100 | 0.002125 |    131072 |     100 |         random 4 |
|  gridsort |        4 |   32 | 0.002214 | 0.002264 |    131072 |     100 |         random 4 |
|  fluxsort |        4 |   32 | 0.001943 | 0.001961 |    131072 |     100 |         random 4 |
|  wolfsort |        4 |   32 | 0.002218 | 0.002242 |    131072 |     100 |         random 4 |
|           |          |      |          |          |           |         |                  |
|   timsort |       16 |   32 | 0.004795 | 0.004845 |     32768 |     100 |        random 16 |
|stablesort |       16 |   32 | 0.005302 | 0.005341 |     32768 |     100 |        random 16 |
|  quadsort |       16 |   32 | 0.002706 | 0.002734 |     32768 |     100 |        random 16 |
|  gridsort |       16 |   32 | 0.002730 | 0.002751 |     32768 |     100 |        random 16 |
|  fluxsort |       16 |   32 | 0.002674 | 0.002694 |     32768 |     100 |        random 16 |
|  wolfsort |       16 |   32 | 0.002706 | 0.002729 |     32768 |     100 |        random 16 |
|           |          |      |          |          |           |         |                  |
|   timsort |       64 |   32 | 0.008662 | 0.008755 |      8192 |     100 |        random 64 |
|stablesort |       64 |   32 | 0.006816 | 0.006928 |      8192 |     100 |        random 64 |
|  quadsort |       64 |   32 | 0.004805 | 0.004824 |      8192 |     100 |        random 64 |
|  gridsort |       64 |   32 | 0.004806 | 0.004831 |      8192 |     100 |        random 64 |
|  fluxsort |       64 |   32 | 0.003699 | 0.003730 |      8192 |     100 |        random 64 |
|  wolfsort |       64 |   32 | 0.003743 | 0.003769 |      8192 |     100 |        random 64 |
|           |          |      |          |          |           |         |                  |
|   timsort |      256 |   32 | 0.011363 | 0.011456 |      2048 |     100 |       random 256 |
|stablesort |      256 |   32 | 0.008692 | 0.008777 |      2048 |     100 |       random 256 |
|  quadsort |      256 |   32 | 0.006656 | 0.006707 |      2048 |     100 |       random 256 |
|  gridsort |      256 |   32 | 0.006663 | 0.006704 |      2048 |     100 |       random 256 |
|  fluxsort |      256 |   32 | 0.004226 | 0.004252 |      2048 |     100 |       random 256 |
|  wolfsort |      256 |   32 | 0.004239 | 0.004278 |      2048 |     100 |       random 256 |
|           |          |      |          |          |           |         |                  |
|   timsort |     1024 |   32 | 0.013543 | 0.013648 |       512 |     100 |      random 1024 |
|stablesort |     1024 |   32 | 0.010744 | 0.010904 |       512 |     100 |      random 1024 |
|  quadsort |     1024 |   32 | 0.008309 | 0.008348 |       512 |     100 |      random 1024 |
|  gridsort |     1024 |   32 | 0.008310 | 0.008362 |       512 |     100 |      random 1024 |
|  fluxsort |     1024 |   32 | 0.004712 | 0.004760 |       512 |     100 |      random 1024 |
|  wolfsort |     1024 |   32 | 0.003600 | 0.003626 |       512 |     100 |      random 1024 |
|           |          |      |          |          |           |         |                  |
|   timsort |     4096 |   32 | 0.015687 | 0.015798 |       128 |     100 |      random 4096 |
|stablesort |     4096 |   32 | 0.012689 | 0.012783 |       128 |     100 |      random 4096 |
|  quadsort |     4096 |   32 | 0.010082 | 0.010129 |       128 |     100 |      random 4096 |
|  gridsort |     4096 |   32 | 0.010377 | 0.010447 |       128 |     100 |      random 4096 |
|  fluxsort |     4096 |   32 | 0.005146 | 0.005187 |       128 |     100 |      random 4096 |
|  wolfsort |     4096 |   32 | 0.003522 | 0.003584 |       128 |     100 |      random 4096 |
|           |          |      |          |          |           |         |                  |
|   timsort |    16384 |   32 | 0.017753 | 0.017893 |        32 |     100 |     random 16384 |
|stablesort |    16384 |   32 | 0.014847 | 0.015000 |        32 |     100 |     random 16384 |
|  quadsort |    16384 |   32 | 0.012000 | 0.012064 |        32 |     100 |     random 16384 |
|  gridsort |    16384 |   32 | 0.011369 | 0.011494 |        32 |     100 |     random 16384 |
|  fluxsort |    16384 |   32 | 0.005634 | 0.005688 |        32 |     100 |     random 16384 |
|  wolfsort |    16384 |   32 | 0.003629 | 0.003688 |        32 |     100 |     random 16384 |
|           |          |      |          |          |           |         |                  |
|   timsort |    65536 |   32 | 0.019872 | 0.020089 |         8 |     100 |     random 65536 |
|stablesort |    65536 |   32 | 0.017033 | 0.017252 |         8 |     100 |     random 65536 |
|  quadsort |    65536 |   32 | 0.013932 | 0.014005 |         8 |     100 |     random 65536 |
|  gridsort |    65536 |   32 | 0.011870 | 0.011972 |         8 |     100 |     random 65536 |
|  fluxsort |    65536 |   32 | 0.006162 | 0.006196 |         8 |     100 |     random 65536 |
|  wolfsort |    65536 |   32 | 0.004044 | 0.004104 |         8 |     100 |     random 65536 |
|           |          |      |          |          |           |         |                  |
|   timsort |   262144 |   32 | 0.021993 | 0.022179 |         2 |     100 |    random 262144 |
|stablesort |   262144 |   32 | 0.019308 | 0.019504 |         2 |     100 |    random 262144 |
|  quadsort |   262144 |   32 | 0.015722 | 0.015855 |         2 |     100 |    random 262144 |
|  gridsort |   262144 |   32 | 0.013180 | 0.013302 |         2 |     100 |    random 262144 |
|  fluxsort |   262144 |   32 | 0.006634 | 0.006758 |         2 |     100 |    random 262144 |
|  wolfsort |   262144 |   32 | 0.005484 | 0.005622 |         2 |     100 |    random 262144 |

</details>
