#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "libbmp.h"
#include "zlib.h"


#define INPUT_TEXT "/Users/fer/Downloads/Workspace/Stego/input.txt"
#define OUTPUT_TEXT "/Users/fer/Downloads/Workspace/Stego/output.txt"
#define INPUT_IMG "/Users/fer/Downloads/Workspace/Stego/input.bmp"
#define OUTPUT_IMG "/Users/fer/Downloads/Workspace/Stego/output.bmp"


/*!
@method createFileAndWrite.
@abstract Creates a file with name fileName and writes the chars from text in it.
Returns a pointer to the created file.
@param fileName: Name of the file to create.
@param text: Text to write in the file.
*/
FILE* createFileAndWrite(char* fileName, char* text) {
    FILE* fp;
    if ((fp = fopen(fileName, "w")) == NULL) {
        printf("failed: createFileAndWrite, errno = %d\n", errno);
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s", text);
    fclose(fp);
    return fp;
}

/*!
@method readFirstNumber.
@abstract Opens the file, reads the first line and writes it in numberOfChars.
@param fileName: The name of the file to open.
@param numberOfChars: The variable to write the number of chars in the file.
@discussion The number written in the file does not count itself.
*/
void readFirstNumber(char* fileName, int* numberOfChars) {
    FILE *fp;
    if ((fp = fopen(fileName, "r")) == NULL) {
        printf("failed: fopen, errno = %d\n", errno);
        return;
    }

    if (fscanf(fp, "%d", numberOfChars) != 1) {
        printf("failed: fscanf numberOfChars, errno = %d\n", errno);
    }

    fclose(fp);
}

/*!
@method readText.
@abstract Opens the text file and writes it's contents in the variable text.
@param fileName: The name of the file to open.
@param text: The variable in which to write to content of the file.
@discussion Skips the first line because the text starts at the second line.
*/
void readText(char* fileName, char text[]) {
    FILE *fp;
    if ((fp = fopen(fileName, "r")) == NULL) {
        printf("failed: fopen, errno = %d\n", errno);
        return;
    }

    //skip the first line
    fgets(text, 800, fp);

    char buffer[100];
    int textIndex = 0;
    while (fgets(buffer, sizeof(buffer)+sizeof(char), fp) != NULL) {
        int i = 0;
        while (buffer[i] != '\0') {
            text[textIndex] = buffer[i];
            ++textIndex;
            ++i;
        }
    }

    fclose(fp);
}

/*!
@method openImage.
@abstract Opens the image file.
@param imageName: The name of the image to open.
@param inputImg: The pointer that will point to the start of the image file.
*/
void openImage(char* imageName, bmp_img* inputImg) {
    if (bmp_img_read(inputImg, imageName) != BMP_OK) {
        printf("failure: opening input image\n");
    }
}

/*!
@method determineMinAmountOfLSB
@abstract Determines the minimum amount of Least Significant Bits required to encode the chars in the image.
Returns -1 if the image is not large enough to encode the image.
@params inputImgName: The name of the image to check as container.
@params numberOfChars: The amount of chars to check as source.
*/
int determineMinAmountOfLSB(char* inputImgName, unsigned int numberOfChars) {
    bmp_img* inputImg = malloc(sizeof(bmp_img));
    openImage(inputImgName, inputImg);

    double dChars = (double)numberOfChars;
    double imgSize = (double)inputImg->img_header.biSizeImage;

    int minLSB = -1;
    for (double i = 1; i <= 8; ++i) {
        if (imgSize >= ceil(dChars/i*8)) {
            minLSB = i;
            break;
        }
    }

    free(inputImg);
    return minLSB;
}

/*!
@method writeStegoInLeastSignificant.
@abstract Opens the original image, and creates a stego image by copying the original image and writting the text in the least
significant bit of every byte (this will represent a RGB channel).
@param inputName: The name of the original image.
@param outputName: The name of the stego image to create.
@param numberOfChars: The metadata representing the amount of chars that will be written after it.
@param text: The text to write.
@discussion This method first checks if the stego image to create will be large enough to hold the entire text.
If not, it exits with a failure message.
*/
void writeStegoInLeastSignificantBit(char* inputName, char* outputName, unsigned int numberOfChars, char* text) {
    bmp_img* inputImg = malloc(sizeof(bmp_img));
    openImage(inputName, inputImg);

    //write numberOfChars
    unsigned char *currentChannel = (unsigned char*)(*(inputImg->img_pixels));
    unsigned int shiftableNumberOfChars = numberOfChars;
    for (int bit = 31; bit >= 0; --bit) {
        unsigned char bitCurrentNumber = (shiftableNumberOfChars >> bit) & 0x01;
        unsigned char originalChannel = *currentChannel;
        unsigned char modifiedChannel = (originalChannel & 0xfe) | bitCurrentNumber;
        *currentChannel = modifiedChannel;
        currentChannel += 1;
    }

    //write text
    for (int i = 0; i < numberOfChars; ++i) {
        char currentChar = text[i];
        for (int bitOffset = 0; bitOffset < 8; ++bitOffset) {
            unsigned char bitCurrentChar = (currentChar >> (8-bitOffset-1)) & 0x01;
            unsigned char originalChannel = *currentChannel;
            unsigned char modifiedChannel = (originalChannel & 0xfe) | bitCurrentChar;
            *currentChannel = modifiedChannel;
            currentChannel += 1;
        }
    }

    bmp_img_write(inputImg, outputName);
    free(inputImg);
}

/*!
@method writeStego.
@abstract Creates a stego image (outputImg) by hiding the text from inputText and hiding it in inputImg.
@params inputText: The name of the text file to hide.
@params inputImg: The name of the image file to use as container.
@params outputImg: The name of the stego image to create.
*/
void writeStego(char* inputText, char* inputImg, char* outputImg) {
    int amountOfChars;
    readFirstNumber(inputText, &amountOfChars);

    char* text = malloc(sizeof(char)*amountOfChars);
    readText(inputText, text);

    int minLSB = determineMinAmountOfLSB(inputImg, amountOfChars);
    switch (minLSB) {
        case -1:
            printf("fatal: not enough space in image\n");
            exit(EXIT_FAILURE);
        case 1:
            writeStegoInLeastSignificantBit(inputImg, outputImg, amountOfChars, text);
            break;
        default:
            printf("writeStego with amount of significant bits:%d - not implemented\n", minLSB);
            exit(EXIT_FAILURE);
    }
    free(text);
}

/*!
@method readStego.
@abstract Opens the stego image, recovers the text encoded in the last significant bit of the concerning bytes,
and creates a text file with it.
@param stegoName: The name of the stego image.
@param outputTextName: The name of the text file to create with the stego text recovered.
*/
void readStego(char* stegoName, char* outputTextName) {
    bmp_img *stegoImg = malloc(sizeof(bmp_img));
    openImage(stegoName, stegoImg);

    unsigned char* currentChannel = (unsigned char*)(*(stegoImg->img_pixels));

    //recover amount of chars to read;
    int amountOfChars = 0;
    for (int bit = 31; bit >= 0; --bit) {
        int currentBit = *currentChannel & 0x01;
        currentBit = currentBit << bit;
        amountOfChars = amountOfChars | currentBit;
        currentChannel += 1;
    }

    //read chars
    char text[amountOfChars];
    for(int i = 0; i < amountOfChars; ++i) {
        char currentChar = 0;
        for (int bit = 7; bit >= 0; --bit) {
            int currentBit = *currentChannel & 0x01;
            currentBit = currentBit << bit;
            currentChar = currentChar | currentBit;
            currentChannel += 1;
        }
        text[i] = currentChar;
    }

    createFileAndWrite(outputTextName, text);
    free(stegoImg);
}


/*!
@method main.
@abstract Creates a stego image (OUTPUT_IMG) by hiding the text from INPUT_TEXT in INPUT_IMG.
Then recovers the hidden text from OUTPUT_IMG and writes it in OUTPUT_TEXT.
*/
int main() {
    writeStego(INPUT_TEXT, INPUT_IMG, OUTPUT_IMG);
    readStego(OUTPUT_IMG, OUTPUT_TEXT);
    return 0;
}