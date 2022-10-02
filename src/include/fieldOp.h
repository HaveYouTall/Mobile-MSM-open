//
// Created by HaveYouTall
//
#ifndef HEADERS_FN_H
#define HEADERS_FN_H

#include "basicOp.h"

void mul_mod_p(const uint64_t x[6], const uint64_t y[6], uint64_t result[6]);
// mul_mod_p_barrett will always used barrett reduction.
void mul_mod_p_barrett(const uint64_t x[6], const uint64_t y[6], uint64_t result[6]); 
void pow_mod_p(const uint64_t x[6], uint64_t result[6]);
void inv_for_mul(const u48* input, u48* result);

#ifdef ASM
extern "C" { void add_mod_p(const uint64_t a[6], const uint64_t b[6], uint64_t result[6]); }
extern "C" { void sub_mod_p(const uint64_t a[6], const uint64_t b[6], uint64_t result[6]);}
extern "C" { void neg_mod_p(const uint64_t a[6], uint64_t result[6]);}
extern "C" { void double_mod_p(const uint64_t a[6], uint64_t result[6]);}
extern "C" { void mul_by_3_mod_p(const uint64_t a[6], uint64_t result[6]);}
extern "C" { void div_by_2_mod_p(const uint64_t a[6], uint64_t result[6]);}
#else
void neg_mod_p(const uint64_t a[6], uint64_t result[6]);
void double_mod_p(const uint64_t a[6], uint64_t result[6]);
void mul_by_3_mod_p(const uint64_t a[6], uint64_t result[6]);
void add_mod_p(const uint64_t a[6], const uint64_t b[6], uint64_t result[6]);
void div_by_2_mod_p(const uint64_t a[6], uint64_t result[6]);
void sub_mod_p(const uint64_t a[6], const uint64_t b[6], uint64_t result[6]);
#endif

#endif

