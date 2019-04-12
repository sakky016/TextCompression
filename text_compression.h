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

// This controls the patterns which have to be converted to coded form. So for instance
// if this value is 2, then it means that after coding the size occupied by all the occurances
// of a particular pattern will take 1/2 the space. Lower this value, more patterns will be
// converted into coded form (for compression)
const float SPACE_SAVE_FACTOR = 1.5;

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
    unordered_map<string, string>            m_patternDictionary;
    unordered_map<string, unsigned long>     m_patternCountDictionary;
    long int                                 m_totalPatterns;
    long int                                 m_patternCodeIndex;
    string                                   m_uncompressedFileExtension;

public:
    TextCompression(CompressionAlgorithm_en algorithm);
    ~TextCompression();
    void DisplayPatternDictionary();
    void DisplayOccuranceCountDictionary();

    bool Compress(const string & filename);
    bool ReadLinesFromFile(const string & filename);
    size_t PreparePatternDictionary();
    size_t OptimizePatternDictionary();
    bool CreateCompressedFile(const string & filename);
    bool WriteCompressedHeader(ofstream & fileStream);
    bool WriteCompressedData(ofstream & fileStream);

    bool Decompress(const string & filename);
    bool ReadCompressedFile(const string & filename);
    bool ReadCompressedHeader(ifstream & fileStream);
    bool ReadCompressedData(ifstream & fileStream);
    bool CreateDecompressedFile(const string & inputFilename);
    bool WriteDecompressedData(ofstream & fileStream);
    string findPatternFromCode(const string & codedWord);
    string findCodeForPattern(const string & pattern);

    void ClearData();
};

#endif
