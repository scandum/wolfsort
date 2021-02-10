Intro
-----

This document describes a stable adaptive hybrid radix / merge sort named wolfsort.

Why a hybrid?
-------------
While an adaptive merge sort is very fast at sorting ordered data, its inability to effectively
partition is its greatest weakness. Radix sort on the other hand is unable to take advantage of
sorted data. Wolfsort tries to avoid the worst case of each algorithm.

Quadsort
--------
Wolfsort uses [quadsort](https://github.com/scandum/quadsort "quadsort") for sorting partitioned
arrays. Quadsort sorts random data slightly faster than merge sort, while ordered data is sorted up to
an order of magnitude faster.

Detecting whether the array is worth partitioning
-------------------------------------------------
Wolfsort operates like a typical radix sort by creating 256 buckets and dividing each unsigned
32 bit integer by 16777216. Without any optimizations this would multiply the memory overhead
by 256 times. Wolfsort solves this problem by running over the data once to find out the final
size of each bucket, it then only needs to multiply the memory once and perform one more run
to finish partitioning.

If the bucket distribution is sub-optimal the radix sort is aborted and quadsort is ran instead.
While this may seem wasteful it means in practice that the worst case of each algorithm is
avoided at a relatively small cost.

Partition in a way that is beneficial to merge sort
---------------------------------------------------
Because this approach is the equivalent of an in-place MSD Radix sort the 256 buckets are
in order once partitioning completes. The next step is to sort the content of each bucket
using quadsort and wolfsort will be finished.

Memory overhead
---------------
Wolfsort requires O(n) memory for the partitioning process and O(sqrt n) memory for the buckets.

If not enough memory is available wolfsort falls back on quadsort which requires 1/2 the array
size in swap memory.

Historically software was always low on memory, and subsequently sorting algorithms were developed
that bent over backwards in order to use as little additional memory as possible. Nowadays most
modern systems have several gigabytes of memory that are not used and are just sitting idle. 

Proof of concept
----------------
Wolfsort is primarily a proof of concept, currently it only properly supports unsigned 32 and
64 bit integers.

Flowsort
--------
Flowsort was an improvement upon wolfsort with a memory optimization. Since this pretty much
obsoleted wolfsort I've merged flowsort back into wolfsort. So wolfsort and flowsort are one
and the same now.

Gridsort
--------
Gridsort stores data in a 2 dimensional self balancing grid. It has good overall performance. 

Fluxsort
--------
Fluxsort uses a pivot table to partition. It somewhat resembles wolfsort but it is a comparison sort. It is faster than gridsort on random data but slower on ordered data.

Big O
-----
```
                 ┌───────────────────────┐┌───────────────────────┐
                 │comparisons            ││swap memory            │
┌───────────────┐├───────┬───────┬───────┤├───────┬───────┬───────┤┌──────┐┌─────────┐┌─────────┐
│name           ││min    │avg    │max    ││min    │avg    │max    ││stable││partition││adaptive │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│gridsort       ││n      │n log n│n log n││n      │n      │n      ││yes   ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│mergesort      ││n log n│n log n│n log n││n      │n      │n      ││yes   ││no       ││no       │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│timsort        ││n      │n log n│n log n││n      │n      │n      ││yes   ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│quadsort       ││n      │n log n│n log n││1      │n      │n      ││yes   ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│quicksort      ││n      │n log n│n²     ││1      │1      │1      ││no    ││yes      ││no       │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│introsort      ││n log n│n log n│n log n││1      │1      │1      ││no    ││yes      ││no       │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│fluxsort       ││n log n│n log n│n log n││n      │n      │n      ││yes   ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│wolfsort       ││n      │n log n│n log n││n      │n      │n      ││yes   ││yes      ││yes      │
└───────────────┘└───────┴───────┴───────┘└───────┴───────┴───────┘└──────┘└─────────┘└─────────┘
```

Benchmarks
----------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using g++ -O3 -fpermissive bench.c.
Each test was ran 100 times and only the best run is reported.

![Graph](/graph1.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
| std::sort |  1000000 |   32 | 0.065128 | 0.065403 |         1 |      10 |     random order |
|  gridsort |  1000000 |   32 | 0.054307 | 0.054722 |         1 |      10 |     random order |
|   timsort |  1000000 |   32 | 0.088793 | 0.089017 |         1 |      10 |     random order |
|  quadsort |  1000000 |   32 | 0.071241 | 0.071587 |         1 |      10 |     random order |
|  fluxsort |  1000000 |   32 | 0.042582 | 0.042675 |         1 |      10 |     random order |
|  wolfsort |  1000000 |   32 | 0.015185 | 0.015307 |         1 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.011304 | 0.011706 |         1 |      10 |  ascending order |
|  gridsort |  1000000 |   32 | 0.003422 | 0.003520 |         1 |      10 |  ascending order |
|   timsort |  1000000 |   32 | 0.000861 | 0.000889 |         1 |      10 |  ascending order |
|  quadsort |  1000000 |   32 | 0.000810 | 0.000884 |         1 |      10 |  ascending order |
|  fluxsort |  1000000 |   32 | 0.011368 | 0.011727 |         1 |      10 |  ascending order |
|  wolfsort |  1000000 |   32 | 0.000953 | 0.000971 |         1 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.033858 | 0.034105 |         1 |      10 |    ascending saw |
|  gridsort |  1000000 |   32 | 0.013613 | 0.013780 |         1 |      10 |    ascending saw |
|   timsort |  1000000 |   32 | 0.009098 | 0.009211 |         1 |      10 |    ascending saw |
|  quadsort |  1000000 |   32 | 0.009355 | 0.009537 |         1 |      10 |    ascending saw |
|  fluxsort |  1000000 |   32 | 0.024408 | 0.024579 |         1 |      10 |    ascending saw |
|  wolfsort |  1000000 |   32 | 0.012417 | 0.012572 |         1 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.030146 | 0.030481 |         1 |      10 |    generic order |
|  gridsort |  1000000 |   32 | 0.016187 | 0.016413 |         1 |      10 |    generic order |
|   timsort |  1000000 |   32 | 0.055019 | 0.055213 |         1 |      10 |    generic order |
|  quadsort |  1000000 |   32 | 0.039879 | 0.040191 |         1 |      10 |    generic order |
|  fluxsort |  1000000 |   32 | 0.018304 | 0.018389 |         1 |      10 |    generic order |
|  wolfsort |  1000000 |   32 | 0.040024 | 0.040371 |         1 |      10 |    generic order |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.008793 | 0.009065 |         1 |      10 | descending order |
|  gridsort |  1000000 |   32 | 0.003644 | 0.003706 |         1 |      10 | descending order |
|   timsort |  1000000 |   32 | 0.001097 | 0.001150 |         1 |      10 | descending order |
|  quadsort |  1000000 |   32 | 0.000870 | 0.000951 |         1 |      10 | descending order |
|  fluxsort |  1000000 |   32 | 0.012440 | 0.012604 |         1 |      10 | descending order |
|  wolfsort |  1000000 |   32 | 0.000922 | 0.000936 |         1 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.025845 | 0.026169 |         1 |      10 |   descending saw |
|  gridsort |  1000000 |   32 | 0.013141 | 0.013328 |         1 |      10 |   descending saw |
|   timsort |  1000000 |   32 | 0.006566 | 0.006690 |         1 |      10 |   descending saw |
|  quadsort |  1000000 |   32 | 0.007464 | 0.007630 |         1 |      10 |   descending saw |
|  fluxsort |  1000000 |   32 | 0.020248 | 0.021360 |         1 |      10 |   descending saw |
|  wolfsort |  1000000 |   32 | 0.007597 | 0.007845 |         1 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.044429 | 0.044600 |         1 |      10 |      random tail |
|  gridsort |  1000000 |   32 | 0.016099 | 0.016216 |         1 |      10 |      random tail |
|   timsort |  1000000 |   32 | 0.023622 | 0.023743 |         1 |      10 |      random tail |
|  quadsort |  1000000 |   32 | 0.019161 | 0.019447 |         1 |      10 |      random tail |
|  fluxsort |  1000000 |   32 | 0.022112 | 0.022253 |         1 |      10 |      random tail |
|  wolfsort |  1000000 |   32 | 0.010646 | 0.010932 |         1 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.055712 | 0.056032 |         1 |      10 |      random half |
|  gridsort |  1000000 |   32 | 0.029853 | 0.030080 |         1 |      10 |      random half |
|   timsort |  1000000 |   32 | 0.046855 | 0.047019 |         1 |      10 |      random half |
|  quadsort |  1000000 |   32 | 0.038099 | 0.038253 |         1 |      10 |      random half |
|  fluxsort |  1000000 |   32 | 0.031118 | 0.031274 |         1 |      10 |      random half |
|  wolfsort |  1000000 |   32 | 0.012945 | 0.013110 |         1 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.027717 | 0.028003 |         1 |      10 |  ascending tiles |
|  gridsort |  1000000 |   32 | 0.012474 | 0.012644 |         1 |      10 |  ascending tiles |
|   timsort |  1000000 |   32 | 0.009282 | 0.009517 |         1 |      10 |  ascending tiles |
|  quadsort |  1000000 |   32 | 0.010096 | 0.010313 |         1 |      10 |  ascending tiles |
|  fluxsort |  1000000 |   32 | 0.016978 | 0.017260 |         1 |      10 |  ascending tiles |
|  wolfsort |  1000000 |   32 | 0.010203 | 0.010622 |         1 |      10 |  ascending tiles |

</details>

![Graph](/graph2.png)
<details><summary><b>data table</b></summary>

| std::sort |       16 |   32 | 0.000824 | 0.001007 |     16384 |     100 |        random 16 |
|  gridsort |       16 |   32 | 0.000680 | 0.000684 |     16384 |     100 |        random 16 |
|   timsort |       16 |   32 | 0.001456 | 0.001688 |     16384 |     100 |        random 16 |
|  quadsort |       16 |   32 | 0.000676 | 0.000679 |     16384 |     100 |        random 16 |
|  fluxsort |       16 |   32 | 0.000682 | 0.000689 |     16384 |     100 |        random 16 |
|  wolfsort |       16 |   32 | 0.000704 | 0.000712 |     16384 |     100 |        random 16 |
|           |          |      |          |          |           |         |                  |
| std::sort |       64 |   32 | 0.000928 | 0.001197 |      4096 |     100 |        random 64 |
|  gridsort |       64 |   32 | 0.001121 | 0.001159 |      4096 |     100 |        random 64 |
|   timsort |       64 |   32 | 0.002642 | 0.003565 |      4096 |     100 |        random 64 |
|  quadsort |       64 |   32 | 0.001118 | 0.001161 |      4096 |     100 |        random 64 |
|  fluxsort |       64 |   32 | 0.001132 | 0.001195 |      4096 |     100 |        random 64 |
|  wolfsort |       64 |   32 | 0.001139 | 0.001189 |      4096 |     100 |        random 64 |
|           |          |      |          |          |           |         |                  |
| std::sort |      256 |   32 | 0.001143 | 0.002045 |      1024 |     100 |       random 256 |
|  gridsort |      256 |   32 | 0.001591 | 0.002312 |      1024 |     100 |       random 256 |
|   timsort |      256 |   32 | 0.003134 | 0.005045 |      1024 |     100 |       random 256 |
|  quadsort |      256 |   32 | 0.001591 | 0.002034 |      1024 |     100 |       random 256 |
|  fluxsort |      256 |   32 | 0.001591 | 0.001982 |      1024 |     100 |       random 256 |
|  wolfsort |      256 |   32 | 0.001591 | 0.002011 |      1024 |     100 |       random 256 |
|           |          |      |          |          |           |         |                  |
| std::sort |     1024 |   32 | 0.004473 | 0.006068 |       256 |     100 |      random 1024 |
|  gridsort |     1024 |   32 | 0.004144 | 0.004570 |       256 |     100 |      random 1024 |
|   timsort |     1024 |   32 | 0.008582 | 0.010212 |       256 |     100 |      random 1024 |
|  quadsort |     1024 |   32 | 0.004197 | 0.005383 |       256 |     100 |      random 1024 |
|  fluxsort |     1024 |   32 | 0.003747 | 0.004403 |       256 |     100 |      random 1024 |
|  wolfsort |     1024 |   32 | 0.001953 | 0.002821 |       256 |     100 |      random 1024 |
|           |          |      |          |          |           |         |                  |
| std::sort |     4096 |   32 | 0.010119 | 0.010335 |        64 |     100 |      random 4096 |
|  gridsort |     4096 |   32 | 0.009886 | 0.009999 |        64 |     100 |      random 4096 |
|   timsort |     4096 |   32 | 0.015007 | 0.015085 |        64 |     100 |      random 4096 |
|  quadsort |     4096 |   32 | 0.010356 | 0.010483 |        64 |     100 |      random 4096 |
|  fluxsort |     4096 |   32 | 0.007246 | 0.007482 |        64 |     100 |      random 4096 |
|  wolfsort |     4096 |   32 | 0.002736 | 0.003196 |        64 |     100 |      random 4096 |
|           |          |      |          |          |           |         |                  |
| std::sort |    16384 |   32 | 0.012230 | 0.012276 |        16 |     100 |     random 16384 |
|  gridsort |    16384 |   32 | 0.011482 | 0.011530 |        16 |     100 |     random 16384 |
|   timsort |    16384 |   32 | 0.017246 | 0.017317 |        16 |     100 |     random 16384 |
|  quadsort |    16384 |   32 | 0.012929 | 0.012992 |        16 |     100 |     random 16384 |
|  fluxsort |    16384 |   32 | 0.009284 | 0.009310 |        16 |     100 |     random 16384 |
|  wolfsort |    16384 |   32 | 0.003380 | 0.003436 |        16 |     100 |     random 16384 |
|           |          |      |          |          |           |         |                  |
| std::sort |    65536 |   32 | 0.013791 | 0.013841 |         4 |     100 |     random 65536 |
|  gridsort |    65536 |   32 | 0.011913 | 0.011968 |         4 |     100 |     random 65536 |
|   timsort |    65536 |   32 | 0.019334 | 0.019397 |         4 |     100 |     random 65536 |
|  quadsort |    65536 |   32 | 0.014814 | 0.014877 |         4 |     100 |     random 65536 |
|  fluxsort |    65536 |   32 | 0.010566 | 0.010605 |         4 |     100 |     random 65536 |
|  wolfsort |    65536 |   32 | 0.003412 | 0.003478 |         4 |     100 |     random 65536 |
|           |          |      |          |          |           |         |                  |
| std::sort |   262144 |   32 | 0.015406 | 0.015474 |         1 |     100 |    random 262144 |
|  gridsort |   262144 |   32 | 0.013370 | 0.013419 |         1 |     100 |    random 262144 |
|   timsort |   262144 |   32 | 0.021478 | 0.021555 |         1 |     100 |    random 262144 |
|  quadsort |   262144 |   32 | 0.016760 | 0.016829 |         1 |     100 |    random 262144 |
|  fluxsort |   262144 |   32 | 0.010333 | 0.010377 |         1 |     100 |    random 262144 |
|  wolfsort |   262144 |   32 | 0.003557 | 0.003596 |         1 |     100 |    random 262144 |

</details>
