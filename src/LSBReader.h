#ifndef __LSBREADER_H__
#define __LSBREADER_H__

#include "LSBStream.h"

class LSBReader : LSBStream {
 public:
  LSBReader(BMPImage image, unsigned int lsb_to_use);

  void read_bytes(unsigned char* buffer, unsigned int count);

 private:
  unsigned char read_byte();
  unsigned char read_bit(unsigned int offset);
};

#endif
