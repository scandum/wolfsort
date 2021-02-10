Intro
-----

This document describes a stable adaptive hybrid radix / merge sort named wolfsort. It
is likely the fastest sort written so far for sorting a mixture of random and ordered data
in a stable manner.

Why a hybrid?
-------------
While an adaptive merge sort is very fast at sorting ordered data, its inability to effectively
partition is its greatest weakness. Radix sort on the other hand is unable to take advantage of
sorted data. Wolfsort tries to avoid the worst case of each algorithm.

Adaptive partitioning
---------------------
Wolfsort uses [quadsort](https://github.com/scandum/quadsort "quadsort") as its adaptive merge
sort since it's faster than Timsort. The radix sort used by wolfsort has two primary functions.

1. Detecting whether the array is worth partitioning with radix sort.
2. Partition in a way that is beneficial to quadsort.

Detecting whether the array is worth partitioning
-------------------------------------------------

Wolfsort operates like a typical radix sort by creating 256 buckets and dividing each unsigned
32 bit integer by 16777216. Without any optimizations this would multiply the memory overhead
by 256 times. Being a hybrid sort wolfsort solves this problem by giving each bucket 1/8th of
the maximum size.

If the maximum bucket size is reached the radix sort is aborted and quadsort is ran instead.
While this may seem wasteful it means in practice that the worst case of each algorithm is
avoided at a relatively small cost.

Partition in a way that is beneficial to merge sort
---------------------------------------------------
Because this approach is the equivalent of an in-place MSD Radix sort the 256 buckets are
in order once partitioning completes. The next step is to sort the content of each bucket
using quadsort and wolfsort will be finished.

Memory overhead
---------------
Wolfsort requires 8 times the array size in swap memory for the O(n) partitioning process.
The sorting process that follows requires 1/32nd the array size in swap memory.

If not enough memory is available wolfsort falls back on quadsort which requires 1/2 the array
size in swap memory.

In ideal conditions there is no computational difference between allocating and freeing 10 MB
of memory and allocating and freeing 80 MB of memory. Historically software was always low on
memory, and subsequently sorting algorithms were developed that bent over backwards in order
to use as little additional memory as possible. Nowadays most modern systems have several
gigabytes of memory that are not used and are just sitting idle. 

During the partitioning process each bucket becomes akin to Schrödinger's cat, it may be used
almost fully, not at all, or somewhere in between. Based on probability we know exactly 1/8th
will be actually used. The only overhead is to keep track of the size of each bucket, and to
check that the bucket is not overflowing upon each addition.

Proof of concept
----------------
Wolfsort is primarily a proof of concept, currently it only properly supports unsigned 32 and
64 bit integers.

Memory allocation with malloc() is a bottleneck on many platforms. Using a better memory
allocator or permanent swap memory is a possible solution. 

Flowsort
--------
Flowsort is similar to wolfsort but has some optimizations to reduce the memory overhead by
8 times. While slower below 1 million items it becomes faster above.

Gridsort
--------
Gridsort stores data in a 2 dimensional self balancing grid.

Fluxsort
--------
Fluxsort uses a pivot table to partition. It somewhat resembles flowsort but it is a stable comparison sort.

Big O
-----
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
│fluxsort       ││n log n│n log n│n log n││n      │n      │n      ││yes   ││yes      ││semi     │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│flowsort       ││n      │n log n│n log n││n      │n      │n      ││yes   ││yes      ││semi     │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│wolfsort       ││n      │n log n│n log n││n      │n      │n      ││yes   ││yes      ││semi     │
└───────────────┘└───────┴───────┴───────┘└───────┴───────┴───────┘└──────┘└─────────┘└─────────┘

Benchmarks
----------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using g++ -O3 -fpermissive bench.c.
Each test was ran 100 times and only the best run is reported.

![Graph](/graph1.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
| std::sort |  1000000 |   32 | 0.064883 | 0.065081 |         1 |     100 |     random order |
|  gridsort |  1000000 |   32 | 0.054172 | 0.054293 |         1 |     100 |     random order |
|   timsort |  1000000 |   32 | 0.090033 | 0.090256 |         1 |     100 |     random order |
|  quadsort |  1000000 |   32 | 0.071145 | 0.071390 |         1 |     100 |     random order |
|  wolfsort |  1000000 |   32 | 0.016238 | 0.016670 |         1 |     100 |     random order |
|  flowsort |  1000000 |   32 | 0.015269 | 0.015434 |         1 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.011833 | 0.012046 |         1 |     100 |  ascending order |
|  gridsort |  1000000 |   32 | 0.003438 | 0.003536 |         1 |     100 |  ascending order |
|   timsort |  1000000 |   32 | 0.000798 | 0.000839 |         1 |     100 |  ascending order |
|  quadsort |  1000000 |   32 | 0.000776 | 0.000838 |         1 |     100 |  ascending order |
|  wolfsort |  1000000 |   32 | 0.000816 | 0.000893 |         1 |     100 |  ascending order |
|  flowsort |  1000000 |   32 | 0.000904 | 0.000969 |         1 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.033793 | 0.034033 |         1 |     100 |    ascending saw |
|  gridsort |  1000000 |   32 | 0.013622 | 0.013715 |         1 |     100 |    ascending saw |
|   timsort |  1000000 |   32 | 0.009138 | 0.009212 |         1 |     100 |    ascending saw |
|  quadsort |  1000000 |   32 | 0.009284 | 0.009385 |         1 |     100 |    ascending saw |
|  wolfsort |  1000000 |   32 | 0.011797 | 0.011904 |         1 |     100 |    ascending saw |
|  flowsort |  1000000 |   32 | 0.011972 | 0.012103 |         1 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.030323 | 0.030553 |         1 |     100 |    generic order |
|  gridsort |  1000000 |   32 | 0.016137 | 0.016389 |         1 |     100 |    generic order |
|   timsort |  1000000 |   32 | 0.056623 | 0.056795 |         1 |     100 |    generic order |
|  quadsort |  1000000 |   32 | 0.039481 | 0.040578 |         1 |     100 |    generic order |
|  wolfsort |  1000000 |   32 | 0.039489 | 0.043261 |         1 |     100 |    generic order |
|  flowsort |  1000000 |   32 | 0.039979 | 0.040394 |         1 |     100 |    generic order |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.009097 | 0.009242 |         1 |     100 | descending order |
|  gridsort |  1000000 |   32 | 0.003644 | 0.003743 |         1 |     100 | descending order |
|   timsort |  1000000 |   32 | 0.000997 | 0.001064 |         1 |     100 | descending order |
|  quadsort |  1000000 |   32 | 0.000790 | 0.000853 |         1 |     100 | descending order |
|  wolfsort |  1000000 |   32 | 0.000847 | 0.000906 |         1 |     100 | descending order |
|  flowsort |  1000000 |   32 | 0.000933 | 0.001058 |         1 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.026505 | 0.028416 |         1 |     100 |   descending saw |
|  gridsort |  1000000 |   32 | 0.013284 | 0.014019 |         1 |     100 |   descending saw |
|   timsort |  1000000 |   32 | 0.006910 | 0.007211 |         1 |     100 |   descending saw |
|  quadsort |  1000000 |   32 | 0.007435 | 0.007999 |         1 |     100 |   descending saw |
|  wolfsort |  1000000 |   32 | 0.007491 | 0.008009 |         1 |     100 |   descending saw |
|  flowsort |  1000000 |   32 | 0.007643 | 0.008120 |         1 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.044561 | 0.045434 |         1 |     100 |      random tail |
|  gridsort |  1000000 |   32 | 0.016231 | 0.017457 |         1 |     100 |      random tail |
|   timsort |  1000000 |   32 | 0.023901 | 0.024176 |         1 |     100 |      random tail |
|  quadsort |  1000000 |   32 | 0.019154 | 0.019405 |         1 |     100 |      random tail |
|  wolfsort |  1000000 |   32 | 0.009828 | 0.010061 |         1 |     100 |      random tail |
|  flowsort |  1000000 |   32 | 0.010346 | 0.010625 |         1 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.055612 | 0.056198 |         1 |     100 |      random half |
|  gridsort |  1000000 |   32 | 0.029677 | 0.029900 |         1 |     100 |      random half |
|   timsort |  1000000 |   32 | 0.047699 | 0.048204 |         1 |     100 |      random half |
|  quadsort |  1000000 |   32 | 0.038318 | 0.038905 |         1 |     100 |      random half |
|  wolfsort |  1000000 |   32 | 0.012920 | 0.013362 |         1 |     100 |      random half |
|  flowsort |  1000000 |   32 | 0.012906 | 0.013222 |         1 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.028192 | 0.029597 |         1 |     100 |  ascending tiles |
|  gridsort |  1000000 |   32 | 0.012479 | 0.013014 |         1 |     100 |  ascending tiles |
|   timsort |  1000000 |   32 | 0.009195 | 0.009666 |         1 |     100 |  ascending tiles |
|  quadsort |  1000000 |   32 | 0.010003 | 0.010229 |         1 |     100 |  ascending tiles |
|  wolfsort |  1000000 |   32 | 0.010007 | 0.010223 |         1 |     100 |  ascending tiles |
|  flowsort |  1000000 |   32 | 0.010088 | 0.010269 |         1 |     100 |  ascending tiles |
</details>

![Graph](/graph2.png)
<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
| std::sort |        4 |   32 | 0.000916 | 0.000952 |     65536 |     100 |         random 4 |
|  gridsort |        4 |   32 | 0.000748 | 0.000763 |     65536 |     100 |         random 4 |
|   timsort |        4 |   32 | 0.001249 | 0.001280 |     65536 |     100 |         random 4 |
|  quadsort |        4 |   32 | 0.000736 | 0.000759 |     65536 |     100 |         random 4 |
|  flowsort |        4 |   32 | 0.000671 | 0.000695 |     65536 |     100 |         random 4 |
|           |          |      |          |          |           |         |                  |
| std::sort |       16 |   32 | 0.000846 | 0.000942 |     16384 |     100 |        random 16 |
|  gridsort |       16 |   32 | 0.000680 | 0.000688 |     16384 |     100 |        random 16 |
|   timsort |       16 |   32 | 0.001497 | 0.001664 |     16384 |     100 |        random 16 |
|  quadsort |       16 |   32 | 0.000671 | 0.000674 |     16384 |     100 |        random 16 |
|  flowsort |       16 |   32 | 0.000658 | 0.000677 |     16384 |     100 |        random 16 |
|           |          |      |          |          |           |         |                  |
| std::sort |       64 |   32 | 0.001088 | 0.001197 |      4096 |     100 |        random 64 |
|  gridsort |       64 |   32 | 0.001052 | 0.001072 |      4096 |     100 |        random 64 |
|   timsort |       64 |   32 | 0.002877 | 0.003748 |      4096 |     100 |        random 64 |
|  quadsort |       64 |   32 | 0.001041 | 0.001050 |      4096 |     100 |        random 64 |
|  flowsort |       64 |   32 | 0.001102 | 0.001159 |      4096 |     100 |        random 64 |
|           |          |      |          |          |           |         |                  |
| std::sort |      256 |   32 | 0.001407 | 0.001440 |      1024 |     100 |       random 256 |
|  gridsort |      256 |   32 | 0.001413 | 0.002011 |      1024 |     100 |       random 256 |
|   timsort |      256 |   32 | 0.003387 | 0.004403 |      1024 |     100 |       random 256 |
|  quadsort |      256 |   32 | 0.001395 | 0.001943 |      1024 |     100 |       random 256 |
|  flowsort |      256 |   32 | 0.001400 | 0.001885 |      1024 |     100 |       random 256 |
|           |          |      |          |          |           |         |                  |
| std::sort |     1024 |   32 | 0.004652 | 0.005927 |       256 |     100 |      random 1024 |
|  gridsort |     1024 |   32 | 0.004267 | 0.005899 |       256 |     100 |      random 1024 |
|   timsort |     1024 |   32 | 0.008371 | 0.010288 |       256 |     100 |      random 1024 |
|  quadsort |     1024 |   32 | 0.004238 | 0.004845 |       256 |     100 |      random 1024 |
|  flowsort |     1024 |   32 | 0.001866 | 0.002555 |       256 |     100 |      random 1024 |
|           |          |      |          |          |           |         |                  |
| std::sort |     4096 |   32 | 0.009948 | 0.010134 |        64 |     100 |      random 4096 |
|  gridsort |     4096 |   32 | 0.009895 | 0.009957 |        64 |     100 |      random 4096 |
|   timsort |     4096 |   32 | 0.015388 | 0.015460 |        64 |     100 |      random 4096 |
|  quadsort |     4096 |   32 | 0.010229 | 0.010367 |        64 |     100 |      random 4096 |
|  flowsort |     4096 |   32 | 0.002698 | 0.003207 |        64 |     100 |      random 4096 |
|           |          |      |          |          |           |         |                  |
| std::sort |    16384 |   32 | 0.012201 | 0.012240 |        16 |     100 |     random 16384 |
|  gridsort |    16384 |   32 | 0.011402 | 0.011461 |        16 |     100 |     random 16384 |
|   timsort |    16384 |   32 | 0.017653 | 0.017699 |        16 |     100 |     random 16384 |
|  quadsort |    16384 |   32 | 0.012828 | 0.012865 |        16 |     100 |     random 16384 |
|  flowsort |    16384 |   32 | 0.003271 | 0.003321 |        16 |     100 |     random 16384 |
|           |          |      |          |          |           |         |                  |
| std::sort |    65536 |   32 | 0.013736 | 0.013781 |         4 |     100 |     random 65536 |
|  gridsort |    65536 |   32 | 0.011852 | 0.011923 |         4 |     100 |     random 65536 |
|   timsort |    65536 |   32 | 0.019756 | 0.019807 |         4 |     100 |     random 65536 |
|  quadsort |    65536 |   32 | 0.014718 | 0.014760 |         4 |     100 |     random 65536 |
|  flowsort |    65536 |   32 | 0.003349 | 0.003409 |         4 |     100 |     random 65536 |
|           |          |      |          |          |           |         |                  |
| std::sort |   262144 |   32 | 0.015384 | 0.015460 |         1 |     100 |    random 262144 |
|  gridsort |   262144 |   32 | 0.013304 | 0.013352 |         1 |     100 |    random 262144 |
|   timsort |   262144 |   32 | 0.021808 | 0.021933 |         1 |     100 |    random 262144 |
|  quadsort |   262144 |   32 | 0.016604 | 0.016665 |         1 |     100 |    random 262144 |
|  flowsort |   262144 |   32 | 0.003600 | 0.003659 |         1 |     100 |    random 262144 |

</details>
