Intro
-----

This document describes a stable adaptive hybrid radix / merge sort named wolfsort. It
is likely the fastest sort written so far for sorting a mixture of random and ordered data.

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
Wolfsort is primarily a proof of concept, currently it only properly supports unsigned 32 bit integers.

Memory allocation with malloc() is a bottleneck on many platforms. Using a better memory allocator
or permanent swap memory is a possible solution. 

Flowsort
--------
Flowsort is slightly slower than wolfsort but has 8 times less memory overhead. When sorting more
than 1 million items it becomes slightly faster because it has better cache utilization.

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
|  quadsort |   100000 |  i32 | 0.005800 | 0.005835 |             |     random order |
|stablesort |   100000 |  i32 | 0.006092 | 0.006122 |             |     random order |
|   timsort |   100000 |  i32 | 0.007605 | 0.007656 |             |     random order |
|   pdqsort |   100000 |  i32 | 0.002648 | 0.002670 |             |     random order |
|  wolfsort |   100000 |  i32 | 0.001148 | 0.001168 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |    10000 |  i32 | 0.004568 | 0.004593 |             |     random order |
|stablesort |    10000 |  i32 | 0.004813 | 0.004923 |             |     random order |
|   timsort |    10000 |  i32 | 0.006326 | 0.006376 |             |     random order |
|   pdqsort |    10000 |  i32 | 0.002345 | 0.002373 |             |     random order |
|  wolfsort |    10000 |  i32 | 0.001256 | 0.001274 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |     5000 |  i32 | 0.004076 | 0.004086 |             |     random order |
|stablesort |     5000 |  i32 | 0.004394 | 0.004420 |             |     random order |
|   timsort |     5000 |  i32 | 0.005901 | 0.005938 |             |     random order |
|   pdqsort |     5000 |  i32 | 0.002093 | 0.002107 |             |     random order |
|  wolfsort |     5000 |  i32 | 0.000968 | 0.001086 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |     2500 |  i32 | 0.003196 | 0.003303 |             |     random order |
|stablesort |     2500 |  i32 | 0.003801 | 0.003942 |             |     random order |
|   timsort |     2500 |  i32 | 0.005296 | 0.005322 |             |     random order |
|   pdqsort |     2500 |  i32 | 0.001606 | 0.001661 |             |     random order |
|  wolfsort |     2500 |  i32 | 0.000509 | 0.000520 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |     1250 |  i32 | 0.001767 | 0.001823 |             |     random order |
|stablesort |     1250 |  i32 | 0.002812 | 0.002887 |             |     random order |
|   timsort |     1250 |  i32 | 0.003789 | 0.003865 |             |     random order |
|   pdqsort |     1250 |  i32 | 0.001036 | 0.001325 |             |     random order |
|  wolfsort |     1250 |  i32 | 0.000534 | 0.000655 |             |     random order |
|           |          |      |          |          |             |                  |
|  quadsort |      675 |  i32 | 0.000368 | 0.000592 |             |     random order |
|stablesort |      675 |  i32 | 0.000974 | 0.001160 |             |     random order |
|   timsort |      675 |  i32 | 0.000896 | 0.000948 |             |     random order |
|   pdqsort |      675 |  i32 | 0.000489 | 0.000531 |             |     random order |
|  wolfsort |      675 |  i32 | 0.000283 | 0.000308 |             |     random order |
</details>
