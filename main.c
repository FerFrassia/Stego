#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "libbmp.h"
#include "zlib.h"


/*!
@method create_file_and_write.
@abstract Creates a file with name file_path and writes the chars from text in it.
Returns a pointer to the created file.
@param file_path: The path of the file to create.
@param text: Text to write in the file.
*/
FILE* create_file_and_write(char* file_path, char* text) {

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
unsigned int read_text(char* file_path, char* text, unsigned int amount_of_chars) {

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
@method open_image.
@abstract Opens the image file.
@param image_path: The path of the image to open.
@param input_img: The pointer that will point to the start of the image file.
*/
void open_image(char* image_path, bmp_img* input_img) {

    if (bmp_img_read(input_img, image_path) != BMP_OK) {
        printf("failure: opening input image\n");
    }
}


/*!
@method determine_min_amount_of_lsb
@abstract Determines the minimum amount of Least Significant Bits required to encode the chars in the image.
Returns -1 if the image is not large enough to encode the image.
@params input_img_path: The name of the image to check as container.
@params number_of_chars: The amount of chars to check as source.
*/
int determine_min_amount_of_lsb(char* input_img_path, unsigned int number_of_chars) {

    bmp_img* input_img = malloc(sizeof(bmp_img));
    open_image(input_img_path, input_img);

    double d_chars  = (double)number_of_chars;
    double img_size = (double)input_img->img_header.biSizeImage;

    int min_lsb = -1;
    for (double i = 1; i <= 8; ++i) {
        if (img_size >= ceil( (d_chars/i)*8) ) {
            min_lsb = i;
            break;
        }
    }

    free(input_img);
    return min_lsb;
}


/*!
@method bit_mask_byte.
@abstract Generates a bitmask with the k least significant bits set to 1.
Returns a char with the generated bitmask.
@param k: The number of least significant bits to set to 1.
*/
unsigned char bit_mask_byte(unsigned int k) {

    unsigned char bit_mask_char = 0x00;
    for (int i = 0; i < k; ++i) {
        bit_mask_char = bit_mask_char | (0x01 << i);
    }

    return bit_mask_char;
}


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
void write_stego_in_lbs(char* input_img_path, char* output_img_path, unsigned int number_of_chars, char* text, unsigned int k) {

    bmp_img* input_img = malloc(sizeof(bmp_img));
    open_image(input_img_path, input_img);

    //Split the number of chars into four chunks of one byte
    unsigned char bytes_to_store[4];
    bytes_to_store[0] = (unsigned char)((number_of_chars >> 24) & 0xFF);
    bytes_to_store[1] = (unsigned char)((number_of_chars >> 16) & 0xFF);
    bytes_to_store[2] = (unsigned char)((number_of_chars >> 8) & 0xFF);
    bytes_to_store[3] = (unsigned char)(number_of_chars & 0xFF);

    unsigned char *current_channel = (unsigned char*)(*(input_img->img_pixels));
    unsigned char bit_mask_char    = bit_mask_byte(1);
    unsigned char bit_mask_channel = ~(bit_mask_char << (k-1)); //This is NOT(bit_mask_byte << k-1)

    unsigned char bit_current_number = 0;
    unsigned char original_channel   = 0;
    unsigned char modified_channel   = 0;
    unsigned int lsb_used = 0;

    //Write number of chars
    for (int i = 0; i < 4; ++i) {
        char current_char = bytes_to_store[i];
        for (int j = 7; j >= 0; --j) {

            bit_current_number = (current_char >> j) & (bit_mask_char);
            original_channel   = *current_channel;
            modified_channel   = (original_channel & bit_mask_channel) | (bit_current_number << (k-1-lsb_used));
            *current_channel   = modified_channel;

            lsb_used++;
            if (lsb_used >= k) {
                current_channel += 1;
                lsb_used = 0;
            }
            bit_mask_channel = ~(bit_mask_char << (k-1-lsb_used));
        }
    }

    //Write text
    for (int i = 0; i < number_of_chars; ++i) {
        char current_char = text[i];
        for (int j = 7; j >= 0; --j) {

            bit_current_number = (current_char >> j) & (bit_mask_char);
            original_channel   = *current_channel;
            modified_channel   = (original_channel & bit_mask_channel) | (bit_current_number << (k-1-lsb_used));
            *current_channel   = modified_channel;

            lsb_used++;
            if (lsb_used >= k) {
                current_channel += 1;
                lsb_used = 0;
            }
            bit_mask_channel = ~(bit_mask_char << (k-1-lsb_used));
        }
    }

    //Store image
    bmp_img_write(input_img, output_img_path);
    free(input_img);
}


/*!
@method read_stego.
@abstract Opens the stego image, recovers the text encoded in the last significant bit of the concerning bytes,
and creates a text file with it.
@param stegoName: The path of the stego image.
@param outputTextName: The path of the text file to create with the stego text Extracted.
@param k: Number of LSBs to use.
*/
void read_stego(char* stego_img_path, char* outputTextPath, unsigned int k) {

    bmp_img *stego_img = malloc(sizeof(bmp_img));
    open_image(stego_img_path, stego_img);

    //Extract the number of characters to read;
    unsigned char bytes_extracted[4];
    unsigned char *current_channel = (unsigned char*)(*(stego_img->img_pixels));
    unsigned char bit_mask_char = bit_mask_byte(1);

    unsigned char current_char = 0;
    unsigned char current_bits = 0;
    unsigned int lsb_used = 0;

    for (int i = 0; i < 4; ++i) {

        current_char = 0;
        for (int j = 7; j >= 0; --j) {

            current_bits = (*current_channel >> (k-1-lsb_used)) & bit_mask_char;
            current_bits = current_bits << j;
            current_char = current_char | current_bits;

            lsb_used++;
            if (lsb_used >= k) {
                current_channel += 1;
                lsb_used = 0;
                bit_mask_char = bit_mask_byte(1);
            }
        }
        bytes_extracted[i] = current_char;
    }
    unsigned int amount_of_chars = ((unsigned int)bytes_extracted[0] << 24) | ((unsigned int)bytes_extracted[1] << 16) | ((unsigned int)bytes_extracted[2] << 8) | (unsigned int)bytes_extracted[3];


    //Debug text
    printf("\n.......text extracted.......\n\n");
    //

    //Read text
    char text[amount_of_chars+1];

    for (int i = 0; i < amount_of_chars; ++i) {

        current_char = 0;
        for (int j = 7; j >= 0; --j) {

            current_bits = (*current_channel >> (k-1-lsb_used)) & bit_mask_char;
            current_bits = current_bits << j;
            current_char = current_char | current_bits;

            lsb_used++;
            if (lsb_used >= k) {
                current_channel += 1;
                lsb_used = 0;
                bit_mask_char = bit_mask_byte(1);
            }
        }
        text[i] = current_char;
        //Debug text
        printf("%c", text[i]);
        //
    }
    text[amount_of_chars] = '\0';

    //Debug text
    printf("\n.......output preview.......\n\n");
    printf("%s\n", text);
    //

    //Store text
    create_file_and_write(outputTextPath, text);
    free(stego_img);
}


/*!
@method write_stego.
@abstract Creates a stego image (outputImg) by hiding the text from inputText and hiding it in input_img.
@params inputText: The path of the text file to hide.
@params input_img: The path of the image file to use as container.
@params outputImg: The path of the stego image to create.
@params amount_of_chars: The amount of characters to hide.
@param k: Number of LSBs to use.
*/
void write_stego(char* inputText, char* input_img, char* outputImg, unsigned int amount_of_chars, unsigned int k) {

    char* text = malloc(sizeof(char)*(amount_of_chars+1));
    unsigned int text_size = read_text(inputText, text, amount_of_chars);
    text[amount_of_chars] = '\0';

    int min_lsb = determine_min_amount_of_lsb(input_img, amount_of_chars);

    switch (min_lsb) {
        case -1:
            printf("fatal: not enough space in image\n");
            exit(EXIT_FAILURE);
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            write_stego_in_lbs(input_img, outputImg, text_size, text, k);
            break;
        default:
            printf("write_stego with amount of significant bits:%d - not implemented\n", min_lsb);
            exit(EXIT_FAILURE);
    }
    free(text);
}

/*!
@method determine_mean_squared_error between 2 images
@abstract estimate the average of the squares of the errors of the chars in the images.
Returns MSE.
@params input_img_path: The name of the image to compare.
@params input_img_path2: The name of the second image to compare.
@params number_of_chars: The amount of chars to check as source.
*/
double determine_mean_squared_error(char* input_img_path, char* input_img_path_2, unsigned int number_of_chars) {

    bmp_img* input_img = malloc(sizeof(bmp_img));
    open_image(input_img_path, input_img);

    bmp_img* input_img_2 = malloc(sizeof(bmp_img));
    open_image(input_img_path_2, input_img_2);

    double d_chars  = (double)number_of_chars;
    double img_size = (double)input_img->img_header.biSizeImage;
    double mean_squared_error = 0;


    for (size_t i = 0; i < img_size; i++){
        double blue_difference = ((double) (*(input_img->img_pixels[i])).blue - (*(input_img_2->img_pixels[i])).blue);
        blue_difference *= blue_difference;
        double green_difference = ((double) (*(input_img->img_pixels[i])).green - (*(input_img_2->img_pixels[i])).green);
        green_difference *= green_difference;
        double red_difference = ((double) (*(input_img->img_pixels[i])).red - (*(input_img_2->img_pixels[i])).red);
        red_difference *= red_difference;
        mean_squared_error += blue_difference + green_difference + red_difference;
    }
    
    mean_squared_error /= img_size*3;

    free(input_img);
    free(input_img_2);
    return mean_squared_error;
}


/*!
@method determine_PSNR between 2 images
@abstract estimate PSNR of the images images.
Returns MSE.
@params input_img_path: The name of the image to compare.
@params input_img_path2: The name of the second image to compare.
@params number_of_chars: The amount of chars to check as source.
 */
double determine_PSNR(char* input_img_path, char* input_img_path_2, unsigned int number_of_chars) {

    double mse = determine_mean_squared_error(input_img_path, input_img_path_2, number_of_chars);
    double peak_signalToNoise_ratio = 20 * log10(255) - 10 * log10(mse);
    return peak_signalToNoise_ratio;
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
    char* imput_img_path   = argv[4];
    char* output_text_path = argv[5];
    char* output_img_path  = argv[6];

    write_stego(input_text_path, imput_img_path, output_img_path, amount_of_chars, k);
    read_stego(output_img_path, output_text_path, k);

    return 0;
}
