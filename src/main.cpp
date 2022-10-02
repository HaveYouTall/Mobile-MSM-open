#include "basicOp.h"
#include "fieldOp.h"
#include "barrett.h"
#include "const.h"
#include "ecc.h"
#include "utils.h"
#include "msm.h"
#include <assert.h>

#include <fstream>
#include <sys/time.h>
#include <omp.h>

// Calculate log2(data)
uint64_t log2(uint64_t data) {
    uint64_t count = 0; 
    while (data) { 
        data = data >> 1; 
        count++; 
    } 
    return count - 1;
}

void run_yyteam_msm_benchmark(int iterations){
    uint64_t a;
    uint64_t* len_vec;
    uint64_t* power_vec;
    AFPoint **baseGroup;
    u48 **scalarGroup;
    std::ifstream fin_p("points", std::ios::in | std::ios::binary);
    std::ifstream fin_s("scalars", std::ios::in | std::ios::binary);
    std::ofstream fout_res,fout_time;
    fout_res.open("result.txt", std::ios::out);
    fout_time.open("resulttime.txt", std::ios::out);

    baseGroup = (AFPoint**)malloc(sizeof(AFPoint*));
    scalarGroup = (u48**)malloc(sizeof(u48*));
    len_vec = (uint64_t*)malloc(sizeof(uint64_t));
    power_vec = (uint64_t*)malloc(sizeof(uint64_t));

    if(!fin_p.is_open()){
        return;
    }
    AFPoint tmp_p;
    uint64_t num_vec_count_p = 1;
    uint8_t flagValue = 0;
    while(fin_p.peek()!=EOF){
        baseGroup = (AFPoint**)realloc(baseGroup, sizeof(AFPoint*) * num_vec_count_p);
        len_vec = (uint64_t*)realloc(len_vec, sizeof(uint64_t)* num_vec_count_p);
        power_vec = (uint64_t*)realloc(power_vec, sizeof(uint64_t)* num_vec_count_p);
        fin_p.read((char *)&a, sizeof(a));
        len_vec[num_vec_count_p-1] = a;
        power_vec[num_vec_count_p-1] = log2(a);
        baseGroup[num_vec_count_p-1] = (AFPoint*)malloc(sizeof(AFPoint) * a);
        for(uint64_t i=0; i < a; i++){
            // fin_p.read((char *)&tmp_p, sizeof(tmp_p));
            fin_p.read((char *)&(tmp_p.x), sizeof(tmp_p.x));
            // Get the last byte as flag.
            flagValue = *(((char *)&(tmp_p.x)) + 47); 
            // If the infinity flag is set.
            if(checkInfinityFlag(flagValue)) {
                memcpy(baseGroup[num_vec_count_p-1] + i, &AFPoint_ZERO, sizeof(AFPoint));
            } else {
                fin_p.read((char *)&(tmp_p.y), sizeof(tmp_p.y));
                memcpy(baseGroup[num_vec_count_p-1] + i, &tmp_p, sizeof(AFPoint));
            }
        }
        num_vec_count_p++;
        
    }
    fin_p.close();

    if(!fin_s.is_open()){
        return;
    }
    u48 tmp_s;
    uint64_t num_vec_count_s = 1;
    while(fin_s.peek()!=EOF){
        fin_s.read((char *)&a, sizeof(a));
        scalarGroup = (u48**)realloc(scalarGroup, sizeof(u48*) *num_vec_count_s);
        scalarGroup[num_vec_count_s-1] = (u48*)malloc(sizeof(u48) * a);
        for(uint64_t i=0;i<a;i++){
            fin_s.read((char *)&tmp_s, sizeof(u32));
            memcpy(scalarGroup[num_vec_count_s-1] + i, &tmp_s, sizeof(u32));
        }
        num_vec_count_s++;
    }
    fin_s.close();
    
    assert(num_vec_count_p==num_vec_count_s);

    AFPoint **final_res;
    final_res = (AFPoint**)malloc(sizeof(AFPoint*) * (num_vec_count_p-1));
    unsigned int avai_cpu = omp_get_max_threads();

    struct timeval t_start, t_end;
    double diff = 0, total_time = 0, vec_ave_time = 0;

    for(uint64_t i=0;i<num_vec_count_p-1;i++){
        final_res[i] = (AFPoint*)malloc(sizeof(AFPoint) * (len_vec[i]));
        AFPoint *prepared_base = (AFPoint*)malloc(sizeof(AFPoint) * (len_vec[i]));
        JPoint bucket_res = {0};

        // First do the data preparation
        data_prepare_for_msm(baseGroup[i], prepared_base, len_vec[i], avai_cpu);

        total_time = 0;
        for(uint64_t j = 0; j < iterations; j++) {
            gettimeofday( &t_start, NULL);
            // Do the multi_scalar_mul.
            multi_scalar_mul(scalarGroup[i], prepared_base, &bucket_res, len_vec[i], power_vec[i], avai_cpu);
            gettimeofday( &t_end, NULL);
            diff = (t_end.tv_sec-t_start.tv_sec) + 
                    (t_end.tv_usec-t_start.tv_usec)/1000000.0;
            total_time += diff;
            // printf("Iterations: %d, MSM time: %f ms\n", j, (diff) * 1000);
            fout_time << "Iterations: " << j << ", MSM time: " << (diff)*1000 << " ms\n";
        }

        // printf("Vec: %d, MSM mean time: %f ms\n", i, (total_time/iterations) * 1000);

        fout_time << "Vec: " << i << ", MSM time: " << (total_time/iterations)*1000 << " ms\n\n\n";
        vec_ave_time += (total_time/iterations);

        fout_res << "Vec: " << i << "'s result: \n";
        // Transfer the msm result back to ShortW curves affine points in normal field.
        get_final_res(&bucket_res, final_res[i]);
        store_AFPoint(fout_res, final_res[i]);
        fout_res << "\n";
    }

    fout_time << "Total ave time: " << (vec_ave_time / (num_vec_count_p - 1))*1000 << " ms\n";

    fout_time.close();
    fout_res.close();
}

int main(int argc, char* argv[]){

    if(argc == 2) {
        int iterations = atoi(argv[1]);
        run_yyteam_msm_benchmark(iterations);
    } else {
        exit(-1);
    }

    return 0;
}

