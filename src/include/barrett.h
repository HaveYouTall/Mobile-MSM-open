//
// Created by HaveYouTall
// 
#ifndef _BARRETT_H_
#define _BARRETT_H_
#include "dataType.h"

// When base is 2, MODULUS_BITS is BARRETT_K
#define MODULUS_BITS 377

// When base is 8, if BARRETT_K is changed, some part in the implementation of barrett should be changed either.
#define BARRETT_K 126 
//377
#define DIGIT_BITS 64
#define DIGIT_MASK 0x3f 

void rshift(uint64_t *r, size_t nlimbs ,size_t n);
void lshift(uint64_t *r, size_t nlimbs ,size_t n);

// r = r mod (2^n)
void mod_2n(uint64_t *r, size_t nlimbs, size_t n);

// This version has constant nlimbs 12 
/// @param x: is at most 12 * uint64_t 
/// @return res: it stores the final result, which is at most 6 * uint64_t
void barrett_384(const uint64_t x[12], uint64_t res[6]);

#endif 