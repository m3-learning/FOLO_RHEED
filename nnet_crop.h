#ifndef NNET_CROP_H_
#define NNET_CROP_H_

#include "nnet_common.h"
#include "hls_stream.h"

namespace nnet {

struct crop_config {
    static const unsigned input_height = 128;
    static const unsigned input_width = 48;
    static const unsigned output_height = 48;
    static const unsigned output_width = 48;
    static const unsigned n_crops = 5;
};

template<class data_T, class original_image_T, class res_T, typename CONFIG_T>
void crop(
    hls::stream<data_T> &data,
    hls::stream<original_image_T> &original_image,
    hls::stream<res_T> &res
) {

    data_T boxes[CONFIG_T::n_crops];
    for(int i = 0; i < CONFIG_T::n_crops; i++) {
        boxes[i] = data.read(); // Read box coordinates and confidence
    }

    for(int i = 0; i < CONFIG_T::n_crops; i++) {
        // Calculate cropping coordinates based on boxes[i]
        int x_center = boxes[i][0];
        int y_center = boxes[i][1];

        int x_start = x_center - CONFIG_T::output_width / 2;
        int y_start = y_center - CONFIG_T::output_height / 2;

        for(int y = 0; y < CONFIG_T::output_height; y++) {
            for(int x = 0; x < CONFIG_T::output_width; x++) {
                res_T pixel;
                if (x_start + x >= 0 && x_start + x < CONFIG_T::input_width &&
                    y_start + y >= 0 && y_start + y < CONFIG_T::input_height) {
                    pixel = original_image.read();
                } else {
                    pixel = 0; // Zero padding if out of bounds
                }
                res.write(pixel);
            }
        }
    }
}

} // namespace nnet

#endif