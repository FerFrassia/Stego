#include "LSBWriter.h"

LSBWriter::LSBWriter(BMPImage image, unsigned int lsb_to_use) :
  LSBStream(image, lsb_to_use) {}

void LSBWriter::write_bytes(unsigned char* buffer, unsigned int count) {
  for (int i = 0; i < count; ++i) {
    write_byte(buffer[i]);
  }
}

void LSBWriter::write_byte(unsigned char byte_to_write) {
  for (int j = 7; j >= 0; --j) {
    unsigned char bit_to_write = (byte_to_write & create_bit_mask(j)) >> j;
    write_bit(bit_to_write, current_lsb_offset);
    advance_current_lsb_offset();
  }
}

void LSBWriter::write_bit(unsigned char bit_to_write, unsigned int offset) {
  unsigned char original_byte = image.read_byte();
  unsigned char bit_mask = ~ create_bit_mask(offset);
  unsigned char modified_byte = (original_byte & bit_mask) | (bit_to_write << offset);
  image.write_byte(modified_byte);
}

void LSBWriter::finish(char* image_path) {
  image.save_image(image_path);
}
