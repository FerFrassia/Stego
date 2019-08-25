#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "BMPImage/libbmp.h"

#include "BMPImage/BMPImage.h"
#include "StegoEncoder/StegoEncoder.h"

/*!
@method create_file_and_write.
@abstract Creates a file with name file_path and writes the chars from text in it.
Returns a pointer to the created file.
@param file_path: The path of the file to create.
@param text: Text to write in the file.
*/
FILE* create_file_and_write(char* file_path, unsigned char* text) {
    FILE* fp;
    if ((fp = fopen(file_path, "w")) == NULL) {
        printf("failed: create_file_and_write, errno = %d\n", errno);
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "%s", text);
    fclose(fp);
    return fp;
}

/*!
@method read_text.
@abstract Opens the text file and writes it's contents in the variable text.
Returns the size of the text.
@param file_path: The path of the file to open.
@param text: The variable in which to write to content of the file.
@param amount_of_chars: The amount of characters to read from the text.
@discussion Skips the first line because the text starts at the second line.
*/
unsigned int read_text(char* file_path, unsigned char* text, unsigned int amount_of_chars) {
    FILE *fp;
    if ((fp = fopen(file_path, "r")) == NULL) {
        printf("failed: fopen, errno = %d\n", errno);
        return 0;
    }

    unsigned int buffer_size = 100;
    char buffer[buffer_size];
    unsigned int text_index  = 0;
    while (fgets(buffer, sizeof(buffer)+sizeof(char), fp) != NULL) {

        int i = 0;
        while (buffer[i] != '\0' && text_index < amount_of_chars-1) {
            text[text_index] = buffer[i];
            ++text_index;
            ++i;
        }

    }

    fclose(fp);
    return text_index;
}

/*!
@method read_stego_from_file.
@abstract Opens the stego image, recovers the text encoded in the least significant bit of the concerning bytes,
and creates a text file with it.
@param stegoName: The path of the stego image.
@param outputTextName: The path of the text file to create with the stego text extracted.
@param lsb_to_use: Number of LSBs to use.
*/
void read_stego_from_file(
    char* stego_img_path,
    char* output_text_path,
    unsigned int lsb_to_use
) {
    BMPImage stego_img = BMPImage(stego_img_path);

    unsigned char* text = StegoEncoder::read_stego(
        stego_img,
        lsb_to_use
    );

    //Debug text
    printf("\n.......output preview.......\n\n");
    printf("%s\n", text);

    create_file_and_write(output_text_path, text);

    free(text);
}

/*!
@method write_stego_to_file.
@abstract Creates a stego image by hiding text in a cover image.
@params input_text_path: The path of the text file to hide.
@params cover_img_path: The path of the image file to use as container.
@params stego_img_path: The path of the stego image to create.
@params amount_of_chars: The amount of characters to hide.
@param max_lsb_to_use: Maximum number of LSBs to use.
@return The amount of LSB actually used to hide the text into the image.
*/
unsigned int write_stego_to_file(
    char* input_text_path,
    char* cover_img_path,
    char* stego_img_path,
    unsigned int amount_of_chars,
    unsigned int max_lsb_to_use
) {
    unsigned char* input_text = (unsigned char*) malloc(sizeof(char)*(amount_of_chars+1));
    read_text(input_text_path, input_text, amount_of_chars);
    input_text[amount_of_chars] = '\0';

    BMPImage cover_img = BMPImage(cover_img_path);

    unsigned int lsb_used = StegoEncoder::write_stego(
        input_text,
        cover_img,
        amount_of_chars,
        max_lsb_to_use
    );

    cover_img.save_image(stego_img_path);

    free(input_text);

    return lsb_used;
}

/*!
@method main.
@abstract Creates a stego image (OUTPUT_IMG) by hiding the text from INPUT_TEXT in INPUT_IMG.
Then recovers the hidden text from OUTPUT_IMG and writes it in OUTPUT_TEXT.
@param argv[1]: The number of LSBs to use.
@param argv[2]: The amount of characters to hide.
@param argv[3]: The path of the text to hide.
@param argv[4]: The path of the image to use as container.
@param argv[5]: The path of the output text.
@param argv[6]: The path of the output image.
*/
int main(int argc, char **argv) {
    if (argc < 7) {
        printf("Missing arguments.\n");
        exit(-1);
    }

    unsigned int k = atoi(argv[1]);
    if (k < 0 || k > 8) {
        printf("Invalid number of LSBs to use.\n");
        exit(-1);
    }

    unsigned int amount_of_chars = atoi(argv[2]);
    if (k < 0) {
        printf("Invalid amount of characters to hide.\n");
        exit(-1);
    }

    char* input_text_path  = argv[3];
    char* cover_img_path   = argv[4];
    char* output_text_path = argv[5];
    char* stego_img_path   = argv[6];

    unsigned int lsb_used = write_stego_to_file(
        input_text_path,
        cover_img_path,
        stego_img_path,
        amount_of_chars,
        k
    );

    read_stego_from_file(stego_img_path, output_text_path, lsb_used);

    return 0;
}
