//
// Created by HaveYouTall
// Twisted Edwards: https://www.hyperelliptic.org/EFD/g1p/auto-twisted.html
// 

// Note 1: We are now using this file.
// Note 2: When using Edwards version, one must use montgomery field operation.
// Note 3: When transform the points to Edwards curve, one must use normal affine point as input.

#include "ecc.h"


#include "ecc.h"


#ifdef Edwards
uint64_t beta[6] = {
    0x450ae9206343e6e4,
    0x7af39509df5027b6,
    0xab82b31405cf8a30,
    0x80d743e1f6c15c7c,
    0xcec22e650360183,
    0x272fd56ac5c669
};

uint64_t inv_beta[6] = {
    0x356175917ba313cb, 
    0x1c87ccf678e91773,
    0x183a0e17d880d6eb,
    0x23a5a4061efe45e,
    0x9292befe4d8fc837,
    0x148a180a7948f23
};

uint64_t s[6] = {
    0x3401d618f0339eab,
    0xf793b8504b428d4,
    0xff643cca95ccc0d,
    0xd7a504665d66cc8c,
    0x1dc07a44b1eeea84,
    0x10f272020f118a
};

uint64_t inv_s[6] = {
    0x9c05824ad09adc01,
    0x2e6bb28f0e1c7a7c,
    0x2fe2cb65fc166427,
    0x86ef0d33183465a4,
    0x59416ece15ccbf8e,
    0x32d756062d349e
};

uint64_t one[6] = {1,0,0,0,0,0};

// tranform non-montgomery sw affine point to edwards affine points.
void sw_to_ed(AFPoint *sw_point, AFPoint *ed_point) {
    u48 Mx, My;
    u48 TE1x, TE1y;
    u48 tmp;

    if(equ_to_AFPoint_one_no_mont(sw_point)) {
        memcpy(ed_point, &AFPoint_ZERO, sizeof(AFPoint));
        return;
    }

    // Mx = s * (Wx - alpha)
    add_mod_p(sw_point->x.v, one, Mx.v);
    mul_mod_p_barrett(Mx.v, s, Mx.v);

    // My = s * Wy
    mul_mod_p_barrett(sw_point->y.v, s, My.v);

    // TE1x = Mx/My
    inv_for_mul(&My, &TE1x);
    mul_mod_p_barrett(Mx.v, TE1x.v, TE1x.v);

    // TE1y = (Mx - 1)/(Mx+1)
    add_mod_p(Mx.v, one, tmp.v);
    inv_for_mul(&tmp, &tmp);
    sub_mod_p(Mx.v, one, TE1y.v);
    mul_mod_p_barrett(TE1y.v, tmp.v, TE1y.v);


    //x = TE1x * beta
    mul_mod_p_barrett(TE1x.v, beta, ed_point->x.v);
    //y = TE1y
    ed_point->y = TE1y;

    return;
}

// tranform non-montgomery edwards affine point back to sw affine points.
void ed_to_sw(AFPoint *ed_point, AFPoint *sw_point) {
    u48 Mx, My;
    u48 TE1x, TE1y;
    u48 tmp;

    if(equ_to_AFPoint_one_no_mont(ed_point)) {
        memcpy(sw_point, &AFPoint_ZERO, sizeof(AFPoint));
        return;
    }

    // TE1y = TE2y
    TE1y = ed_point->y;
    // TE1x = TEx2 / beta
    mul_mod_p_barrett(ed_point->x.v, inv_beta, TE1x.v);

    // Mx = (-(TE1y + 1)) / (TE1y - 1)
    add_mod_p(TE1y.v, one, Mx.v);
    neg_mod_p(Mx.v, Mx.v);
    sub_mod_p(TE1y.v, one, tmp.v);
    inv_for_mul(&tmp, &tmp);
    mul_mod_p_barrett(Mx.v, tmp.v, Mx.v);

    // My = Mx / TE1x
    inv_for_mul(&TE1x, &My);
    mul_mod_p_barrett(My.v, Mx.v, My.v);

    // y = My / s
    mul_mod_p_barrett(My.v, inv_s, sw_point->y.v);
    // x = (Mx / s) + alpha
    mul_mod_p_barrett(Mx.v, inv_s, sw_point->x.v);
    sub_mod_p(sw_point->x.v, one, sw_point->x.v);

    return;
}



// ref: https://github.com/arkworks-rs/algebra/blob/v0.3.0/ec/src/models/twisted_edwards_extended.rs
// See "Twisted Edwards Curves Revisited" (https://eprint.iacr.org/2008/522.pdf)
// by Huseyin Hisil, Kenneth Koon-Ho Wong, Gary Carter, and Ed Dawson
// 3.1 Unified Addition in E^e
// Only support montg version.
void add_EDPoint(const JPoint* point1, const JPoint* point2, JPoint* result) {
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
    
    u48 x3, y3, z3, t3;
    u48 A, B, C, D, E, F, G, H;

    // A = x1 * x2
    mul_mod_p(point1->x.v, point2->x.v, A.v); 

    // B = y1 * y2
    mul_mod_p(point1->y.v, point2->y.v, B.v);

    // C = d * t1 * t2
    mul_mod_p(point1->t.v, point2->t.v, C.v);
    mul_mod_p(montg_COEFF_D, C.v, C.v);

    // D = z1 * z2
    mul_mod_p(point1->z.v, point2->z.v, D.v);

    // E = (x1 + y1) * (x2 + y2) - A - B
    add_mod_p(point1->x.v, point1->y.v, F.v); // F = x1 + y1
    add_mod_p(point2->x.v, point2->y.v, H.v); // H = x2 + y2
    mul_mod_p(F.v, H.v, E.v); // E = (x1 + y1) * (x2 + y2)
    sub_mod_p(E.v, A.v, E.v); // E = E - A
    sub_mod_p(E.v, B.v, E.v); // E = E - B
    // puts("E: ");
    // print_u48(&E);

    // H = B - aA
    // mul_mod_p(A.v, COEFF_A, H.v); // H = a*A
    // mul_mod_p(A.v, montg_COEFF_A, H.v); // H = a*A
    // sub_mod_p(B.v, H.v, H.v); // H = B - a*A
    add_mod_p(B.v, A.v, H.v); // H = B - a*A = B + A
    // puts("H: ");
    // print_u48(&H);

    // F = D - C
    sub_mod_p(D.v, C.v, F.v);
    // puts("F: ");
    // print_u48(&F);

    // G = D + C
    add_mod_p(D.v, C.v, G.v);
    // puts("G: ");
    // print_u48(&G);

    // x3 = E * F
    mul_mod_p(E.v, F.v, x3.v);

    // y3 = G * H
    mul_mod_p(G.v, H.v, y3.v);

    // t3 = E * H
    mul_mod_p(E.v, H.v, t3.v);

    // z3 = F * G
    mul_mod_p(G.v, F.v, z3.v);

    result->x = x3;
    result->y = y3;
    result->z = z3;
    result->t = t3;
}


void add_EDPoint_and_AFPoint(const JPoint* point1, const AFPoint* point2, JPoint* result) {
    // puts("add_EDPoint_and_AFPoint");
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

    u48 x3, y3, z3, t3;
    u48 A, B, C, E, F, G, H;
    
    // A = x1 * x2
    mul_mod_p(point1->x.v, point2->x.v, A.v); 

    // B = y1 * y2
    mul_mod_p(point1->y.v, point2->y.v, B.v);

    // C = d * t1 * t2
    mul_mod_p(point2->x.v, point2->y.v, C.v); // C = x2 * y2 = t2
    mul_mod_p(point1->t.v, C.v, C.v); // C = t1 * t2
    /// TODO: implement mul_coeff_mod_p. Which only need 6 x 64-bit mul.
    mul_mod_p(montg_COEFF_D, C.v, C.v); // C = d * t1 * t2


    // D = Z1

    // E = (x1 + y1) * (x2 + y2) - A - B
    add_mod_p(point1->x.v, point1->y.v, F.v); // F = x1 + y1
    add_mod_p(point2->x.v, point2->y.v, H.v); // H = x2 + y2
    mul_mod_p(F.v, H.v, E.v); // E = (x1 + y1) * (x2 + y2)
    sub_mod_p(E.v, A.v, E.v); // E = E - A
    sub_mod_p(E.v, B.v, E.v); // E = E - B

    // H = B - aA
    add_mod_p(B.v, A.v, H.v); // H = B - a*A = B + A

    // F = D - C
    sub_mod_p(point1->z.v, C.v, F.v);

    // G = D + C
    add_mod_p(point1->z.v, C.v, G.v);

    // x3 = E * F
    mul_mod_p(E.v, F.v, x3.v);

    // y3 = G * H
    mul_mod_p(G.v, H.v, y3.v);

    // t3 = E * H
    mul_mod_p(E.v, H.v, t3.v);

    // z3 = F * G
    mul_mod_p(G.v, F.v, z3.v);

    result->x = x3;
    result->y = y3;
    result->z = z3;
    result->t = t3;

}

// See "Twisted Edwards Curves Revisited"
// Huseyin Hisil, Kenneth Koon-Ho Wong, Gary Carter, and Ed Dawson
// 3.3 Doubling in E^e
// Source: https://www.hyperelliptic.org/EFD/g1p/data/twisted/extended/doubling/dbl-2008-hwcd
void double_EDPoint(const JPoint* pt, JPoint* result) {
    if (equ_to_JPoint_one(pt))
	{
		*result = *pt;
		return;
	}
	
    u48 x3, y3, z3, t3;
    u48 A, B, C, D, E, F, G, H;

    // A = x1^2
    pow_mod_p(pt->x.v, A.v);

    // B = y1^2
    pow_mod_p(pt->y.v, B.v);

    // C = 2* Z1^2
    pow_mod_p(pt->z.v, C.v);
    double_mod_p(C.v, C.v);

    // D = a * A = -A
    neg_mod_p(A.v, D.v);

    // E = (x1 + y1) ^2 - A - B
    add_mod_p(pt->x.v, pt->y.v, E.v); // E = x1 + y1
    pow_mod_p(E.v, E.v); // E = (x1 + y1) ^2
    sub_mod_p(E.v, A.v, E.v); // E = E - A
    sub_mod_p(E.v, B.v, E.v); // E = E - B

    // G = D + B
    add_mod_p(D.v, B.v, G.v);

    // F = G - C
    sub_mod_p(G.v, C.v, F.v);

    // H = D - B
    sub_mod_p(D.v, B.v, H.v);

    // x3 = E * F
    mul_mod_p(E.v, F.v, x3.v);

    // y3 = G * H
    mul_mod_p(G.v, H.v, y3.v);

    // t3 = E * H
    mul_mod_p(E.v, H.v, t3.v);

    // z3 = F * G
    mul_mod_p(G.v, F.v, z3.v);

    result->x = x3;
    result->y = y3;
    result->z = z3;
    result->t = t3;
}
#endif