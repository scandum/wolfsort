// skipsort 1.2.1.3 - Igor van den Hoven ivdhoven@gmail.com

#ifndef SKIPSORT_H
#define SKIPSORT_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

typedef int CMPFUNC (const void *a, const void *b);

//#define cmp(a,b) (*(a) > *(b))

#ifndef QUADSORT_H
  #include "quadsort.h"
#endif
#ifndef FLUXSORT_H
  #include "fluxsort.h"
#endif

// When sorting an array of pointers, like a string array, QUAD_CACHE needs to
// be adjusted in quadsort.h for proper performance when sorting large arrays.


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

#include "skipsort.c"

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

#include "skipsort.c"

#undef VAR
#undef FUNC

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

#define VAR int
#define FUNC(NAME) NAME##32

#include "skipsort.c"

#undef VAR
#undef FUNC

#ifndef cmp
  #define cmp(a,b) (*(a) > *(b))

  #define VAR int
  #define FUNC(NAME) NAME##_int32

  #include "skipsort.c"

  #undef VAR
  #undef FUNC

  #undef cmp
#endif

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

#define VAR long long
#define FUNC(NAME) NAME##64

#include "skipsort.c"

#undef VAR
#undef FUNC

#ifndef cmp
  #define cmp(a,b) (*(a) > *(b))

  #define VAR long long
  #define FUNC(NAME) NAME##_int64

  #include "skipsort.c"

  #undef VAR
  #undef FUNC

  #undef cmp
#endif

//////////////////////////////////////////////////////////
//┌────────────────────────────────────────────────────┐//
//│  ▄██┐  ██████┐  █████┐    ██████┐ ██████┐████████┐ │//
//│ ████│  └────██┐██┌──██┐   ██┌──██┐└─██┌─┘└──██┌──┘ │//
//│ └─██│   █████┌┘└█████┌┘   ██████┌┘  ██│     ██│    │//
//│   ██│  ██┌───┘ ██┌──██┐   ██┌──██┐  ██│     ██│    │//
//│ ██████┐███████┐└█████┌┘   ██████┌┘██████┐   ██│    │//
//│ └─────┘└──────┘ └────┘    └─────┘ └─────┘   └─┘    │//
//└────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#define VAR long double
#define FUNC(NAME) NAME##128

#include "skipsort.c"

#undef VAR
#undef FUNC

////////////////////////////////////////////////////////////////////////
//┌──────────────────────────────────────────────────────────────────┐//
//│███████┐██┐  ██┐██████┐██████┐ ███████┐ ██████┐ ██████┐ ████████┐ │//
//│██┌────┘██│ ██┌┘└─██┌─┘██┌──██┐██┌────┘██┌───██┐██┌──██┐└──██┌──┘ │//
//│███████┐█████┌┘   ██│  ██████┌┘███████┐██│   ██│██████┌┘   ██│    │//
//│└────██│██┌─██┐   ██│  ██┌───┘ └────██│██│   ██│██┌──██┐   ██│    │//
//│███████│██│  ██┐██████┐██│     ███████│└██████┌┘██│  ██│   ██│    │//
//│└──────┘└─┘  └─┘└─────┘└─┘     └──────┘ └─────┘ └─┘  └─┘   └─┘    │//
//└──────────────────────────────────────────────────────────────────┘//
////////////////////////////////////////////////////////////////////////

void skipsort(void *array, size_t nmemb, size_t size, CMPFUNC *cmp)
{
	if (nmemb < 2)
	{
		return;
	}
#ifndef cmp
	if (cmp == NULL)
	{
		switch (size)
		{
			case sizeof(int):
				return skipsort_int32(array, nmemb, cmp);
			case sizeof(long long):
				return skipsort_int64(array, nmemb, cmp);
		}
		return assert(size == sizeof(int));
	}
#endif

	switch (size)
	{
		case sizeof(char):
			return skipsort8(array, nmemb, cmp);

		case sizeof(short):
			return skipsort16(array, nmemb, cmp);

		case sizeof(int):
			return skipsort32(array, nmemb, cmp);

		case sizeof(long long):
			return skipsort64(array, nmemb, cmp);

		case sizeof(long double):
			return skipsort128(array, nmemb, cmp);

		default:
			return assert(size == sizeof(char) || size == sizeof(short) || size == sizeof(int) || size == sizeof(long long) || size == sizeof(long double));
	}
}

#endif
