#ifndef __STEGOENCODER_H__
#define __STEGOENCODER_H__

#include "../BMPImage/BMPImage.h"

class StegoEncoder {
 public:
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
  static unsigned int write_stego(
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
  static unsigned char* read_stego(BMPImage &stego_img, unsigned int lsb_to_use);

 private:
  /*!
  @method determine_min_amount_of_lsb
  @abstract Determines the minimum amount of Least Significant Bits required to encode the chars in the image.
  Returns -1 if the image is not large enough to encode the image.
  @params image: The image to check as container.
  @params number_of_chars: The amount of chars to check as source.
  */
  static int determine_min_amount_of_lsb(BMPImage &image, unsigned int number_of_chars);
  
  static void split_into_bytes(unsigned int number, unsigned char* bytes);
  static unsigned int join_bytes(unsigned char* bytes);
};

#endif
