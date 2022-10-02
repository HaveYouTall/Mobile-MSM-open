//
// Created by HaveYouTall
//
#include "ecc.h"

#define ASSIGN_AFFINE_PONIT(point,x,y) point={{(x),(y)}}
#define ASSIGN_JACOBIAN_POINT(point,x,y,z) point={{(x),(y),(z)}}

#ifdef Edwards
	const JPoint JPoint_ZERO = { { 0, 0, 0, 0,0,0 },{ 1, 0, 0, 0,0,0 },{ 1, 0, 0, 0,0,0 }, { 0, 0, 0, 0,0,0 } };
#else 
	const JPoint JPoint_ZERO = { { 1, 0, 0, 0,0,0 },{ 1, 0, 0, 0,0,0 },{ 0, 0, 0, 0,0,0 } };
#endif
const AFPoint AFPoint_ZERO = { {0,0,0,0,0,0}, {1,0,0,0,0,0} };


void CopyAFPoint(const AFPoint* src, AFPoint* dst)
{
	memcpy(dst, src, sizeof(AFPoint));
}
void CopyJPoint(const JPoint* src, JPoint* dst) // Will generate error: nvlink error   : Undefined reference to '_Z10CopyJPointP6JPointS0_' in '/tmp/tmpxft_001862b9_00000000-50_ecc.o'
{
	memcpy(dst, src, sizeof(JPoint));
}


// Need to distinguish the difference between montgomery version and normal version.
bool equ_to_AFPoint_one(const AFPoint* point)
{
#ifdef MONTG
	return eq_zero(&point->x) && eq(&point->y, &montg_one_u48);
#else
	return eq_zero(&point->x) && u48_eq_one(&point->y);
#endif
}

bool equ_to_AFPoint_one_no_mont(const AFPoint* point)
{
	return eq_zero(&point->x) && u48_eq_one(&point->y);
}

// Because there is no exact number compare, it do not need a montgomery version.
bool equ_to_JPoint_one(const JPoint* point)
{
#ifdef Edwards
	return eq_zero(&point->x) && !eq_zero(&point->y) && eq(&point->y, &point->z) && eq_zero(&point->t);
#else
	return u48_eq_one(&point->x) && u48_eq_one(&point->y) && eq_zero(&point->z);
#endif
}


void affine_to_projective(const AFPoint* point, JPoint* result)
{

	if (!equ_to_AFPoint_one(point))
	{
		result->x = point->x;
		result->y = point->y;
#ifdef MONTG
		memcpy(result->z.v, montg_one, sizeof(uint64_t)*6);
#else
		result->z.v[0] = 1;
		result->z.v[1] = 0;
		result->z.v[2] = 0;
		result->z.v[3] = 0;
		result->z.v[4] = 0;
		result->z.v[5] = 0;
#endif
#ifdef Edwards
		// T = x * y * Z, where Z is one here.
		// Note: In non-montg version, T = x * y * Z = x * y * 1 = x * y % modulus
		//       So in montg version, montg_T = to_mont(T) = to_mont(x * y % modulus) = montg_reduce(montg_x * montg_y)
		//       So we can totally use `mul_mod_p` to cover all version.
		mul_mod_p(result->x.v, result->y.v, result->t.v);
#endif
		return;
	}

	*result = JPoint_ZERO;
}

// This method should not be used for montg version.
void projective_to_affine(const JPoint* point, AFPoint* result)
{
	if(!equ_to_JPoint_one(point)){
		if(eq_one(point->z.v)) {
			memcpy(result->x.v, point->x.v, sizeof(uint64_t)*6);
			memcpy(result->y.v, point->y.v, sizeof(uint64_t)*6);
		} else {
#ifdef Edwards
			u48 inv_z;
			inv_for_mul(&(point->z), &inv_z);

			mul_mod_p(point->x.v, inv_z.v, result->x.v);
			mul_mod_p(point->y.v, inv_z.v, result->y.v);
#else // ShortW curve, using Jacobian 
			u48 inv_z, inv_z_square, inv_z_cube;
			inv_for_mul(&(point->z), &inv_z);
			mul_mod_p(inv_z.v, inv_z.v, inv_z_square.v);
			mul_mod_p(inv_z_square.v, inv_z.v, inv_z_cube.v);

			mul_mod_p(point->x.v, inv_z_square.v, result->x.v);
			mul_mod_p(point->y.v, inv_z_cube.v, result->y.v);
#endif
		}
	}
	else
	{
		// memset(result, 0, sizeof(AFPoint));
		memcpy(result, &AFPoint_ZERO, sizeof(AFPoint));
	}
}

void montg_projective_to_affine(const JPoint* montg_point, AFPoint* result)
{
	JPoint point = {0};
	mont_inv(montg_point->x.v, point.x.v);
	mont_inv(montg_point->y.v, point.y.v);
	mont_inv(montg_point->z.v, point.z.v);
	
	
	if (!eq_zero(&point.z))
	{
		if(eq_one(point.z.v)) {
			memcpy(result->x.v, point.x.v, sizeof(uint64_t)*6);
			memcpy(result->y.v, point.y.v, sizeof(uint64_t)*6);
		} else {
#ifdef Edwards
			u48 inv_z;
			inv_for_mul(&(point.z), &inv_z);

			mul_mod_p_barrett(point.x.v, inv_z.v, result->x.v);
			mul_mod_p_barrett(point.y.v, inv_z.v, result->y.v);
#else // ShortW curve, using Jacobian 
			u48 inv_z, inv_z_square, inv_z_cube;
			inv_for_mul(&(point.z), &inv_z);
			mul_mod_p_barrett(inv_z.v, inv_z.v, inv_z_square.v);
			mul_mod_p_barrett(inv_z_square.v, inv_z.v, inv_z_cube.v);

			mul_mod_p_barrett(point.x.v, inv_z_square.v, result->x.v);
			mul_mod_p_barrett(point.y.v, inv_z_cube.v, result->y.v);
#endif
		}
	}
	else if (equ_to_JPoint_one(&point))
	{
		// memset(result, 0, sizeof(AFPoint));
		memcpy(result, &AFPoint_ZERO, sizeof(AFPoint));
	}
}


// Note: this function should
// ALWAYS be called with different point
// Use Intel-IPP version
void add_JPoint_and_AFPoint(const JPoint* point1, const AFPoint* point2, JPoint* result)
{
	if (equ_to_JPoint_one(point1))
	{
		affine_to_projective(point2, result);
		return;
	}

	if (equ_to_AFPoint_one(point2))
	{
		*result = *point1;
		return;
	}

	u48 R, S2, U2, H, x3, y3, z3;


	pow_mod_p(point1->z.v, R.v);	 // R = Z1^2
	mul_mod_p(point2->y.v, point1->z.v, S2.v);        // S2 = Y2*Z1
   	mul_mod_p(point2->x.v, R.v, U2.v);         // U2 = X2*Z1^2
   	mul_mod_p(S2.v, R.v, S2.v);         // S2 = Y2*Z1^3

	if(eq(&U2, &(point1->x)) && eq(&S2, &(point1->y))) {
		double_JPoint(point1, result);
		return;
	} else {
		add_mod_p(point1->y.v, S2.v, R.v);
		if (eq(&(point1->x), &U2) && eq_zero(&R))
		{
			*result = JPoint_ZERO;
			return;
		}
		sub_mod_p(U2.v, point1->x.v, H.v);         // H = U2-X1
		sub_mod_p(S2.v, point1->y.v, R.v);         // R = S2-Y1
		mul_mod_p(H.v, point1->z.v, z3.v);        // Z3 = H*Z1

		pow_mod_p(H.v, U2.v);             // U2 = H^2
		pow_mod_p(R.v, S2.v);             // S2 = R^2
		mul_mod_p(H.v, U2.v, H.v);          // H = H^3
		mul_mod_p(U2.v, point1->x.v, U2.v);       // U2 = X1*H^2

		mul_mod_p(H.v, point1->y.v, y3.v);        // Y3 = Y1*H^3

		double_mod_p(U2.v, x3.v);          // X3 = 2*X1*H^2
		sub_mod_p(S2.v, x3.v, x3.v);      // X3 = R^2 - 2*X1*H^2
		sub_mod_p(x3.v, H.v, x3.v);       // X3 = R^2 - 2*X1*H^2 -H^3

		sub_mod_p(U2.v, x3.v, U2.v);       // U2 = X1*H^2 - X3
		mul_mod_p(U2.v, R.v, U2.v);         // U2 = R*(X1*H^2 - X3)
		sub_mod_p(U2.v, y3.v, y3.v);      // Y3 = -Y1*H^3 + R*(X1*H^2 - X3)

		result->x = x3;
		result->y = y3;
		result->z = z3;
	}

}



#include "utils.h"
// Use Intel-IPP version
// 3M + 5S + 2*2 + 1* /2 + 1*3 + 3add
void double_JPoint(const JPoint* pt, JPoint* result)
{
	u48 S, U, M;
	if (equ_to_JPoint_one(pt))
	{
		*result = *pt;
		return;
	}
	
	// A = 4*x*y^2
	// B = 3*x^2 + a*z^4, here a = 0.
	//
	// x3 = -2*A + B^2
	// y3 = -8y^4 +B*(A-x3)
	// z3 = 2*y*z
	double_mod_p(pt->y.v, S.v);  // S = 2Y
	pow_mod_p((pt->z.v), U.v);       // U = Z^2
	pow_mod_p(S.v, M.v);           // M = 4Y^2
	mul_mod_p(S.v, (pt->z.v), (result->z.v));  // rZ = 2YZ
	pow_mod_p(M.v, (result->y.v));          // rY = 16Y^4


	
	mul_mod_p(M.v, (pt->x.v), S.v);      // S = 4XY^2
	div_by_2_mod_p(result->y.v, result->y.v); // rY = 8Y^4


	pow_mod_p(pt->x.v, M.v);			   // M = X^2
	mul_by_3_mod_p(M.v, M.v);              // M = 3* X^2


	double_mod_p(S.v, U.v);                // U = 8XY^2
	pow_mod_p(M.v, (result->x.v));             // rX = M^2
	sub_mod_p(result->x.v, U.v, result->x.v);  // rX = M^2-U

	sub_mod_p(S.v, result->x.v, S.v);      // S = 4XY^2-rX
	mul_mod_p(S.v, M.v, S.v);                 // S = M*(4XY^2 - rX)
	sub_mod_p(S.v, result->y.v, result->y.v); // rY = M(4XY^2-rX) -8Y^4
}

// Use Intel-IPP version
// 12M + 4S + 7add + 1*2
void add_JPoint(const JPoint* point1, const JPoint* point2, JPoint* result)
{
	if (equ_to_JPoint_one(point1))
	{
		*result = *point2;
		return;
	}
	if (equ_to_JPoint_one(point2))
	{
		*result = *point1;
		return;
	}

	u48 U1 = {0}, U2 = {0}, S1 = {0}, S2 = {0};
	// ref intel-ipp
	mul_mod_p(point1->y.v, point2->z.v, S1.v); // S1 = Y1 * Z2
	pow_mod_p(point2->z.v, U1.v); 			   // U1 = Z2 ^ 2

	mul_mod_p(point2->y.v, point1->z.v, S2.v); // S2 = Y2 * Z1
	pow_mod_p(point1->z.v, U2.v); 			   // U2 = Z1 ^ 2

	mul_mod_p(S1.v, U1.v, S1.v); // S1 = Y1 * Z2 ^ 3
	mul_mod_p(S2.v, U2.v, S2.v); // S2 = Y2 * Z1 ^ 3

	mul_mod_p(point1->x.v, U1.v, U1.v); // U1 = X1 * Z2 ^ 2
	mul_mod_p(point2->x.v, U2.v, U2.v); // U2 = X2 * Z1 ^ 2

	//P1=P2
	if (eq(&U1, &U2) && eq(&S1, &S2))
	{
		double_JPoint(point1, result);
	}
	//P1!=P2
	else
	{
		u48 R = {0}, H = {0}, x3 = {0}, y3 = {0}, z3 = {0};
		add_mod_p(S1.v, S2.v, R.v);
		//P1=-P2
		if (eq(&U1, &U2) && eq_zero(&R))
		{
			*result = JPoint_ZERO;
			return;
		}
		
		sub_mod_p(S2.v, S1.v, R.v); // R = S2-S1
		sub_mod_p(U2.v, U1.v, H.v); // H = U2-U1

		mul_mod_p(point1->z.v, point2->z.v, z3.v); // Z3 = Z1*Z2
		pow_mod_p(H.v, U2.v);              // U2 = H^2
		mul_mod_p(z3.v, H.v, z3.v);        // Z3 = (Z1*Z2)*H
		pow_mod_p(R.v, S2.v);              // S2 = R^2
		mul_mod_p(H.v, U2.v, H.v);          // H = H^3

		mul_mod_p(U1.v, U2.v, U1.v);         // U1 = U1*H^2
   		sub_mod_p(S2.v, H.v, x3.v);         // X3 = R^2 - H^3
		double_mod_p(U1.v, U2.v);  			// U2 = 2*U1*H^2
   		mul_mod_p(S1.v, H.v, S1.v);          // S1 = S1*H^3
   		sub_mod_p(x3.v, U2.v, x3.v);       // X3 = (R^2 - H^3) -2*U1*H^2

   		sub_mod_p(U1.v, x3.v, y3.v);       // Y3 = R*(U1*H^2 - X3) -S1*H^3
		mul_mod_p(y3.v, R.v, y3.v);
		sub_mod_p(y3.v, S1.v, y3.v);

		result->x = x3;
		result->y = y3;
		result->z = z3;
	}
}



void AFPoint_neg(const AFPoint* point, AFPoint* result)
{
	result->x = point->x;
	sub(&_MODULUS_, &point->y, &result->y);
}

void JPoint_neg(const JPoint* point, JPoint* result)
{
	result->x = point->x;
	sub(&_MODULUS_, &point->y, &result->y);
	result->z = point->z;
}