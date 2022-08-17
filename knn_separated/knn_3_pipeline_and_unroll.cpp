#include "knn.h"

extern "C"{
void workload(
    float inputQuery[NUM_FEATURE],
    float searchSpace[NUM_PT_IN_SEARCHSPACE*NUM_FEATURE],
    float distance[NUM_PT_IN_SEARCHSPACE]
){
    #pragma HLS INTERFACE m_axi port=inputQuery offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=inputQuery bundle=control
    #pragma HLS INTERFACE m_axi port=searchSpace offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=searchSpace bundle=control
    #pragma HLS INTERFACE m_axi port=distance offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=distance bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    
    float sum;
    float feature_delta;


    for (int i = 0; i < NUM_PT_IN_BUFFER; i+=UNROLL_FACTOR) {
        #pragma HLS PIPELINE II=1
            for (int j = 0; j < UNROLL_FACTOR; ++j){
            #pragma HLS UNROLL
                sum = 0.0;
                for (int k = 0; k < NUM_FEATURE; ++k){
                    feature_delta = searchSpace[(i+j)*NUM_FEATURE+k] - inputQuery[k];
                    sum += feature_delta*feature_delta;
                }
                distance[i+j] = sum;
            }
        }


//    for(int i = 0; i < NUM_PT_IN_SEARCHSPACE; ++i){
//        #pragma HLS PIPELINE II= 1
//        sum = 0.0;
//        for(int j = 0; j < NUM_FEATURE; ++j){
//            feature_delta = searchSpace[i*NUM_FEATURE+j] - inputQuery[j];
//            sum += feature_delta*feature_delta;
//        }
//        distance[i] = sum;
//    }

    return;
}
}
