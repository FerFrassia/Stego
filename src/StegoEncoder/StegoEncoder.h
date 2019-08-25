#ifndef __STEGOENCODER_H__
#define __STEGOENCODER_H__

#include "../BMPImage/BMPImage.h"

class StegoEncoder {
 public:
  /*!
  @method write_stegoInLeastSignificant.
  @abstract Opens the original image, and creates a stego image by copying the original image and writting the text in the least
  significant bit of every byte (this will represent a RGB channel).
  @param inputName: The path of the original image.
  @param outputName: The path of the stego image to create.
  @param number_of_chars: The metadata representing the amount of chars that will be written after it.
  @param text: The text to write.
  @param k: Number of LSBs to use.
  @discussion This method first checks if the stego image to create will be large enough to hold the entire text.
  If not, it exits with a failure message.
  */
  static unsigned int write_stego(
    unsigned char* input_text,
    BMPImage &cover_img,
    unsigned int amount_of_chars,
    unsigned int max_lsb_to_use
  );

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
};

#endif
