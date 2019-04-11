#include"text_compression.h"

//-----------------------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------------------
const string FILENAME = "sampleFiles/solitary.txt";

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
        printf("Compression: SUCCESS\n");
    else
        printf("Compression: FAILED\n");

    bool doneDecompression = tc.Decompress(compressedFilename);
    if (doneDecompression)
        printf("Decompression: SUCCESS\n");
    else
        printf("Decompression: FAILED\n");

    getchar();
    return 0;
}