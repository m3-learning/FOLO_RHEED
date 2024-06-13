#ifndef NNET_REDUCE_H_
#define NNET_REDUCE_H_

#include "nnet_common.h"
#include "hls_stream.h"

namespace nnet {

struct reduce_config {
    static const unsigned n_in = 1;
    static const unsigned grid_size = 1;
};

template<class data_T, class res_T, typename CONFIG_T>
void reduce(
    hls::stream<data_T> &data,
    hls::stream<res_T> &res
) {

    static const ap_ufixed<8,0> threshold = 0.6;

    ReduceLoopRow:
    for(int my = 0; my < 3; my++){
      ReduceLoopCol:
      for(int mx = 0; mx < 8; mx++){
        #pragma HLS PIPELINE

        data_T in_data = data.read();
        res_T out_data; 
        #pragma HLS DATA_PACK variable=out_data
        
        out_data[0].range(26,26) = (in_data[0] > threshold) ? 1 : 0; // If probability is > 0.6, we consider the digit to be present

        /* Calculate Bounding Box */
        out_data[0].range(25,19) = (CONFIG_T::grid_size * mx) + ((in_data[1] + in_data[3]) / 2) - 24; // px = (mx * grid_size) + x1, 7 bits
        out_data[0].range(18,13) = (CONFIG_T::grid_size * my) + 0 //in_data[2];  py = (my * grid_size) + y1, 6 bits
        out_data[0].range(12,6) = (CONFIG_T::grid_size * mx) + ((in_data[1] + in_data[3]) / 2) + 24; // px + x2, 7 bits
        out_data[0].range(5,0) = (CONFIG_T::grid_size * my) + 47 //in_data[2] + in_data[4]; py + y2, 6 bits

        // // Calculate argmax, this is prediction
        // typename data_T::value_type max = 0;
        // for(unsigned i = 5; i < 15; i++){
        //   #pragma HLS UNROLL
        //   if(in_data[i] > max){
        //     max = in_data[i];
        //     out_data[0].range(3,0) = i-5;
        //   }
        // }

        res.write(out_data);
      }
    }
  }
}

#endif