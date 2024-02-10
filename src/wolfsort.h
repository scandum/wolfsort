// wolfsort 1.2.1.3 - Igor van den Hoven ivdhoven@gmail.com

#ifndef WOLFSORT_H
#define WOLFSORT_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdalign.h>

typedef int CMPFUNC (const void *a, const void *b);

//#define cmp(a,b) (*(a) > *(b))

// When sorting an array of pointers, like a string array, the QUAD_CACHE needs
// to be set for proper performance when sorting large arrays.
// wolfsort_prim() can be used to sort 32 and 64 bit primitives.

// With a 6 MB L3 cache a value of 262144 works well.

#ifdef cmp
  #define QUAD_CACHE 4294967295
#else
//#define QUAD_CACHE 131072
  #define QUAD_CACHE 262144
//#define QUAD_CACHE 524288
//#define QUAD_CACHE 4294967295
#endif

#ifndef FLUXSORT_H
  #include "fluxsort.h"
#endif

//////////////////////////////////////////////////////////
// ┌───────────────────────────────────────────────────┐//
// │       ██████┐ ██████┐    ██████┐ ██████┐████████┐ │//
// │       └────██┐└────██┐   ██┌──██┐└─██┌─┘└──██┌──┘ │//
// │        █████┌┘ █████┌┘   ██████┌┘  ██│     ██│    │//
// │        └───██┐██┌───┘    ██┌──██┐  ██│     ██│    │//
// │       ██████┌┘███████┐   ██████┌┘██████┐   ██│    │//
// │       └─────┘ └──────┘   └─────┘ └─────┘   └─┘    │//
// └───────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////
/*
#define VAR int
#define FUNC(NAME) NAME##32

#include "wolfsort.c"

#undef VAR
#undef FUNC
*/
// wolfsort_prim

#define VAR int
#define FUNC(NAME) NAME##_int32
#ifndef cmp
  #define cmp(a,b) (*(a) > *(b))
  #include "wolfsort.c"
  #undef cmp
#else
  #include "wolfsort.c"
#endif
#undef VAR
#undef FUNC

#define VAR unsigned int
#define FUNC(NAME) NAME##_uint32
#ifndef cmp
  #define cmp(a,b) (*(a) > *(b))
  #include "wolfsort.c"
  #undef cmp
#else
  #include "wolfsort.c"
#endif
#undef VAR
#undef FUNC

//////////////////////////////////////////////////////////
// ┌───────────────────────────────────────────────────┐//
// │        █████┐ ██┐  ██┐   ██████┐ ██████┐████████┐ │//
// │       ██┌───┘ ██│  ██│   ██┌──██┐└─██┌─┘└──██┌──┘ │//
// │       ██████┐ ███████│   ██████┌┘  ██│     ██│    │//
// │       ██┌──██┐└────██│   ██┌──██┐  ██│     ██│    │//
// │       └█████┌┘     ██│   ██████┌┘██████┐   ██│    │//
// │        └────┘      └─┘   └─────┘ └─────┘   └─┘    │//
// └───────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////
/*
#define VAR long long
#define FUNC(NAME) NAME##64

#include "wolfsort.c"

#undef VAR
#undef FUNC
*/
// wolfsort_prim

#define VAR long long
#define FUNC(NAME) NAME##_int64
#ifndef cmp
  #define cmp(a,b) (*(a) > *(b))
  #include "wolfsort.c"
  #undef cmp
#else
  #include "wolfsort.c"
#endif
#undef VAR
#undef FUNC

#define VAR unsigned long long
#define FUNC(NAME) NAME##_uint64
#ifndef cmp
  #define cmp(a,b) (*(a) > *(b))
  #include "wolfsort.c"
  #undef cmp
#else
  #include "wolfsort.c"
#endif
#undef VAR
#undef FUNC

// This section is outside of 32/64 bit pointer territory, so no cache checks
// necessary, unless sorting 32+ byte structures.

#undef QUAD_CACHE
#define QUAD_CACHE 4294967295

//////////////////////////////////////////////////////////
//┌────────────────────────────────────────────────────┐//
//│                █████┐    ██████┐ ██████┐████████┐  │//
//│               ██┌──██┐   ██┌──██┐└─██┌─┘└──██┌──┘  │//
//│               └█████┌┘   ██████┌┘  ██│     ██│     │//
//│               ██┌──██┐   ██┌──██┐  ██│     ██│     │//
//│               └█████┌┘   ██████┌┘██████┐   ██│     │//
//│                └────┘    └─────┘ └─────┘   └─┘     │//
//└────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#define VAR char
#define FUNC(NAME) NAME##8

#include "wolfsort.c"

#undef VAR
#undef FUNC

//////////////////////////////////////////////////////////
//┌────────────────────────────────────────────────────┐//
//│           ▄██┐   █████┐    ██████┐ ██████┐████████┐│//
//│          ████│  ██┌───┘    ██┌──██┐└─██┌─┘└──██┌──┘│//
//│          └─██│  ██████┐    ██████┌┘  ██│     ██│   │//
//│            ██│  ██┌──██┐   ██┌──██┐  ██│     ██│   │//
//│          ██████┐└█████┌┘   ██████┌┘██████┐   ██│   │//
//│          └─────┘ └────┘    └─────┘ └─────┘   └─┘   │//
//└────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#define VAR short
#define FUNC(NAME) NAME##16

#include "wolfsort.c"

#undef VAR
#undef FUNC

///////////////////////////////////////////////////////////
//┌─────────────────────────────────────────────────────┐//
//│ ██████┐██┐   ██┐███████┐████████┐ ██████┐ ███┐  ███┐│//
//│██┌────┘██│   ██│██┌────┘└──██┌──┘██┌───██┐████┐████││//
//│██│     ██│   ██│███████┐   ██│   ██│   ██│██┌███┌██││//
//│██│     ██│   ██│└────██│   ██│   ██│   ██│██│└█┌┘██││//
//│└██████┐└██████┌┘███████│   ██│   └██████┌┘██│ └┘ ██││//
//│ └─────┘ └─────┘ └──────┘   └─┘    └─────┘ └─┘    └─┘│//
//└─────────────────────────────────────────────────────┘//
///////////////////////////////////////////////////////////

/*
typedef struct {char bytes[32];} struct256;
#define VAR struct256
#define FUNC(NAME) NAME##256

#include "wolfsort.c"

#undef VAR
#undef FUNC
*/

 //////////////////////////////////////////////////////////////////////////
//┌─────────────────────────────────────────────────────────────────────┐//
//│██┐    ██┐ ██████┐ ██┐     ███████┐███████┐ ██████┐ ██████┐ ████████┐│//
//│██│    ██│██┌───██┐██│     ██┌────┘██┌────┘██┌───██┐██┌──██┐└──██┌──┘│//
//│██│ █┐ ██│██│   ██│██│     █████┐  ███████┐██│   ██│██████┌┘   ██│   │//
//│██│███┐██│██│   ██│██│     ██┌──┘  └────██│██│   ██│██┌──██┐   ██│   │//
//│└███┌███┌┘└██████┌┘███████┐██│     ███████│└██████┌┘██│  ██│   ██│   │//
//│ └──┘└──┘  └─────┘ └──────┘└─┘     └──────┘ └─────┘ └─┘  └─┘   └─┘   │//
//└─────────────────────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////////////////////

void wolfsort(void *array, size_t nmemb, size_t size, CMPFUNC *cmp)
{
	if (nmemb < 2)
	{
		return;
	}

	switch (size)
	{
		case sizeof(char):
			wolfsort8(array, nmemb, cmp);
			return;

		case sizeof(short):
			wolfsort16(array, nmemb, cmp);
			return;

		case sizeof(int):
			wolfsort_uint32(array, nmemb, cmp);
			return;

		case sizeof(long long):
			wolfsort_uint64(array, nmemb, cmp);
//			fluxsort64(array, nmemb, cmp); // fluxsort generally beats wolfsort for 64+ bit types
			return;

		case sizeof(long double):
			fluxsort128(array, nmemb, cmp);
			return;

//		case sizeof(struct256):
//			wolfsort256(array, nmemb, cmp);
			return;

		default:
			assert(size == sizeof(char) || size == sizeof(short) || size == sizeof(int) || size == sizeof(long long) || size == sizeof(long double));
//			qsort(array, nmemb, size, cmp);
	}
}

// suggested size values for primitives:

//		case  0: unsigned char
//		case  1: signed char
//		case  2: signed short
//		case  3: unsigned short
//		case  4: signed int
//		case  5: unsigned int
//		case  6: float
//		case  7: double
//		case  8: signed long long
//		case  9: unsigned long long
//		case 16: long double

void wolfsort_prim(void *array, size_t nmemb, size_t size)
{
	if (nmemb < 2)
	{
		return;
	}

	switch (size)
	{
		case 4:
			fluxsort_int32(array, nmemb, NULL);
			return;
		case 8:
			fluxsort_int64(array, nmemb, NULL);
			return;
		case 5:
			wolfsort_uint32(array, nmemb, NULL);
			return;
		case 9:
			wolfsort_uint64(array, nmemb, NULL);
			return;
		default:
			assert(size == sizeof(int) || size == sizeof(long long) || size == sizeof(int) + 1 || size == sizeof(long long) + 1);
			return;
	}
}

#undef QUAD_CACHE

#endif
