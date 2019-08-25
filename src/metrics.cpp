#include <stdio.h>
#include <string.h>

#include "BMPImage/BMPImage.h"
#include "Metrics/Metrics.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "A metric name should be provided.\n");
    fprintf(stderr, "Valid metrics are: mse, psnr.\n");
    return -1;
  }
  char* metric_name = argv[1];

  if (argc < 4) {
    fprintf(stderr, "Missing arguments.\n");
    fprintf(stderr, "Usage: %s <metric> <image1> <image2>\n", argv[0]);
    return -1;
  }

  char* image_path = argv[2];
  char* image_path_2 = argv[3];

  BMPImage image = BMPImage(image_path);
  BMPImage image_2 = BMPImage(image_path_2);
  double result;

  if (strcmp(metric_name, "mse") == 0) {
    result = Metrics::determine_mean_squared_error(image, image_2, 0);
  }
  else if (strcmp(metric_name, "psnr") == 0) {
    result =  Metrics::determine_PSNR(image, image_2, 0);
  }
  else {
    fprintf(stderr, "Invalid metric.\n");
    fprintf(stderr, "Valid metrics are: mse, psnr.\n");
    return -1;
  }

  printf("%.3g\n", result);
  return 0;
}
