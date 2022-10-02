#ifndef HEADERS_DATATYPE_H
#define HEADERS_DATATYPE_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


// Use montgomery data form.
#define MONTG
// Use asm montgomery implementation
#define MONTG_ASM
// Use asm field operation.
#define ASM

// Use Twisted Edwards Curve.
// We are now using Edwards Curve.
#define Edwards


typedef uint64_t	u8;
typedef uint32_t	u4;
typedef uint16_t	u2;
typedef uint8_t		u1;



typedef struct
{
	u8 v[4];
} u32;

typedef struct
{
	u8 v[6];
} u48;

typedef struct
{
	u48 x;
	u48 y;
} AFPoint;

typedef struct
{
#ifdef Edwards
	u48 x;
	u48 y;
	u48 z;
	u48 t;
#else
	u48 x;
	u48 y;
	u48 z;
#endif
} JPoint;

#endif