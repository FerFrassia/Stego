#include "LSBStream.h"

LSBStream::LSBStream(char* image_path, unsigned int max_lsb_to_use) :
  image(BMPImage(image_path)),
  max_lsb_to_use(max_lsb_to_use) {
  current_lsb_offset = 0;
}

void LSBStream::advance_current_lsb_offset() {
  current_lsb_offset++;
  if (current_lsb_offset >= max_lsb_to_use) {
    image.next_byte();
    current_lsb_offset = 0;
  }
}

unsigned char LSBStream::create_bit_mask(unsigned int k) {
    return 0x01 << k;
}
