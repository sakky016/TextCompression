#include"text_compression.h"

//-----------------------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------------------
const string FILENAME = "sampleFiles/beggars.txt";

//-----------------------------------------------------------------------------------------
// Get size of file in bytes
//-----------------------------------------------------------------------------------------
long GetFileSize(const string & filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

//-----------------------------------------------------------------------------------------
// Main 
//-----------------------------------------------------------------------------------------
int main()
{
    size_t extensionStart = FILENAME.find('.');
    if (extensionStart == std::string::npos)
    {
        printf("Invalid input file format!\n");
        return false;
    }

    // Determine compressed filename
    string inputFilename = FILENAME;
    string compressedFilename = inputFilename.substr(0, extensionStart + 1);
    compressedFilename.append(COMPRESSED_FILE_EXTENSION);

    TextCompression tc(COMPRESSION_SIMPLE_WORD_DICTIONARY);

    bool doneCompression = tc.Compress(inputFilename);
    if (doneCompression)
        printf("\n*** Compression: SUCCESS\n");
    else
        printf("\n*** Compression: FAILED\n");

    bool doneDecompression = tc.Decompress(compressedFilename);
    if (doneDecompression)
        printf("\n*** Decompression: SUCCESS\n");
    else
        printf("\n*** Decompression: FAILED\n");

    printf("\n");
    printf("Input      %s     : %ld bytes\n", inputFilename.c_str(), GetFileSize(inputFilename));
    printf("Compressed %s     : %ld bytes\n", compressedFilename.c_str(), GetFileSize(compressedFilename));
    printf("Compression ratio : %.2f %%\n", 100 * ((float)(GetFileSize(inputFilename) - GetFileSize(compressedFilename))/ GetFileSize(compressedFilename)));
    getchar();
    return 0;
}