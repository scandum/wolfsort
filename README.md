Intro
-----

This document describes a stable adaptive hybrid radix / quicksort / merge sort named wolfsort.

Why a hybrid?
-------------
While an adaptive merge sort is very fast at sorting ordered data, its inability to effectively
partition is its greatest weakness. Radix sort on the other hand is unable to take advantage of
sorted data. While quicksort is fast at partitioning, a radix counting sort is faster on
medium-sized arrays in the 1K - 1M element range.

Fluxsort
--------
Wolfsort uses [fluxsort](https://github.com/scandum/fluxsort "fluxsort") for sorting partitioned
arrays. Fluxsort is a stable hybrid quicksort / mergesort which sorts random data 33% faster than
merge sort, while ordered data is sorted up to an order of magnitude faster.

Detecting whether the array is worth partitioning
-------------------------------------------------
Wolfsort operates like a typical radix sort by creating 256 buckets and dividing each unsigned
32 bit integer by 16777216. Without any optimizations this would multiply the memory overhead
by 256 times. Wolfsort solves this problem by running over the data once to find out the final
size of each bucket, it then only needs n auxiliary memory and perform one more run
to finish partitioning.

If the bucket distribution is sub-optimal the radix sort is aborted and fluxsort is ran instead.
While this may seem wasteful it doesn't take much time in practice.

Partition in a way that is beneficial to comparison sorts
---------------------------------------------------------
Because this approach is the equivalent of an MSD Radix sort the 256 buckets are
in order once partitioning completes. The next step is to sort the content of each bucket
using fluxsort and wolfsort will be finished.

Memory overhead
---------------
Wolfsort requires O(n) memory for the partitioning process and O(sqrt n) memory for the buckets.

If not enough memory is available wolfsort falls back on fluxsort which requires n swap memory,
and if that's not sufficient fluxsort falls back on quadsort which can sort in-place.

Proof of concept
----------------
Wolfsort is primarily a proof of concept for a hybrid radix / comparison sort, currently it only supports unsigned 32 bit integers.

Other radix sorts of interest are [ska sort](https://github.com/skarupke/ska_sort) and [Robin Hood Sort](https://github.com/mlochbaum/rhsort).

I'll briefly mention other sorting algorithms listed in the benchmark code / graphs.

Blitsort
--------
Blitsort is a hybrid in-place stable adaptive rotate quick / merge sort.

Crumsort
--------
Crumsort is a hybrid in-place unstable adaptive quick / rotate merge sort.

Quadsort
--------
Quadsort is an adaptive mergesort. It supports rotations as a fall-back to sort in-place.

Gridsort
--------
Gridsort is a stable comparison sort which stores data in a 2 dimensional self-balancing grid.

Fluxsort
--------
Fluxsort is a hybrid stable branchless out-of-place quick / merge sort.

Pdqsort
-------
Pdqsort is a hybrid unstable branchless introsort. It serves as a reference for the performance of a relatively pure quicksort.

Big O
-----
```cobol
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
│pdqsort        ││n      │n log n│n log n││1     │1     │1     ││no    ││yes      ││semi     ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│quadsort       ││n      │n log n│n log n││1     │n     │n     ││yes   ││no       ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├──────┼──────┼──────┤├──────┤├─────────┤├─────────┤├─────────┤
│wolfsort       ││n      │n log n│n log n││n     │n     │n     ││yes   ││yes      ││yes      ││no       │
└───────────────┘└───────┴───────┴───────┘└──────┴──────┴──────┘└──────┘└─────────┘└─────────┘└─────────┘
```

fluxsort vs gridsort vs quadsort vs wolfsort on 100K elements
-------------------------------------------------------------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using g++ -O3 -fpermissive bench.c. All comparisons are inlined through the cmp macro.
Each test was ran 100 times with the best run and average reported.

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
Each test was ran 100 times with the best run and average reported.

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
