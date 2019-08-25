#ifndef __BPMIMAGE_H__
#define __BPMIMAGE_H__

#include <stdio.h>
#include "libbmp.h"

class BMPImage {
 public:
  BMPImage(char* image_path);
  ~BMPImage();

  unsigned char read_byte();
  bmp_pixel read_pixel();
  void write_byte(unsigned char byte_to_write);
  void next_byte();
  void next_pixel();
  void save_image(char* output_path);
  unsigned int get_image_size();

 private:
  bmp_img* image;
  unsigned int row_length;
  unsigned int current_row;
  unsigned int current_byte;

  /*!
  @method open_image.
  @abstract Opens the image file.
  @param image_path: The path of the image to open.
  @param input_img: The pointer that will point to the start of the image file.
  */
  void open_image(char* image_path);
  unsigned char* get_current_byte_pointer();
  bmp_pixel* get_pixel_pointer(int pixel_index);
  void advance_current_byte(unsigned int n);
};

#endif
