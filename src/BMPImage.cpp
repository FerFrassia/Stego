#include "BMPImage.h"

#include <stdlib.h>
#include <errno.h>
#include <math.h>

BMPImage::BMPImage(char* image_path) {
  image = (bmp_img*) malloc(sizeof(bmp_img));
  open_image(image_path);

  row_length = image->img_header.biWidth * sizeof(bmp_pixel);
  current_row = 0;
  current_byte = 0;
}

BMPImage::~BMPImage() {
  free(image);
}

unsigned char BMPImage::read_byte() {
  return *(get_current_byte_pointer());
}

bmp_pixel BMPImage::read_pixel() {
  int current_pixel = (int)floor(current_byte/sizeof(bmp_pixel));
  return *(get_pixel_pointer(current_pixel));
}

void BMPImage::write_byte(unsigned char byte_to_write) {
  unsigned char* current_byte_pointer = get_current_byte_pointer();
  *current_byte_pointer = byte_to_write;
}

void BMPImage::next_byte() {
  advance_current_byte(1);
}

void BMPImage::next_pixel() {
  advance_current_byte(sizeof(bmp_pixel) - current_byte % sizeof(bmp_pixel));
}

void BMPImage::save_image(char* output_path) {
  bmp_img_write(image, output_path);
}

unsigned int BMPImage::get_image_size() {
  return image->img_header.biSizeImage;
}

void BMPImage::open_image(char* image_path) {
  if (bmp_img_read(image, image_path) != BMP_OK) {
    printf("failure: opening input image\n");
  }
}

unsigned char* BMPImage::get_current_byte_pointer() {
  return (unsigned char*)(image->img_pixels[current_row]) + current_byte;
}

bmp_pixel* BMPImage::get_pixel_pointer(int pixel_index) {
  return (image->img_pixels[current_row]) + pixel_index;
}

void BMPImage::advance_current_byte(unsigned int n) {
  while (current_byte + n >= row_length) {
    n -= row_length - current_byte; 
    current_row += 1;
    current_byte = 0;
  }
  current_byte += n;
}
