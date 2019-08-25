#ifndef __TEXT_H__
#define __TEXT_H__

#include <stdio.h>

namespace TextFileHandling {
  /*!
  @method create_file_and_write.
  @abstract Creates a file with name file_path and writes the chars from text in it.
  Returns the amount of bytes written.
  @param file_path: The path of the file to create.
  @param text: Text to write in the file.
  */
  int create_file_and_write(char* file_path, unsigned char* text);

  /*!
  @method read_text.
  @abstract Opens the text file and writes it's contents in the variable text.
  Returns the size of the text.
  @param file_path: The path of the file to open.
  @param text: The variable in which to write to content of the file.
  @param amount_of_chars: The amount of characters to read from the text.
  @discussion Skips the first line because the text starts at the second line.
  */
  unsigned int read_text(char* file_path, unsigned char* text, unsigned int amount_of_chars);
};

#endif
