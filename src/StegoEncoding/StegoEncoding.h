#ifndef __STEGOENCODING_H__
#define __STEGOENCODING_H__

#include "../BMPImage/BMPImage.h"

namespace StegoEncoding {
  /*!
  @method write_stego.
  @abstract Modifies the provided image by writing the provided text in the least
  significant bit of each byte, in row order.
  @param input_text: The text to write.
  @param cover_img: The image where to hide the text.
  @param amount_of_chars: The metadata representing the amount of chars that will be written after it.
  @param max_lsb_to_use: Maximum number of LSBs to use.
  @discussion This method first checks if the stego image to create will be large enough to hold the entire text.
  If not, it exits with a failure message.
  @return The number of LSBs actually used to hide the text.
  */
  unsigned int write_stego(
    unsigned char* input_text,
    BMPImage &cover_img,
    unsigned int amount_of_chars,
    unsigned int max_lsb_to_use
  );

  /*!
  @method read_stego.
  @abstract Extract from the image the text represented by the least significant bits
  of every bytes. The total amount of bytes to be read is assumed to be encoded
  as an unsigned integer in the first four bytes of the image.
  @param stego_img: The image from which to extract the text.
  @param lsb_to_use: The number of LSBs to use.
  @return A pointer to a buffer containing the extracted text.
  */
  unsigned char* read_stego(BMPImage &stego_img, unsigned int lsb_to_use);
};

#endif
