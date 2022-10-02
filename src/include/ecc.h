//
// Created by HaveYouTall
//

// NOTE:
// We define Affine Point one as (x,y) = (0,1). 
// And we define Projective Point one as (x,y,z) = (1,1,0) for Jacobian and as (x,y,z,t) = (0,1,1,0) for Edwards.
// Besides,
// Affine Point (x,y) = (X/Z^2, Y/Z^3), where X, Y, Z is from Jacobian Projective Point (X, Y, Z).
// When it comes to Twisted Edwards Curve, (x, y) = (X/Z, Y/Z), and T = (x * y * Z)
#ifndef HEADERS_ECCGROUP_H
#define HEADERS_ECCGROUP_H

#include "fieldOp.h"
#include "const.h"
#include "dataType.h"
#include "montgomery.h"


extern const JPoint JPoint_ZERO;
extern const AFPoint AFPoint_ZERO;

bool equ_to_AFPoint_one(const AFPoint* point);
bool equ_to_JPoint_one(const JPoint* point);
bool equ_to_AFPoint_one_no_mont(const AFPoint* point);

void affine_to_projective(const AFPoint* point, JPoint* result);
// ================================ //
void projective_to_affine(const JPoint* point, AFPoint* result);
void montg_projective_to_affine(const JPoint* montg_point, AFPoint* result);
// ================================ //
void CopyAFPoint(const AFPoint* src, AFPoint* dst);
void CopyJPoint(const JPoint* src, JPoint* dst);


// ShortW Jacobian Projective Point Operation.
void add_JPoint_and_AFPoint(const JPoint* point1, const AFPoint* point2, JPoint* result);;
void add_JPoint(const JPoint* point1, const JPoint* point2, JPoint* result);
void double_JPoint(const JPoint* pt, JPoint* result);

// Twisted Edwards Point Operation.
#ifdef Edwards
void add_EDPoint_and_AFPoint(const JPoint* point1, const AFPoint* point2, JPoint* result);
void add_EDPoint(const JPoint* point1, const JPoint* point2, JPoint* result);
void double_EDPoint(const JPoint* pt, JPoint* result);
void sw_to_ed(AFPoint *sw_point, AFPoint *ec_point);
void ed_to_sw(AFPoint *ed_point, AFPoint *sw_point);
#endif

// Define BLS12-377 G1 Point Operation Interface.
#ifdef Edwards
  #define Mixed_Point_Add add_EDPoint_and_AFPoint
  #define Projective_Point_Double double_EDPoint
  #define Projective_Point_Add add_EDPoint
#else
  #define Mixed_Point_Add add_JPoint_and_AFPoint
  #define Projective_Point_Double double_JPoint
  #define Projective_Point_Add add_JPoint
#endif


#endif