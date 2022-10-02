#ifndef _MSM_H_
#define _MSM_H_

#include "dataType.h"
#include "barrett.h"
#include "basicOp.h"
#include "montgomery.h"
#include "ecc.h"
#include "utils.h"

#define SCALAR_BIT 253

// void msm(u48 *scalar, AFPoint *base, AFPoint *result, size_t size, size_t power, unsigned int avai_cpu);
void data_prepare_for_msm(AFPoint *base, AFPoint *prepared_base, size_t size, unsigned int avai_cpu);
void get_final_res(JPoint *bucket_res, AFPoint *result);
void msm_scalar_mul_parallel_omp(u48 *scalar, AFPoint *base, JPoint *result, size_t size, size_t power, unsigned int avai_cpu);

#define multi_scalar_mul msm_scalar_mul_parallel_omp

#endif