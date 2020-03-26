Intro
-----

This document describes a stable adaptive hybrid radix / merge sort named wolfsort.

Quadsort
--------
[quadsort]((../quadsort "quadsort") is used for sorting arrays with fewer than 512
elements, the partitions creates by wolfsort, as well as to sort any array that
fails partitioning.

Adaptive partitioning
---------------------
Since quadsort is very fast, except for purely random arrays, the partitioning processes
has two primary functions.

1. Detecting whether the array is worth partitioning.
2. Partition in a way that is beneficial to quadsort.

Detecting whether the array is worth partitioning
-------------------------------------------------

The default hash table size for 32 bit integers is 256 buckets. Each 32 bit integer is
divided by 16777216. The maximum bucket size is 1/32nd of the array size.

If the maximum bucket size is reached the partitioning process is cancelled and quadsort
is ran instead. The maximum bucket size is reached in three cases: 

1. All values are under 256, in which case partitioning is pointless.
2. The values are not random, in which case quadsort is faster.
3. There are a lot of repeat values, in which case partitioning is inefficient.

In all these cases quadsort is typically faster by itself.

Partition in a way that is beneficial to quadsort
-------------------------------------------------
After partitioning completes the 256 buckets are in order, so all that's needed to finish up is to sort each buckets, and wolfsort is done.

Memory overhead
---------------
Wolfsort requires 8 times the array size in swap memory for the O(n) partitioning process, the following sorting process requires less than 1/32nd the array size in swap memory.

If not enough memory is available wolfsort falls back on quadsort which requires 1/2 the array size in swap memory. An important thing to note is that while quite a bit of memory is allocated most of it will remain untouched and unused, it's there just in case it's needed.

Quantum partitioning
--------------------

While the memory overhead may seem like a bad thing, it can be considered a form of quantum computing. Most modern systems have several gigabytes of memory that are not used and are just sitting there idle. During the partitioning process the swap memory becomes akin to Schrödinger's cat, it may be used, or it may not be used, based on probability we know only 1/8th will be directly accessed, but because there is 7/8th to spare assumptions can be made that significantly reduce complexity and computations.

While more testing is needed it appears that in the 1K-100K element range wolfsort outperforms all currently existing sorts for random numbers, turning spare memory into computing power.
