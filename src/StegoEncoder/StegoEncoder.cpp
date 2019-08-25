#include <math.h>

#include "StegoEncoder.h"
#include "../LSBStream/LSBWriter.h"
#include "../LSBStream/LSBReader.h"

unsigned int StegoEncoder::write_stego(
  unsigned char* input_text,
  BMPImage &cover_img,
  unsigned int amount_of_chars,
  unsigned int max_lsb_to_use
) {
    int lsb_to_use = determine_min_amount_of_lsb(cover_img, amount_of_chars);
  
    if (lsb_to_use < 0) {
      printf("fatal: not enough space in image\n");
      exit(EXIT_FAILURE);
    }
    else if (lsb_to_use > max_lsb_to_use) {
      printf("fatal: at least %d LSB are necessary\n", lsb_to_use);
      exit(EXIT_FAILURE);      
    }

    printf("using %d LSB to hide text\n", lsb_to_use);

    LSBWriter writer = LSBWriter(cover_img, lsb_to_use);

    //Split the number of chars into four chunks of one byte
    unsigned char bytes_to_store[4];
    split_into_bytes(amount_of_chars, bytes_to_store);

    //Write number of chars
    writer.write_bytes(bytes_to_store, 4);

    //Write text
    writer.write_bytes(input_text, amount_of_chars);

    return lsb_to_use;
};

unsigned char* StegoEncoder::read_stego(
  BMPImage &stego_img,
  unsigned int lsb_to_use
) {
  LSBReader reader = LSBReader(stego_img, lsb_to_use);

  //Extract the number of characters to read;
  unsigned char bytes_extracted[4];
  reader.read_bytes(bytes_extracted, 4);
  unsigned int amount_of_chars = join_bytes(bytes_extracted); 

  //Read text
  unsigned char* text = (unsigned char*) malloc(amount_of_chars + 1);
  reader.read_bytes(text, amount_of_chars);
  text[amount_of_chars] = '\0';

  //Debug text
  printf("\n.......output preview.......\n\n");
  printf("%s\n", text);
  //

  return text;
};

int StegoEncoder::determine_min_amount_of_lsb(
  BMPImage &image,
  unsigned int number_of_chars
) {
    double d_chars  = (double) number_of_chars;
    double img_size = (double) image.get_image_size();

    int min_lsb = -1;
    for (double i = 1; i <= 8; ++i) {
        if (img_size >= ceil( (d_chars/i)*8) ) {
            min_lsb = i;
            break;
        }
    }

    return min_lsb;
}

void StegoEncoder::split_into_bytes(unsigned int number, unsigned char* bytes) {
  bytes[0] = (unsigned char)((number >> 24) & 0xFF);
    bytes[1] = (unsigned char)((number >> 16) & 0xFF);
    bytes[2] = (unsigned char)((number >> 8) & 0xFF);
    bytes[3] = (unsigned char)(number & 0xFF);
}

unsigned int StegoEncoder::join_bytes(unsigned char* bytes) {
  return ((unsigned int)bytes[0] << 24)
    | ((unsigned int)bytes[1] << 16)
    | ((unsigned int)bytes[2] << 8)
    | (unsigned int)bytes[3];
}