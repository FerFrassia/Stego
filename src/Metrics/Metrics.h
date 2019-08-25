#ifndef __METRICS_H__
#define __METRICS_H__

#include "../BMPImage/BMPImage.h"

namespace Metrics {
  double determine_mean_squared_error(
    BMPImage &input_img,
    BMPImage &input_img_2
  );

  double determine_PSNR(
    BMPImage &input_img,
    BMPImage &input_img_2
  );

  double determine_SSIM(
    BMPImage &input_img,
    BMPImage &input_img_2
  );

};

#endif
