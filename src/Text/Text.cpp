#include <stdlib.h>
#include <errno.h>
#include "Text.h"

/*!
@method create_file_and_write.
@abstract Creates a file with name file_path and writes the chars from text in it.
Returns a pointer to the created file.
@param file_path: The path of the file to create.
@param text: Text to write in the file.
*/
FILE* TextFileHandling::create_file_and_write(char* file_path, unsigned char* text) {
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
unsigned int TextFileHandling::read_text(char* file_path, unsigned char* text, unsigned int amount_of_chars) {
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
