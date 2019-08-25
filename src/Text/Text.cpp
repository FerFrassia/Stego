#include <stdlib.h>
#include <errno.h>
#include "Text.h"

int TextFileHandling::create_file_and_write(char* file_path, unsigned char* text) {
    FILE* fp;
    if ((fp = fopen(file_path, "w")) == NULL) {
        printf("failed: create_file_and_write, errno = %d\n", errno);
        exit(EXIT_FAILURE);
    }

    int bytes_written = fprintf(fp, "%s", text);
    fclose(fp);
    return bytes_written;
}

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
