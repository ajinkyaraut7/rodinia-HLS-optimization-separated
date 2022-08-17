#include "knn.h"
extern "C"{
void load (int load_idx, float* searchSpace, float* local_searchSpace)
{
    int start_idx = load_idx * NUM_PT_IN_BUFFER * NUM_FEATURE;
    LOAD_TILE: for (int i(0); i < NUM_PT_IN_BUFFER*NUM_FEATURE; ++i){
    #pragma HLS PIPELINE II=1 off
        local_searchSpace[i] = searchSpace[start_idx+i];
    }
}

void compute_dist (float* local_inputQuery, float* local_searchSpace, float* local_distance)
{
    float sum;
    float feature_delta;
    #pragma HLS PIPELINE II=1 off
    COMPUTE_TILE: for (int i = 0; i < NUM_PT_IN_BUFFER; ++i) {
        sum = 0.0;
        #pragma HLS PIPELINE II=1 off
        for (int j = 0; j < NUM_FEATURE; ++j){
            feature_delta = local_searchSpace[i*NUM_FEATURE+j] - local_inputQuery[j];
            sum += feature_delta*feature_delta;
        }
        local_distance[i] = sum;
    }
}

void store (int store_idx, float* local_distance, float* distance)
{
    int start_idx = store_idx * NUM_PT_IN_BUFFER;
#pragma HLS PIPELINE II=1 off
    STORE_TILE: for (int i(0); i < NUM_PT_IN_BUFFER; ++i){
    #pragma HLS PIPELINE II=1 off
        distance[start_idx+i] = local_distance[i];
    }
}

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

    float local_inputQuery[NUM_FEATURE];
    float local_searchSpace[NUM_PT_IN_BUFFER*NUM_FEATURE];
    float local_distance[NUM_PT_IN_BUFFER];
#pragma HLS PIPELINE II=1 off

    LOAD_INPUTQUERY: for (int i(0); i<NUM_FEATURE; ++i){
    #pragma HLS PIPELINE II=1 off
        local_inputQuery[i] = inputQuery[i];
    }
#pragma HLS PIPELINE II=1 off
    TILED_PE: for (int tile_idx(0); tile_idx<NUM_TILES; ++tile_idx){
        load(tile_idx, searchSpace, local_searchSpace);
        compute_dist(local_inputQuery, local_searchSpace, local_distance);
        store(tile_idx, local_distance, distance);
    }

    return;
}
}






