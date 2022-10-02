//
// Created by HaveYouTall
//

#include "basicOp.h"

// add for u48, i.e., uint64_t [6]
// result = a + b
/// @return carry
uint8_t add(const uint64_t a[6], const uint64_t b[6], uint64_t result[6])
{
	u1 carry = 0;
	carry = _addcarryx_u64(carry, a[0], b[0], result + 0);
	carry = _addcarryx_u64(carry, a[1], b[1], result + 1);
	carry = _addcarryx_u64(carry, a[2], b[2], result + 2);
	carry = _addcarryx_u64(carry, a[3], b[3], result + 3);
	carry = _addcarryx_u64(carry, a[4], b[4], result + 4);
	carry = _addcarryx_u64(carry, a[5], b[5], result + 5);
	return carry;
}

// inc for u48, i.e., uint64_t [6]
// result = a + b, where b is at most 64-bit data. 
/// @return carry
uint8_t inc(const uint64_t a[6], const uint64_t b, uint64_t result[6])
{
	u1 carry = 0;
	carry = _addcarryx_u64(carry, a[0], b, result + 0);
	carry = _addcarryx_u64(carry, a[1], 0, result + 1);
	carry = _addcarryx_u64(carry, a[2], 0, result + 2);
	carry = _addcarryx_u64(carry, a[3], 0, result + 3);
	carry = _addcarryx_u64(carry, a[4], 0, result + 4);
	carry = _addcarryx_u64(carry, a[5], 0, result + 5);
	return carry;
}

// sub for u48, i.e., uint64_t [6]
// result = a - b
void sub(const u48* a, const u48* b, u48* result)
{
	u1 carry = 1;
	carry = _addcarryx_u64(carry, a->v[0], ~b->v[0], result->v);
	carry = _addcarryx_u64(carry, a->v[1], ~b->v[1], result->v + 1);
	carry = _addcarryx_u64(carry, a->v[2], ~b->v[2], result->v + 2);
	carry = _addcarryx_u64(carry, a->v[3], ~b->v[3], result->v + 3);
	carry = _addcarryx_u64(carry, a->v[4], ~b->v[4], result->v + 4);
	carry = _addcarryx_u64(carry, a->v[5], ~b->v[5], result->v + 5);
}

// res = a - b
// return carry, if carry == 1, it means a >= b, otherwise b > a.
uint8_t u384_sub(const uint64_t a[6], const uint64_t b[6], uint64_t res[6]){
    uint8_t carry = 1;
	carry = _addcarryx_u64(carry, a[0], ~b[0], res);
	carry = _addcarryx_u64(carry, a[1], ~b[1], (res + 1));
	carry = _addcarryx_u64(carry, a[2], ~b[2], (res + 2));
	carry = _addcarryx_u64(carry, a[3], ~b[3], (res + 3));
	carry = _addcarryx_u64(carry, a[4], ~b[4], (res + 4));
	carry = _addcarryx_u64(carry, a[5], ~b[5], (res + 5));
    return carry;
}


// neg for u48, i.e., uint64_t [6]
// input = -input
void neg(u48* input)
{
	u1 carry = 1;
	carry = _addcarryx_u64(carry, ~input->v[0], 0, input->v);
	carry = _addcarryx_u64(carry, ~input->v[1], 0, input->v + 1);
	carry = _addcarryx_u64(carry, ~input->v[2], 0, input->v + 2);
	carry = _addcarryx_u64(carry, ~input->v[3], 0, input->v + 3);
	carry = _addcarryx_u64(carry, ~input->v[4], 0, input->v + 4);
	carry = _addcarryx_u64(carry, ~input->v[5], 0, input->v + 5);
}

// left shift for u48, i.e., uint64_t [6]
// input = input << 1
/// @return carry
u1 shl(u48* input)
{
	u1 carry = (input->v[5] >> 63);
	input->v[5] = (input->v[5] << 1) | (input->v[4] >> 63);
	input->v[4] = (input->v[4] << 1) | (input->v[3] >> 63);
	input->v[3] = (input->v[3] << 1) | (input->v[2] >> 63);
	input->v[2] = (input->v[2] << 1) | (input->v[1] >> 63);
	input->v[1] = (input->v[1] << 1) | (input->v[0] >> 63);
	input->v[0] <<= 1;
	return carry;
}

// right shift for u48, i.e., uint64_t [6]
// input = input >> 1
/// @return carry
void shr(u48* input)
{
	input->v[0] = (input->v[0] >> 1) | (input->v[1] << 63);
	input->v[1] = (input->v[1] >> 1) | (input->v[2] << 63);
	input->v[2] = (input->v[2] >> 1) | (input->v[3] << 63);
	input->v[3] = (input->v[3] >> 1) | (input->v[4] << 63);
	input->v[4] = (input->v[4] >> 1) | (input->v[5] << 63);
	input->v[5] >>= 1;
}

// judge for u48, i.e., uint64_t [6]
/// @return true if a is greater than b or equal to b.
bool ge(const u48* a, const u48* b)
{
	if (a->v[5] != b->v[5])
		return (a->v[5] > b->v[5]);
	if (a->v[4] != b->v[4])
		return (a->v[4] > b->v[4]);
	if (a->v[3] != b->v[3])
		return (a->v[3] > b->v[3]);
	if (a->v[2] != b->v[2])
		return (a->v[2] > b->v[2]);
	if (a->v[1] != b->v[1])
		return (a->v[1] > b->v[1]);
	return (a->v[0] >= b->v[0]);
}

bool u384_ge(const uint64_t a[6], const uint64_t b[6]){
    if (a[5] != b[5])
		return (a[5] > b[5]);
    if (a[4] != b[4])
		return (a[4] > b[4]);
    if (a[3] != b[3])
		return (a[3] > b[3]);
	if (a[2] != b[2])
		return (a[2] > b[2]);
	if (a[1] != b[1])
		return (a[1] > b[1]);
	return (a[0] >= b[0]);
}



// judge for u48, i.e., uint64_t [6]
/// @return true if a is greater than b. 
bool gr(const u48* a, const u48* b)
{
	if (a->v[5] != b->v[5])
		return (a->v[5] > b->v[5]);
	if (a->v[4] != b->v[4])
		return (a->v[4] > b->v[4]);
	if (a->v[3] != b->v[3])
		return (a->v[3] > b->v[3]);
	if (a->v[2] != b->v[2])
		return (a->v[2] > b->v[2]);
	if (a->v[1] != b->v[1])
		return (a->v[1] > b->v[1]);
	return (a->v[0] > b->v[0]);
}

// judge for u48, i.e., uint64_t [6]
/// @return true if a is equal to b.
bool eq(const u48* a, const u48* b)
{
	return !( (a->v[0] ^ b->v[0]) | (a->v[1] ^ b->v[1]) | (a->v[2] ^ b->v[2]) |
	          (a->v[3] ^ b->v[3]) | (a->v[4] ^ b->v[4]) | (a->v[5] ^ b->v[5]) );
}

// judge for u48, i.e., uint64_t [6]
/// @return true if a is equal to zero. 
bool eq_zero(const u48* a)
{
	return !(a->v[0] | a->v[1] | a->v[2] | a->v[3] | a->v[4] | a->v[5]);
}

// judge for u48, i.e., uint64_t [6]
/// @return true if a is equal to 1. 
bool u48_eq_one(const u48* a)
{
	return !((a->v[0] ^ 1) | a->v[1] | a->v[2] | a->v[3] | a->v[4] | a->v[5]);
}

// judge for u48, i.e., uint64_t [6]
/// @return true if a is equal to 1. 
bool eq_one(const uint64_t a[6])
{
	return !((a[0] ^ 1) | a[1] | a[2] | a[3] | a[4] | a[5]);
}
