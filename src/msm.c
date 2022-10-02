//
// Created by HaveYouTall
// 

#include "msm.h"
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <errno.h>
// ref: https://stackoverflow.com/questions/16319725/android-set-thread-affinity
#include <sched.h> // Only for cpp

typedef struct {
    size_t c;
    size_t size;
    u48* scalar;
    AFPoint *base;
    JPoint *window_sum;
    int cpu_id;
    int avai_cpu;
}window_ctxt;

#include <omp.h>
void  window_sum_per_window_omp(window_ctxt* ctx) {
    JPoint *buckets = (JPoint*) malloc(sizeof(JPoint)*((1<<(ctx->c)) - 1));
    for(size_t w_start = 0, idx = 0; w_start < SCALAR_BIT; w_start+=(ctx->c), idx++) {
        if(idx % ctx->avai_cpu == ctx->cpu_id) {
            JPoint res = JPoint_ZERO;
            for(size_t i = 0; i < (1<<(ctx->c)) - 1; i++) {
                memcpy(buckets + i, &res, sizeof(JPoint));
            }

            // for_each pair of scalar and base
            for(size_t i = 0; i < ctx->size; i++) {
                if(u48_eq_one(ctx->scalar + i)) {
                    if(w_start == 0) {
                        Mixed_Point_Add(&res, ctx->base + i, &res);
                    }
                } else {

                    // ref: https://github.com/arkworks-rs/algebra/blob/80857c9714c5a59068f8c20f1298e2138440a1d0/ff/src/biginteger/mod.rs#L833
                    u48 _scalar;
                    memcpy(&_scalar, ctx->scalar + i, sizeof(u48));
                    rshift(_scalar.v, 6, w_start);

                    // Normally, c is less than 64 (Not sure). At least in MSM case ,
                    // because the arkworks is like let scalar = scalar.as_ref()[0] % (1 << c); which uses the last limbs of BIGINT.
                    // So after mod 2^c, the _scalar has less than c bit digit, which means only _scalar.v[0] carrys the digits.
                    mod_2n(_scalar.v, 6, ctx->c); 
                    
                    if(!eq_zero(&_scalar)) {
                        Mixed_Point_Add(buckets + _scalar.v[0] - 1, ctx->base + i, buckets + _scalar.v[0] - 1);
                    }
                }
            }

            JPoint running_sum = JPoint_ZERO;
            for(size_t i = (1<<(ctx->c)) - 2; i >= 0; i--) {
                
                Projective_Point_Add(&running_sum, buckets + i, &running_sum);
                Projective_Point_Add(&res, &running_sum, &res);

                if(i==0) {
                    break;
                }
            }

            memcpy(ctx->window_sum + idx, &res, sizeof(JPoint));
        }
        
    }
}

void msm_scalar_mul_parallel_omp(u48 *scalar, AFPoint *base, JPoint *result, size_t size, size_t power, unsigned int avai_cpu) {
    size_t c = 0; 
    if(size < 32) {
        c = 3;
    } else { 
        // ref: https://github.com/arkworks-rs/algebra/blob/e7eeea3f2ac7d621f68089b978097403dd02e91b/ec/src/msm/mod.rs
        c = power * 69 / 100 + 2; // c = ln_without_floats(size) + 2; Where ln_without_floats(size) calculates ln(size) = log2(size) * ln(2) 
    }

    JPoint *window_sum = (JPoint*) malloc(((SCALAR_BIT / c)+1)*sizeof(JPoint));
    window_ctxt *ctx = (window_ctxt*)malloc(avai_cpu * sizeof(window_ctxt));

    for(size_t idx = 0; idx < avai_cpu; idx++) {
        (ctx + idx)->c = c; 
        (ctx + idx)->size = size;
        (ctx + idx)->scalar = scalar;
        (ctx + idx)->base = base;
        (ctx + idx)->window_sum = window_sum;
        (ctx + idx)->avai_cpu = avai_cpu;
        (ctx + idx)->cpu_id = idx;
    }

    #pragma omp parallel num_threads(avai_cpu)
    {
        window_sum_per_window_omp(ctx + omp_get_thread_num());
    }
    
    JPoint lowest;
    memcpy(&lowest, window_sum, sizeof(JPoint));
    JPoint total = JPoint_ZERO;
    for(size_t i = (SCALAR_BIT / c); i>=1 ;i--) {
        Projective_Point_Add(&total, window_sum + i, &total);
        for(size_t j = 0; j<c;j++) {
            Projective_Point_Double(&total, &total);
        }
    }

    Projective_Point_Add(&lowest, &total, result);
}

void data_to_montg(AFPoint *base, AFPoint *montg_base, size_t size, unsigned int avai_cpu) {
    
    #pragma omp parallel for num_threads(avai_cpu)
    for(size_t i = 0; i < size; i++) {
        to_mont((base + i)->x.v, (montg_base + i)->x.v);
        to_mont((base + i)->y.v, (montg_base + i)->y.v);
    }
}

#ifdef Edwards
void data_sw_point_to_ed_point(AFPoint *swpoint, AFPoint *edpoint, size_t size, unsigned int avai_cpu) {
    #pragma omp parallel for num_threads(avai_cpu)
    for(size_t i = 0; i < size; i++) {
        sw_to_ed((swpoint + i), (edpoint + i));
    }
}
#endif

void get_final_res(JPoint *bucket_res, AFPoint *result) {
#ifdef MONTG
    montg_projective_to_affine(bucket_res, result);
#else
    projective_to_affine(bucket_res, result);
#endif
#ifdef Edwards
    ed_to_sw(result, result);
#endif
}

void data_prepare_for_msm(AFPoint *base, AFPoint *prepared_base, size_t size, unsigned int avai_cpu) {
// Prepare date.
#ifdef MONTG
  #ifdef Edwards
    AFPoint *ed_base = (AFPoint*)malloc(sizeof(AFPoint)*size);
    data_sw_point_to_ed_point(base, ed_base, size, avai_cpu);
    data_to_montg(ed_base, prepared_base, size, avai_cpu);
  #else
    data_to_montg(base, prepared_base, size, avai_cpu);
  #endif
#endif
    return;
}

/*
void msm(u48 *scalar, AFPoint *base, AFPoint *result, size_t size, size_t power, unsigned int avai_cpu) {
    JPoint bucket_res = {0};
#ifdef MONTG
    // Prepare date.
  #ifdef Edwards
    puts("Edwards version");
    AFPoint *ed_base = (AFPoint*)malloc(sizeof(AFPoint)*size);
    AFPoint *montg_base = (AFPoint*)malloc(sizeof(AFPoint)*size);
    data_sw_point_to_ed_point(base, ed_base, size, avai_cpu);
    data_to_montg(ed_base, montg_base, size, avai_cpu);
  #else
    AFPoint *montg_base = (AFPoint*)malloc(sizeof(AFPoint)*size);
    data_to_montg(base, montg_base, size, avai_cpu);
  #endif
    msm_scalar_mul_parallel_omp(scalar, montg_base, &bucket_res, size, power, avai_cpu);
#else
    msm_scalar_mul_parallel_omp(scalar, base, &bucket_res, size, power, avai_cpu);
#endif
    // Translate the bucker_res to final AFPoint result.
    get_final_res(&bucket_res, result);
}
*/

