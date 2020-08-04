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
usinge quadsort and wolfsort will be finished.

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

Benchmark
---------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using g++ -O3 -fpermissive bench.c.
Each test was ran 100 times and only the best run is reported.

Benchmark: quadsort vs std::stable_sort vs timsort vs pdqsort vs wolfsort
-------------------------------------------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using g++ -O3 quadsort.cpp. Each test was ran 100 times
and only the best run is reported. It should be noted that pdqsort is not a stable
sort which is the reason it's faster on generic order data.

The X axis of the bar graph below is the execution time.

![Graph](/graph1.png)

<details><summary><b>quadsort vs std::stable_sort vs timsort vs pdqsort vs wolfsort data table</b></summary>

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|  quadsort |  1000000 |  i32 | 0.070469 | 0.070635 |             |     random order |
|stablesort |  1000000 |  i32 | 0.073865 | 0.074078 |             |     random order |
|   timsort |  1000000 |  i32 | 0.089192 | 0.089301 |             |     random order |
|   pdqsort |  1000000 |  i32 | 0.029911 | 0.029948 |             |     random order |
|  wolfsort |  1000000 |  i32 | 0.017359 | 0.017744 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.000485 | 0.000511 |             |        ascending |
|stablesort |  1000000 |  i32 | 0.010188 | 0.010261 |             |        ascending |
|   timsort |  1000000 |  i32 | 0.000331 | 0.000342 |             |        ascending |
|   pdqsort |  1000000 |  i32 | 0.000863 | 0.000875 |             |        ascending |
|  wolfsort |  1000000 |  i32 | 0.000539 | 0.000579 |             |        ascending |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.008901 | 0.008934 |             |    ascending saw |
|stablesort |  1000000 |  i32 | 0.017093 | 0.017275 |             |    ascending saw |
|   timsort |  1000000 |  i32 | 0.008615 | 0.008674 |             |    ascending saw |
|   pdqsort |  1000000 |  i32 | 0.047785 | 0.047921 |             |    ascending saw |
|  wolfsort |  1000000 |  i32 | 0.012490 | 0.012554 |             |    ascending saw |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.038260 | 0.038343 |             |    generic order |
|stablesort |  1000000 |  i32 | 0.042292 | 0.042388 |             |    generic order |
|   timsort |  1000000 |  i32 | 0.055855 | 0.055967 |             |    generic order |
|   pdqsort |  1000000 |  i32 | 0.008093 | 0.008168 |             |    generic order |
|  wolfsort |  1000000 |  i32 | 0.038320 | 0.038417 |             |    generic order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.000559 | 0.000576 |             | descending order |
|stablesort |  1000000 |  i32 | 0.010343 | 0.010438 |             | descending order |
|   timsort |  1000000 |  i32 | 0.000891 | 0.000900 |             | descending order |
|   pdqsort |  1000000 |  i32 | 0.001882 | 0.001897 |             | descending order |
|  wolfsort |  1000000 |  i32 | 0.000604 | 0.000625 |             | descending order |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.006174 | 0.006245 |             |   descending saw |
|stablesort |  1000000 |  i32 | 0.014679 | 0.014767 |             |   descending saw |
|   timsort |  1000000 |  i32 | 0.006419 | 0.006468 |             |   descending saw |
|   pdqsort |  1000000 |  i32 | 0.016603 | 0.016629 |             |   descending saw |
|  wolfsort |  1000000 |  i32 | 0.006264 | 0.006329 |             |   descending saw |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.018675 | 0.018729 |             |      random tail |
|stablesort |  1000000 |  i32 | 0.026384 | 0.026508 |             |      random tail |
|   timsort |  1000000 |  i32 | 0.023226 | 0.023395 |             |      random tail |
|   pdqsort |  1000000 |  i32 | 0.028599 | 0.028674 |             |      random tail |
|  wolfsort |  1000000 |  i32 | 0.009517 | 0.009631 |             |      random tail |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.037593 | 0.037679 |             |      random half |
|stablesort |  1000000 |  i32 | 0.043755 | 0.043899 |             |      random half |
|   timsort |  1000000 |  i32 | 0.047008 | 0.047112 |             |      random half |
|   pdqsort |  1000000 |  i32 | 0.029800 | 0.029847 |             |      random half |
|  wolfsort |  1000000 |  i32 | 0.013238 | 0.013355 |             |      random half |
|           |          |      |          |          |             |                  |
|  quadsort |  1000000 |  i32 | 0.009605 | 0.009673 |             |       wave order |
|stablesort |  1000000 |  i32 | 0.013667 | 0.013785 |             |       wave order |
|   timsort |  1000000 |  i32 | 0.007994 | 0.008138 |             |       wave order |
|   pdqsort |  1000000 |  i32 | 0.024683 | 0.024727 |             |       wave order |
|  wolfsort |  1000000 |  i32 | 0.009642 | 0.009709 |             |       wave order |
</details>

The X axis of the graph below is the number of elements, the Y axis is the execution time.
![Graph](/graph2.png)
<details><summary><b>quadsort vs std::stable_sort vs timsort vs pdqsort vs wolfsort data table</b></summary>

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|  quadsort |      256 |  i32 | 0.008306 | 0.009226 |             |     random order |
|stablesort |      256 |  i32 | 0.009325 | 0.022037 |             |     random order |
|   timsort |      256 |  i32 | 0.015605 | 0.026554 |             |     random order |
|   pdqsort |      256 |  i32 | 0.010840 | 0.015047 |             |     random order |
|  wolfsort |      256 |  i32 | 0.008287 | 0.008338 |             |     random order |
|  flowsort |      256 |  i32 | 0.008332 | 0.009783 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |      512 |  i32 | 0.007497 | 0.012202 |             |     random order |
|stablesort |      512 |  i32 | 0.014719 | 0.023305 |             |     random order |
|   timsort |      512 |  i32 | 0.014791 | 0.026926 |             |     random order |
|   pdqsort |      512 |  i32 | 0.010218 | 0.015399 |             |     random order |
|  wolfsort |      512 |  i32 | 0.005434 | 0.005711 |             |     random order |
|  flowsort |      512 |  i32 | 0.008525 | 0.008842 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |     1024 |  i32 | 0.012224 | 0.013265 |             |     random order |
|stablesort |     1024 |  i32 | 0.027686 | 0.033828 |             |     random order |
|   timsort |     1024 |  i32 | 0.033677 | 0.043642 |             |     random order |
|   pdqsort |     1024 |  i32 | 0.011030 | 0.015985 |             |     random order |
|  wolfsort |     1024 |  i32 | 0.005497 | 0.007087 |             |     random order |
|  flowsort |     1024 |  i32 | 0.008267 | 0.009476 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |     4096 |  i32 | 0.040918 | 0.041427 |             |     random order |
|stablesort |     4096 |  i32 | 0.043393 | 0.044039 |             |     random order |
|   timsort |     4096 |  i32 | 0.059069 | 0.059289 |             |     random order |
|   pdqsort |     4096 |  i32 | 0.019076 | 0.020721 |             |     random order |
|  wolfsort |     4096 |  i32 | 0.007710 | 0.009736 |             |     random order |
|  flowsort |     4096 |  i32 | 0.010430 | 0.012855 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |    16384 |  i32 | 0.051287 | 0.051480 |             |     random order |
|stablesort |    16384 |  i32 | 0.052293 | 0.052393 |             |     random order |
|   timsort |    16384 |  i32 | 0.068145 | 0.068304 |             |     random order |
|   pdqsort |    16384 |  i32 | 0.024337 | 0.024414 |             |     random order |
|  wolfsort |    16384 |  i32 | 0.012334 | 0.012433 |             |     random order |
|  flowsort |    16384 |  i32 | 0.015112 | 0.015173 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |    65536 |  i32 | 0.058787 | 0.058863 |             |     random order |
|stablesort |    65536 |  i32 | 0.060166 | 0.060262 |             |     random order |
|   timsort |    65536 |  i32 | 0.076500 | 0.076612 |             |     random order |
|   pdqsort |    65536 |  i32 | 0.026368 | 0.026425 |             |     random order |
|  wolfsort |    65536 |  i32 | 0.013164 | 0.013208 |             |     random order |
|  flowsort |    65536 |  i32 | 0.015327 | 0.015362 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |   262144 |  i32 | 0.066391 | 0.066484 |             |     random order |
|stablesort |   262144 |  i32 | 0.068144 | 0.068255 |             |     random order |
|   timsort |   262144 |  i32 | 0.084703 | 0.084835 |             |     random order |
|   pdqsort |   262144 |  i32 | 0.028397 | 0.028457 |             |     random order |
|  wolfsort |   262144 |  i32 | 0.013937 | 0.014095 |             |     random order |
|  flowsort |   262144 |  i32 | 0.016058 | 0.016107 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |  1048576 |  i32 | 0.074302 | 0.074495 |             |     random order |
|stablesort |  1048576 |  i32 | 0.076274 | 0.076419 |             |     random order |
|   timsort |  1048576 |  i32 | 0.093351 | 0.093517 |             |     random order |
|   pdqsort |  1048576 |  i32 | 0.030378 | 0.030446 |             |     random order |
|  wolfsort |  1048576 |  i32 | 0.034210 | 0.034403 |             |     random order |
|  flowsort |  1048576 |  i32 | 0.017668 | 0.017795 |             |     random order |

</details>
