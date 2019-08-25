#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "libbmp.h"

#include "BMPImage.h"
#include "LSBReader.h"
#include "LSBWriter.h"

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
@method determine_min_amount_of_lsb
@abstract Determines the minimum amount of Least Significant Bits required to encode the chars in the image.
Returns -1 if the image is not large enough to encode the image.
@params input_img_path: The name of the image to check as container.
@params number_of_chars: The amount of chars to check as source.
*/
int determine_min_amount_of_lsb(char* input_img_path, unsigned int number_of_chars) {

    BMPImage input_img = BMPImage(input_img_path);

    double d_chars  = (double)number_of_chars;
    double img_size = (double)input_img.get_image_size();

    int min_lsb = -1;
    for (double i = 1; i <= 8; ++i) {
        if (img_size >= ceil( (d_chars/i)*8) ) {
            min_lsb = i;
            break;
        }
    }

    return min_lsb;
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
void write_stego_in_lbs(char* input_img_path, char* output_img_path, unsigned int number_of_chars, unsigned char* text, unsigned int k) {
    LSBWriter writer = LSBWriter(BMPImage(input_img_path), k);

    //Split the number of chars into four chunks of one byte
    unsigned char bytes_to_store[4];
    bytes_to_store[0] = (unsigned char)((number_of_chars >> 24) & 0xFF);
    bytes_to_store[1] = (unsigned char)((number_of_chars >> 16) & 0xFF);
    bytes_to_store[2] = (unsigned char)((number_of_chars >> 8) & 0xFF);
    bytes_to_store[3] = (unsigned char)(number_of_chars & 0xFF);

    //Write number of chars
    writer.write_bytes(bytes_to_store, 4);

    //Write text
    writer.write_bytes(text, number_of_chars);

    //Store image
    writer.finish(output_img_path);
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
    LSBReader reader = LSBReader(BMPImage(stego_img_path), k);

    //Extract the number of characters to read;
    unsigned char bytes_extracted[4];
    reader.read_bytes(bytes_extracted, 4);
    unsigned int amount_of_chars = ((unsigned int)bytes_extracted[0] << 24) | ((unsigned int)bytes_extracted[1] << 16) | ((unsigned int)bytes_extracted[2] << 8) | (unsigned int)bytes_extracted[3];

    //Debug text
    printf("\n.......text extracted.......\n\n");
    //

    //Read text
    unsigned char text[amount_of_chars+1];
    reader.read_bytes(text, amount_of_chars);
    text[amount_of_chars] = '\0';

    //Debug text
    printf("\n.......output preview.......\n\n");
    printf("%s\n", text);
    //

    //Store text
    create_file_and_write(outputTextPath, text);
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

    unsigned char* text = (unsigned char*)malloc(sizeof(char)*(amount_of_chars+1));
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
    BMPImage input_img = BMPImage(input_img_path);
    BMPImage input_img_2 = BMPImage(input_img_path_2);

    double d_chars  = (double)number_of_chars;
    double img_size = (double)input_img.get_image_size();
    double mean_squared_error = 0;

    for (size_t i = 0; i < img_size; i++){
        bmp_pixel pixel = input_img.read_pixel();
        bmp_pixel pixel_2 = input_img_2.read_pixel();

        double blue_difference = ((double) pixel.blue - pixel_2.blue);
        blue_difference *= blue_difference;
        double green_difference = ((double) pixel.green - pixel_2.green);
        green_difference *= green_difference;
        double red_difference = ((double) pixel.red - pixel_2.red);
        red_difference *= red_difference;
        mean_squared_error += blue_difference + green_difference + red_difference;

        input_img.next_pixel();
        input_img_2.next_pixel();
    }
    
    mean_squared_error /= img_size*3;
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
