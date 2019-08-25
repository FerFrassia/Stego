#ifndef __LSBSTREAM_H__
#define __LSBSTREAM_H__

#include "BMPImage.h"

class LSBStream {
 public:
  LSBStream(char* image_path, unsigned int max_lsb_to_use);

 protected:
  BMPImage image;
  unsigned int max_lsb_to_use;
  unsigned int current_lsb_offset;

  void advance_current_lsb_offset();
  /*!
  @method create_bit_mask.
  @abstract Generates a bit mask with the k-th least significant bit set to 1.
  Returns a char with the generated bit mask.
  @param k: The index of the bit to set to 1.
  */
  static unsigned char create_bit_mask(unsigned int k);
};

#endif
