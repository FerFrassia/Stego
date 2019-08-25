#ifndef __LSBWRITER_H__
#define __LSBWRITER_H__

#include "LSBStream.h"

class LSBWriter : LSBStream {
 public:
  LSBWriter(BMPImage &image, unsigned int lsb_to_use);

  void write_bytes(unsigned char* buffer, unsigned int count);

 private:
  void write_byte(unsigned char byte_to_write);
  void write_bit(unsigned char bit_to_write, unsigned int offset);
};

#endif
