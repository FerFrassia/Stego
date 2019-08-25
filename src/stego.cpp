#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <chrono>

#include "Text/Text.h"
#include "BMPImage/BMPImage.h"
#include "StegoEncoding/StegoEncoding.h"

typedef typeof std::chrono::steady_clock::now() time_point;

struct time_measurements {
    time_point total_start;
    time_point total_end;
    time_point processing_start;
    time_point processing_end;
};


/*!
@method read_stego_from_file.
@abstract Opens the stego image, recovers the text encoded in the least significant bit of the concerning bytes,
and creates a text file with it.
@param stegoName: The path of the stego image.
@param outputTextName: The path of the text file to create with the stego text extracted.
@param time_measurements: Time measurements will be stored here.
@param lsb_to_use: Number of LSBs to use.
*/
int read_stego_from_file(
    char* stego_img_path,
    char* output_text_path,
    unsigned int lsb_to_use,
    time_measurements &time_measurements
) {
    time_measurements.total_start = std::chrono::steady_clock::now();

    BMPImage stego_img = BMPImage(stego_img_path);

    time_measurements.processing_start = std::chrono::steady_clock::now();
    unsigned char* text = StegoEncoding::read_stego(
        stego_img,
        lsb_to_use
    );
    time_measurements.processing_end = std::chrono::steady_clock::now();

    int text_length = TextFileHandling::create_file_and_write(output_text_path, text);

    free(text);
    time_measurements.total_end = std::chrono::steady_clock::now();
    return text_length;
}

/*!
@method write_stego_to_file.
@abstract Creates a stego image by hiding text in a cover image.
@params input_text_path: The path of the text file to hide.
@params cover_img_path: The path of the image file to use as container.
@params stego_img_path: The path of the stego image to create.
@params amount_of_chars: The amount of characters to hide.
@param max_lsb_to_use: Maximum number of LSBs to use.
@param time_measurements: Time measurements will be stored here.
@return The amount of LSB actually used to hide the text into the image.
*/
unsigned int write_stego_to_file(
    char* input_text_path,
    char* cover_img_path,
    char* stego_img_path,
    unsigned int amount_of_chars,
    unsigned int max_lsb_to_use,
    bool force_max_lsb,
    time_measurements &time_measurements
) {
    time_measurements.total_start = std::chrono::steady_clock::now();

    unsigned char* input_text = (unsigned char*) malloc(sizeof(char)*(amount_of_chars));
    TextFileHandling::read_text(input_text_path, input_text, amount_of_chars);

    BMPImage cover_img = BMPImage(cover_img_path);

    time_measurements.processing_start = std::chrono::steady_clock::now();
    unsigned int lsb_used = StegoEncoding::write_stego(
        input_text,
        cover_img,
        amount_of_chars,
        max_lsb_to_use,
        force_max_lsb
    );
    time_measurements.processing_end = std::chrono::steady_clock::now();
        
    cover_img.save_image(stego_img_path);

    free(input_text);
    time_measurements.total_end = std::chrono::steady_clock::now();
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
    time_measurements time_measurements;

    if (argc < 2) {
        fprintf(stderr, "A command should be provided.\n");
        fprintf(stderr, "Valid commands are: read, write.\n");
        return -1;
    }

    char* command = argv[1];

    if (strcmp(command, "read") == 0) {
        if (argc < 5) {
            fprintf(stderr, "Missing arguments.\n");
            fprintf(stderr, "Usage: %s read <stego_img_path> <output_text_path> <k>\n", argv[0]);
            return -1;
        }

        char* stego_img_path   = argv[2];
        char* output_text_path = argv[3];
    
        unsigned int lsb_to_use = atoi(argv[4]);
        if (lsb_to_use < 1 || lsb_to_use > 8) {
            fprintf(stderr, "Invalid amount of LSBs to use.\n");
            return -1;
        }

        int text_length = read_stego_from_file(
            stego_img_path,
            output_text_path,
            lsb_to_use,
            time_measurements
        );

        printf("%d characters successfully extracted.\n", text_length);
    }

    else if (strcmp(command, "write") == 0) {
        if (argc < 7) {
            fprintf(stderr, "Missing arguments.\n");
            fprintf(stderr, "Usage: %s write <text_path> <cover_img_path> <stego_img_path> <|t|> <k> [-f]\n", argv[0]);
            return -1;
        }

        char* input_text_path  = argv[2];
        char* cover_img_path   = argv[3];
        char* stego_img_path   = argv[4];

        unsigned int amount_of_chars = atoi(argv[5]);
        if (amount_of_chars < 0) {
            fprintf(stderr, "Invalid amount of characters to hide.\n");
            return -1;
        }

        unsigned int max_lsb_to_use = atoi(argv[6]);
        if (max_lsb_to_use < 1 || max_lsb_to_use > 8) {
            fprintf(stderr, "Invalid amount of LSBs to use.\n");
            return -1;
        }

        bool force_max_lsb = argc > 7 && strcmp(argv[7], "-f") == 0;

        unsigned int lsb_used = write_stego_to_file(
            input_text_path,
            cover_img_path,
            stego_img_path,
            amount_of_chars,
            max_lsb_to_use,
            force_max_lsb,
            time_measurements
        );

        printf("Stego image successfully created.\n");
        printf("Number of LSB used: %d.\n", lsb_used);
    }

    else {
        fprintf(stderr, "Invalid command.\n");
        fprintf(stderr, "Valid commands are: read, write.\n");
        return -1;
    }

    auto total_time = std::chrono::duration_cast<std::chrono::microseconds>
        (time_measurements.total_end - time_measurements.total_start).count();
    auto processing_time = std::chrono::duration_cast<std::chrono::microseconds>
        (time_measurements.processing_end - time_measurements.processing_start).count();
    printf("Elapsed time: processing  %d µs.\n", processing_time); 
    printf("              total       %d µs.\n", total_time);
    return 0;
}
