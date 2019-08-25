#ifndef __METRICS_H__
#define __METRICS_H__

#include "../BMPImage/BMPImage.h"

class Metrics {
 public:
  static double determine_mean_squared_error(
    BMPImage input_img,
    BMPImage input_img_2,
    unsigned int number_of_chars
  );

  static double determine_PSNR(
    BMPImage input_img,
    BMPImage input_img_2,
    unsigned int number_of_chars
  );
};

#endif
