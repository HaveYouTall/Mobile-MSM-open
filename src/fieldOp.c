//
// Created by HaveYouTall
//
#include "fieldOp.h"
#include "barrett.h"
#include "montgomery.h"
#include "const.h"
#include "basicOp.h"

#ifndef ASM
// result = a + b mod MODULUS
// In montg domain: result = MONT(a) + MONT(b) mod MODULUS
void add_mod_p(const uint64_t a[6], const uint64_t b[6], uint64_t result[6]) {
	u1 carry = 0;
	uint64_t tmp_res[6];
	// a + b 
	carry = _addcarryx_u64(carry, a[0], b[0], tmp_res + 0);
	carry = _addcarryx_u64(carry, a[1], b[1], tmp_res + 1);
	carry = _addcarryx_u64(carry, a[2], b[2], tmp_res + 2);
	carry = _addcarryx_u64(carry, a[3], b[3], tmp_res + 3);
	carry = _addcarryx_u64(carry, a[4], b[4], tmp_res + 4);
	carry = _addcarryx_u64(carry, a[5], b[5], tmp_res + 5);
	
	carry = 1;
	carry = _addcarryx_u64(carry, tmp_res[0], ~MODULUS[0], result);
	carry = _addcarryx_u64(carry, tmp_res[1], ~MODULUS[1], result + 1);
	carry = _addcarryx_u64(carry, tmp_res[2], ~MODULUS[2], result + 2);
	carry = _addcarryx_u64(carry, tmp_res[3], ~MODULUS[3], result + 3);
	carry = _addcarryx_u64(carry, tmp_res[4], ~MODULUS[4], result + 4);
	carry = _addcarryx_u64(carry, tmp_res[5], ~MODULUS[5], result + 5);

	// If tmp_res - MODULUS < 0
	if(!carry) {
		for(int i = 0; i<6; i++){
			result[i] = tmp_res[i];
		}
	}
}

// result = a - b mod MODULUS
// In montg domain: result = MONT(a) - MONT(b) mod MODULUS
void sub_mod_p(const uint64_t a[6], const uint64_t b[6], uint64_t result[6]) {
	u1 carry = 1;
	// uint64_t tmp_res[6];
	// result = a - b
	carry = _addcarryx_u64(carry, a[0], ~b[0], result + 0);
	carry = _addcarryx_u64(carry, a[1], ~b[1], result + 1);
	carry = _addcarryx_u64(carry, a[2], ~b[2], result + 2);
	carry = _addcarryx_u64(carry, a[3], ~b[3], result + 3);
	carry = _addcarryx_u64(carry, a[4], ~b[4], result + 4);
	carry = _addcarryx_u64(carry, a[5], ~b[5], result + 5);
	
	// if a - b < 0
	if(!carry) {
		// result + modulus
		carry = 0;
		carry = _addcarryx_u64(carry, result[0], MODULUS[0], result);
		carry = _addcarryx_u64(carry, result[1], MODULUS[1], result + 1);
		carry = _addcarryx_u64(carry, result[2], MODULUS[2], result + 2);
		carry = _addcarryx_u64(carry, result[3], MODULUS[3], result + 3);
		carry = _addcarryx_u64(carry, result[4], MODULUS[4], result + 4);
		carry = _addcarryx_u64(carry, result[5], MODULUS[5], result + 5);
	}
}

void neg_mod_p(const uint64_t a[6], uint64_t result[6]) {
	u1 carry = 1;
	// result = -a 
	carry = _addcarryx_u64(carry, ~a[0], 0, result + 0);
	carry = _addcarryx_u64(carry, ~a[1], 0, result + 1);
	carry = _addcarryx_u64(carry, ~a[2], 0, result + 2);
	carry = _addcarryx_u64(carry, ~a[3], 0, result + 3);
	carry = _addcarryx_u64(carry, ~a[4], 0, result + 4);
	carry = _addcarryx_u64(carry, ~a[5], 0, result + 5);
	
	//  -a < 0
	// result + modulus
	carry = 0;
	carry = _addcarryx_u64(carry, result[0], MODULUS[0], result);
	carry = _addcarryx_u64(carry, result[1], MODULUS[1], result + 1);
	carry = _addcarryx_u64(carry, result[2], MODULUS[2], result + 2);
	carry = _addcarryx_u64(carry, result[3], MODULUS[3], result + 3);
	carry = _addcarryx_u64(carry, result[4], MODULUS[4], result + 4);
	carry = _addcarryx_u64(carry, result[5], MODULUS[5], result + 5);
}

// result = a / 2 mod MODULUS
void div_by_2_mod_p(const uint64_t a[6], uint64_t result[6]) {
	// Make sure the last bit is 0
	if (a[0] & (uint64_t)(1)) { // If last bit is 1, than add Modulus.
		// Add MODULUS
		u1 carry = 0;
		carry = _addcarryx_u64(carry, a[0], MODULUS[0], result);
		carry = _addcarryx_u64(carry, a[1], MODULUS[1], result + 1);
		carry = _addcarryx_u64(carry, a[2], MODULUS[2], result + 2);
		carry = _addcarryx_u64(carry, a[3], MODULUS[3], result + 3);
		carry = _addcarryx_u64(carry, a[4], MODULUS[4], result + 4);
		carry = _addcarryx_u64(carry, a[5], MODULUS[5], result + 5);
	}

	// Right shift 1 bit.
	result[0] = (result[0] >> 1) | (result[1] << 63);
	result[1] = (result[1] >> 1) | (result[2] << 63);
	result[2] = (result[2] >> 1) | (result[3] << 63);
	result[3] = (result[3] >> 1) | (result[4] << 63);
	result[4] = (result[4] >> 1) | (result[5] << 63);
	result[5] >>= 1;
}

void double_mod_p(const uint64_t a[6], uint64_t result[6]) {
	uint64_t tmp_res[6];
	tmp_res[5] = (a[5] << 1) | (a[4] >> 63);
	tmp_res[4] = (a[4] << 1) | (a[3] >> 63);
	tmp_res[3] = (a[3] << 1) | (a[2] >> 63);
	tmp_res[2] = (a[2] << 1) | (a[1] >> 63);
	tmp_res[1] = (a[1] << 1) | (a[0] >> 63);
	tmp_res[0] = a[0] << 1;
	
	// cal (a * 2 mod MODULUS )
	// result = tmp_res - MODULUS
	u1 carry = 1;
	carry = _addcarryx_u64(carry, tmp_res[0], ~MODULUS[0], result);
	carry = _addcarryx_u64(carry, tmp_res[1], ~MODULUS[1], result + 1);
	carry = _addcarryx_u64(carry, tmp_res[2], ~MODULUS[2], result + 2);
	carry = _addcarryx_u64(carry, tmp_res[3], ~MODULUS[3], result + 3);
	carry = _addcarryx_u64(carry, tmp_res[4], ~MODULUS[4], result + 4);
	carry = _addcarryx_u64(carry, tmp_res[5], ~MODULUS[5], result + 5);

	// If tmp_res - MODULUS < 0
	if(!carry) {
		for(int i = 0; i<6; i++){
			result[i] = tmp_res[i];
		}
	}

}


void mul_by_3_mod_p(const uint64_t a[6], uint64_t result[6]) {
	// Because a < MODULUS, which only has 380 bits, than a[5] >> 1 is at most 381 bits (less than 384 bits, won't generate carry.)
	uint64_t tmp_res[6], tmp_a[6];
	for(int i = 0; i < 6;i++){ // fix bug, when result is set as address of a.
		tmp_a[i] = a[i];
	}
	tmp_res[5] = (tmp_a[5] << 1) | (tmp_a[4] >> 63);
	tmp_res[4] = (tmp_a[4] << 1) | (tmp_a[3] >> 63);
	tmp_res[3] = (tmp_a[3] << 1) | (tmp_a[2] >> 63);
	tmp_res[2] = (tmp_a[2] << 1) | (tmp_a[1] >> 63);
	tmp_res[1] = (tmp_a[1] << 1) | (tmp_a[0] >> 63);
	tmp_res[0] = tmp_a[0] << 1;
	
	// cal (a * 2 mod MODULUS )
	// result = tmp_res - MODULUS
	u1 carry = 1;
	carry = _addcarryx_u64(carry, tmp_res[0], ~MODULUS[0], result);
	carry = _addcarryx_u64(carry, tmp_res[1], ~MODULUS[1], result + 1);
	carry = _addcarryx_u64(carry, tmp_res[2], ~MODULUS[2], result + 2);
	carry = _addcarryx_u64(carry, tmp_res[3], ~MODULUS[3], result + 3);
	carry = _addcarryx_u64(carry, tmp_res[4], ~MODULUS[4], result + 4);
	carry = _addcarryx_u64(carry, tmp_res[5], ~MODULUS[5], result + 5);

	// cal (a * 2 mod MODULUS )  + a
	if(!carry) { // If tmp_res - MODULUS < 0, use tmp_res
		carry = 0;
		carry = _addcarryx_u64(carry, tmp_a[0], tmp_res[0], tmp_res);
		carry = _addcarryx_u64(carry, tmp_a[1], tmp_res[1], tmp_res + 1);
		carry = _addcarryx_u64(carry, tmp_a[2], tmp_res[2], tmp_res + 2);
		carry = _addcarryx_u64(carry, tmp_a[3], tmp_res[3], tmp_res + 3);
		carry = _addcarryx_u64(carry, tmp_a[4], tmp_res[4], tmp_res + 4);
		carry = _addcarryx_u64(carry, tmp_a[5], tmp_res[5], tmp_res + 5);
	} else { // use result = tmp_res - MODULUS
		carry = 0;
		carry = _addcarryx_u64(carry, tmp_a[0], result[0], tmp_res);
		carry = _addcarryx_u64(carry, tmp_a[1], result[1], tmp_res + 1);
		carry = _addcarryx_u64(carry, tmp_a[2], result[2], tmp_res + 2);
		carry = _addcarryx_u64(carry, tmp_a[3], result[3], tmp_res + 3);
		carry = _addcarryx_u64(carry, tmp_a[4], result[4], tmp_res + 4);
		carry = _addcarryx_u64(carry, tmp_a[5], result[5], tmp_res + 5);
	}
	carry = 1;
	carry = _addcarryx_u64(carry, tmp_res[0], ~MODULUS[0], result);
	carry = _addcarryx_u64(carry, tmp_res[1], ~MODULUS[1], result + 1);
	carry = _addcarryx_u64(carry, tmp_res[2], ~MODULUS[2], result + 2);
	carry = _addcarryx_u64(carry, tmp_res[3], ~MODULUS[3], result + 3);
	carry = _addcarryx_u64(carry, tmp_res[4], ~MODULUS[4], result + 4);
	carry = _addcarryx_u64(carry, tmp_res[5], ~MODULUS[5], result + 5);

	// If tmp_res - MODULUS < 0
	if(!carry) {
		for(int i = 0; i<6; i++){
			result[i] = tmp_res[i];
		}
	}
}
#endif


void pow_mod_p(const uint64_t x[6], uint64_t result[6]) {
	if (eq_one(x))
	{
		memcpy(result, x, sizeof(uint64_t)*6);
		return;
	}
#ifdef MONTG
  #ifdef MONTG_ASM
	mont_sqr_reduce_as(x, result);
  #else
	mont_mul_reduce(x, x, result);
  #endif
#else
	uint64_t res[12];
	raw_pow(x, res);
	barrett_384(res, result);
#endif
}

void mul_mod_p(const uint64_t x[6], const uint64_t y[6], uint64_t result[6]) {
	if (eq_one(x))
	{
		memcpy(result, y, sizeof(uint64_t)*6);
		return;
	}
	if (eq_one(y))
	{
		memcpy(result, x, sizeof(uint64_t)*6);
		return;
	}
#ifdef MONTG
  #ifdef MONTG_ASM
	mont_mul_reduce_as(x, y, result);
  #else
	mont_mul_reduce(x, y, result);
  #endif
#else
	uint64_t res[12];
	raw_mul(x, y, res);
	barrett_384(res, result);
#endif
}

void mul_mod_p_barrett(const uint64_t x[6], const uint64_t y[6], uint64_t result[6]) {

	if (eq_one(x))
	{
		memcpy(result, y, sizeof(uint64_t)*6);
		return;
	}
	if (eq_one(y))
	{
		memcpy(result, x, sizeof(uint64_t)*6);
		return;
	}
	uint64_t res[12];
	raw_mul(x, y, res);
	barrett_384(res, result);
}


static inline bool transform(u48* x, u48* y, bool carry)
{
	while (x->v[0] % 2 == 0)
	{
		shr(x);

		if (y->v[0] % 2 == 0)
		{
			shr(y);
		}
		else
		{
			carry = add(y->v, MODULUS, y->v);
			shr(y);
			if (carry)
			{
				y->v[5] |= 0x8000000000000000;
			}
		}
	}
	return carry;
}


void inv_for_mul(const u48* input, u48* result)
{
	if (eq_zero(input))
	{
		*result = *input;
		return;
	}

	u48 u = *input;
	u48 v;
	for (int i = 0; i < 6; i++)
	{
		v.v[i] = MODULUS[i];
	}
	
	u48 x1 = { 1, 0, 0, 0, 0, 0};
	u48 x2 = { 0, 0, 0, 0, 0, 0};


	bool overflowFlag = false;

	while ((!u48_eq_one(&u)) && (!u48_eq_one(&v)))
	{
		overflowFlag = transform(&u, &x1, overflowFlag);
		overflowFlag = transform(&v, &x2, overflowFlag);

		if (ge(&u, &v))
		{
			sub_mod_p(u.v, v.v, u.v);
			sub_mod_p(x1.v, x2.v, x1.v);
		}
		else
		{
			sub_mod_p(v.v, u.v, v.v);
			sub_mod_p(x2.v, x1.v, x2.v);
		}
	}

	if (u48_eq_one(&u))
	{
		if(u384_ge(x1.v, MODULUS))
			u384_sub(x1.v, MODULUS, x1.v);
		*result = x1;
	}
	else
	{
		if(u384_ge(x2.v, MODULUS))
			u384_sub(x2.v, MODULUS,x2.v);
		*result = x2;
	}

}