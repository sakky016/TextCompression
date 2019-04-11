#ifndef _TEXT_COMPRESSION_H_
#define _TEXT_COMPRESSION_H_

#include<fstream>
#include<string>
#include<unordered_map>
#include<vector>

using namespace std;

//-----------------------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------------------
const string COMPRESSED_FILE_EXTENSION = "xix";
const string CODED_WORD_NOT_FOUND_MARKER = "~^~";

//-----------------------------------------------------------------------------------------
// Enums
//-----------------------------------------------------------------------------------------
enum CompressionAlgorithm_en
{
    COMPRESSION_SIMPLE_WORD_DICTIONARY,
    COMPRESSION_LZW
};

//-----------------------------------------------------------------------------------------
// Text Compression class
//-----------------------------------------------------------------------------------------
class TextCompression
{
private:
    CompressionAlgorithm_en                  m_alogrithm;
    vector<string>                           m_lines;
    unordered_map<string, long int>          m_patternDictionary;
    long int                                 m_totalPatterns;
    long int                                 m_patternCodeIndex;
    string                                   m_uncompressedFileExtension;

public:
    TextCompression(CompressionAlgorithm_en algorithm);
    ~TextCompression();
    void DisplayDictionary();

    bool Compress(const string & filename);
    bool ReadLinesFromFile(const string & filename);
    size_t PrepareWordDictionary();
    bool CreateCompressedFile(const string & filename);
    bool WriteCompressedHeader(ofstream & fileStream);
    bool WriteCompressedData(ofstream & fileStream);

    bool Decompress(const string & filename);
    bool ReadCompressedFile(const string & filename);
    bool ReadCompressedHeader(ifstream & fileStream);
    bool ReadCompressedData(ifstream & fileStream);
    bool CreateDecompressedFile(const string & inputFilename);
    bool WriteDecompressedData(ofstream & fileStream);
    string findCodedWordInDictionary(long int codedWord);

    void ClearData();
};

#endif
