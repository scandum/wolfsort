Intro
-----

This document describes a stable adaptive hybrid radix / merge sort named wolfsort.

Why a hybrid?
------------------------
While merge sort is very fast at the adaptive sorting of ordered data, its inability to effectively partition is its greatest weakness. Radix sort on the other hand is unable to take advantage of pre-ordered data. Wolfsort tries to avoid the worst case of each algorithm.

Adaptive partitioning
---------------------
Wolfsort uses [quadsort](https://github.com/scandum/quadsort "quadsort") as its merge sort, which is very fast, except for purely random arrays, the adaptive partitioning processes has two primary functions.

1. Detecting whether the array is worth partitioning.
2. Partition in a way that is beneficial to quadsort.

Detecting whether the array is worth partitioning
-------------------------------------------------

The default hash table size for 32 bit integers is 256 buckets. Each 32 bit integer is
divided by 16777216. The maximum bucket size is 1/32nd of the array size.

If the maximum bucket size is reached the partitioning process is cancelled and quadsort
is ran instead. The maximum bucket size is reached in three cases: 

1. Many values are under 256, in which case all the values gather in the first bucket.
2. Many values are not random, in which case quadsort is likely to be faster.
3. Many values are repetitions, in which case partitioning is inefficient.

In all these cases quadsort is typically faster by itself while an aborted partitioning
does not notably hamper performance.

Partition in a way that is beneficial to merge sort
---------------------------------------------------
Because this approach is the equivalent of an in-place MSD Radix sort the 256 buckets are
in order once partitioning completes. The next step is to sort the content of each bucket,
and wolfsort will be finished.

Memory overhead
---------------
Wolfsort requires 8 times the array size in swap memory for the O(n) partitioning process.
The sorting process that follows requires 1/32nd the array size in swap memory.

If not enough memory is available wolfsort falls back on quadsort which requires 1/2 the array
size in swap memory. An important thing to note is that while quite a bit of memory is
allocated most of it will remain unused, it's there just in case it's needed.

Virtual quantum partitioning
----------------------------

While the memory overhead may seem like a bad thing, it can be considered a form of virtual quantum computing. Most modern systems have several gigabytes of memory that are not used and are just sitting idle. During the partitioning process the swap memory becomes akin to Schrödinger's cat, it may be used, or it may not be used. Based on probability we know only 1/8th will be directly accessed, but because there is 7/8th to spare assumptions can be made that significantly reduce complexity and computations.

While more testing is needed it appears that in the 1K-100K element range wolfsort outperforms most sorting algorithms for random numbers, possibly turning spare memory into computing power.

Benchmark
---------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using g++ -O3 -fpermissive bench.c.
Each test was ran 100 times and only the best run is reported.
```
  quadsort: sorted 1000000 i32s in 0.071601 seconds.  (random order)
stablesort: sorted 1000000 i32s in 0.074852 seconds.  (random order)
   timsort: sorted 1000000 i32s in 0.088949 seconds.  (random order)
   pdqsort: sorted 1000000 i32s in 0.029686 seconds.  (random order)
  wolfsort: sorted 1000000 i32s in 0.018847 seconds.  (random order)

  quadsort: sorted 1000000 i32s in 0.009015 seconds.  (ascending saw)
stablesort: sorted 1000000 i32s in 0.017991 seconds.  (ascending saw)
   timsort: sorted 1000000 i32s in 0.008724 seconds.  (ascending saw)
   pdqsort: sorted 1000000 i32s in 0.042457 seconds.  (ascending saw)
  wolfsort: sorted 1000000 i32s in 0.016305 seconds.  (ascending saw)

  quadsort: sorted 1000000 i32s in 0.007562 seconds.  (descending saw)
stablesort: sorted 1000000 i32s in 0.015189 seconds.  (descending saw)
   timsort: sorted 1000000 i32s in 0.006679 seconds.  (descending saw)
   pdqsort: sorted 1000000 i32s in 0.016479 seconds.  (descending saw)
  wolfsort: sorted 1000000 i32s in 0.007287 seconds.  (descending saw)

  quadsort: sorted 1000000 i32s in 0.018925 seconds.  (random tail)
stablesort: sorted 1000000 i32s in 0.026586 seconds.  (random tail)
   timsort: sorted 1000000 i32s in 0.023148 seconds.  (random tail)
   pdqsort: sorted 1000000 i32s in 0.028902 seconds.  (random tail)
  wolfsort: sorted 1000000 i32s in 0.011490 seconds.  (random tail)

  quadsort: sorted 1000000 i32s in 0.037930 seconds.  (random half)
stablesort: sorted 1000000 i32s in 0.044522 seconds.  (random half)
   timsort: sorted 1000000 i32s in 0.046759 seconds.  (random half)
   pdqsort: sorted 1000000 i32s in 0.029573 seconds.  (random half)
  wolfsort: sorted 1000000 i32s in 0.016424 seconds.  (random half)
```

```
  quadsort: sorted 1000000 i32s in 0.071601 seconds.  (random order)
stablesort: sorted 1000000 i32s in 0.074852 seconds.  (random order)
   timsort: sorted 1000000 i32s in 0.088949 seconds.  (random order)
   pdqsort: sorted 1000000 i32s in 0.029686 seconds.  (random order)
  wolfsort: sorted 1000000 i32s in 0.021148 seconds.  (random order)

  quadsort: sorted  100000 i32s in 0.058524 seconds. (random order)
stablesort: sorted  100000 i32s in 0.060902 seconds. (random order)
   timsort: sorted  100000 i32s in 0.075829 seconds. (random order)
   pdqsort: sorted  100000 i32s in 0.026641 seconds. (random order)
  wolfsort: sorted  100000 i32s in 0.015566 seconds. (random order)

  quadsort: sorted   10000 i32s in 0.046235 seconds. (random order)
stablesort: sorted   10000 i32s in 0.047956 seconds. (random order)
   timsort: sorted   10000 i32s in 0.063083 seconds. (random order)
   pdqsort: sorted   10000 i32s in 0.023288 seconds. (random order)
  wolfsort: sorted   10000 i32s in 0.014106 seconds. (random order)

  quadsort: sorted    5000 i32s in 0.041448 seconds. (random order)
stablesort: sorted    5000 i32s in 0.044017 seconds. (random order)
   timsort: sorted    5000 i32s in 0.058817 seconds. (random order)
   pdqsort: sorted    5000 i32s in 0.020567 seconds. (random order)
  wolfsort: sorted    5000 i32s in 0.011412 seconds. (random order)

  quadsort: sorted     500 i32s in 0.000366 seconds. (random order)
stablesort: sorted     500 i32s in 0.000516 seconds. (random order)
   timsort: sorted     500 i32s in 0.000697 seconds. (random order)
   pdqsort: sorted     500 i32s in 0.000453 seconds. (random order)
  wolfsort: sorted     500 i32s in 0.000364 seconds. (random order)
```
