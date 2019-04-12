# TextCompression
Provides functionality to compress and decompress text files. We basically store the coded version of the patterns found in the input text file by creating a dictionary of the patterns and their corresponding codes. The dictionary itself is stored in the compressed file which helps in cross-referencing the coded words at the time of decompression. Depending upon the structure of patterns in the given text file, it can provide compression of 10-70%. Higher the repetition in the file, greater is the compression.

Additionally, we can get greater compression if we don't preserve leading spaces in a line. This can be done by setting PRESERVE_LEADING_SPACES = false in text_compression.h;

Refer to sample_console_output.txt for details of execution.
