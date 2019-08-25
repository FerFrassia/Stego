#include "LSBReader.h"

LSBReader::LSBReader(BMPImage &image, unsigned int max_lsb_to_use) :
  LSBStream(image, max_lsb_to_use) {}

void LSBReader::read_bytes(unsigned char* buffer, unsigned int count) {
  for (int i = 0; i < count; ++i) {
    buffer[i] = read_byte();
  }
}

unsigned char LSBReader::read_byte() {
  unsigned char byte_read = 0;
  for (int j = 7; j >= 0; --j) {
    byte_read = byte_read | read_bit(current_lsb_offset) << j;
    advance_current_lsb_offset();
  }
  return byte_read;
}

unsigned char LSBReader::read_bit(unsigned int offset) {
  return (image.read_byte() & create_bit_mask(offset)) >> offset;
}
