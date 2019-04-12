#include<algorithm>
#include<assert.h>
#include<iostream>
#include <sstream>
#include<stdio.h>
#include  <stdlib.h>
#include"text_compression.h"

//----------------------------------------------------------------------------------------------
// @name                            : less_second
//
// @description                     : This is a comparator used to sort on the basis of the 
//                                    second parameter in a pair.
//----------------------------------------------------------------------------------------------
template <typename T1, typename T2>
struct less_second
{
    typedef pair<T1, T2> type;
    bool operator ()(type const& a, type const& b) const
    {
        return a.second < b.second;
    }
};

//----------------------------------------------------------------------------------------------
// @name                            : TextCompression
//
// @description                     : Constructor
//
// @returns                         : Nothing
//----------------------------------------------------------------------------------------------
TextCompression::TextCompression(CompressionAlgorithm_en algorithm)
{
    m_alogrithm = algorithm;
    m_totalPatterns = 0;
    m_patternCodeIndex = 0;
}

//----------------------------------------------------------------------------------------------
// @name                            : ~TextCompression
//
// @description                     : Destructor
//
// @returns                         : Nothing
//----------------------------------------------------------------------------------------------
TextCompression::~TextCompression()
{

}

//----------------------------------------------------------------------------------------------
// @name                            : ClearData
//
// @description                     : Clears all the data structures which should be blank
//                                    before performing a new compression/decompression. 
//
// @returns                         : Nothing
//----------------------------------------------------------------------------------------------
void TextCompression::ClearData()
{
    m_lines.clear();
    m_patternDictionary.clear();
    m_uncompressedFileExtension.clear();
    m_totalPatterns = 0;
    m_patternCodeIndex = 0;
}

//************************************************************************************************************
// Compression APIs
//************************************************************************************************************

//----------------------------------------------------------------------------------------------
// @name                            : Compress
//
// @description                     : Creates a new compressed file with extension COMPRESSED_FILE_EXTENSION.
//
// @returns                         : true if successful, false otherwise
//----------------------------------------------------------------------------------------------
bool TextCompression::Compress(const string & filename)
{
    // Clear all data structures first
    ClearData();

    printf("Input File: %s\n", filename.c_str());

    // Read file content
    if (!ReadLinesFromFile(filename))
    {
        return false;
    }

    // Prepare word dictionary
    size_t uniquePatterns = PreparePatternDictionary();

    DisplayPatternDictionary();
    DisplayOccuranceCountDictionary();

    // Write header and compressed data to file
    return CreateCompressedFile(filename);
}

//----------------------------------------------------------------------------------------------
// @name                            : PreparePatternDictionary
//
// @description                     : Creates dicitionary of pattern.
//
// @returns                         : Size of the pattern dictionary
//----------------------------------------------------------------------------------------------
size_t TextCompression::PreparePatternDictionary()
{
    printf("Preparing pattern dictionary...\n");

    size_t uniquePatterns = PreparePatternCountDictionary();

    vector<pair<string, unsigned long> > patternCountVector(m_patternCountDictionary.begin(), m_patternCountDictionary.end());
    sort(patternCountVector.begin(), patternCountVector.end(), less_second<string, unsigned long>());

    for (size_t i = patternCountVector.size() - 1; i > 0 ; i--)
    {
        string pattern = patternCountVector[i].first;
        unsigned long occurance = patternCountVector[i].second;
        string code = to_string(m_patternCodeIndex);

        // Calculate the header space that will be required to store this pattern in
        // the compressed file.
        int headerSizeForCode = pattern.size() + 1 + code.size();

        // If space can be saved on using coded word instead of the pattern, then we
        // add this pattern and code to our pattern dictionary.
        if (((code.size() * occurance) + headerSizeForCode )< pattern.size() * occurance)
        {
            m_patternDictionary[pattern] = code;
            m_patternCodeIndex++;
        }
    }

    printf("\n");
    printf("Total words   : %ld\n", m_totalPatterns);
    printf("Unique words  : %ld\n", m_patternDictionary.size());
    printf("Ratio         : %.2f\n", (float)m_totalPatterns / m_patternDictionary.size());
    printf("\n");

    return m_patternDictionary.size();
}

//----------------------------------------------------------------------------------------------
// @name                            : PreparePatternCountDictionary
//
// @description                     : Prepares the dictionary of all the words and their 
//                                    occurance count.
//
// @returns                         : Size of the dictionary formed
//----------------------------------------------------------------------------------------------
size_t TextCompression::PreparePatternCountDictionary()
{
    printf("Preparing pattern count dictionary...\n");
    for (size_t i = 0; i < m_lines.size(); i++)
    {
        string line = m_lines[i];
        stringstream ss(line);
        string word;

        // Extract words from the line
        // TODO: The demerit of using this is that leading spaces
        // will be ignored
        while (ss >> word)
        {
            m_totalPatterns++;
            m_patternCountDictionary[word]++;
        } // End of line
    }//End of file

    printf("\n");
    printf("Total words   : %ld\n", m_totalPatterns);
    printf("Unique words  : %ld\n", m_patternDictionary.size());
    printf("Ratio         : %.2f\n", (float)m_totalPatterns / m_patternDictionary.size());
    printf("\n");

    return m_patternDictionary.size();
}

//----------------------------------------------------------------------------------------------
// @name                            : DisplayPatternDictionary
//
// @description                     : Display all the words and their corresponding codes.
//
// @returns                         : Nothing
//----------------------------------------------------------------------------------------------
void TextCompression::DisplayPatternDictionary()
{
    printf("\n");
    printf("+----------------------------------------------+\n");
    printf("| Word                            Code         |\n");
    printf("+----------------------------------------------+\n");
    for (auto it = m_patternDictionary.begin(); it != m_patternDictionary.end(); it++)
    {
        printf("%-30s : %s\n", it->first.c_str(), it->second.c_str());
    }
}

//----------------------------------------------------------------------------------------------
// @name                            : DisplayOccuranceCountDictionary
//
// @description                     : Display all the words and their corresponding occurance counts.
//
// @returns                         : Nothing
//----------------------------------------------------------------------------------------------
void TextCompression::DisplayOccuranceCountDictionary()
{
    printf("\n");
    printf("+----------------------------------------------+\n");
    printf("| Word                            Occurances   |\n");
    printf("+----------------------------------------------+\n");
    for (auto it = m_patternCountDictionary.begin(); it != m_patternCountDictionary.end(); it++)
    {
        printf("%-30s : %lu\n", it->first.c_str(), it->second);
    }
}

//----------------------------------------------------------------------------------------------
// @name                            : ReadLinesFromFile
//
// @description                     : Reads all the lines from the given filename and stores
//                                    it in a vector of strings - m_lines.
//
// @returns                         : true if successful, false otherwise
//----------------------------------------------------------------------------------------------
bool TextCompression::ReadLinesFromFile(const string & filename)
{
    ifstream fileStream;
    fileStream.open(filename, ios::in);
    if (!fileStream)
    {
        printf("File [ %s ] NOT found!\n", filename.c_str());
        return false;
    }

    // Read the file
    std::string line;
    long int lineNumber = 0;
    printf("Loading file...\n");
    while (getline(fileStream, line))
    {
        m_lines.push_back(line);
        lineNumber++;
    }

    printf("Lines in file: %ld\n", lineNumber);

    // Close the file
    fileStream.close();

    return true;
}

//----------------------------------------------------------------------------------------------
// @name                            : CreateCompressedFile
//
// @description                     : Creates compressed file
//
// @returns                         : true if successful, false otherwise
//----------------------------------------------------------------------------------------------
bool TextCompression::CreateCompressedFile(const string & inputFilename)
{
    bool retval = false;

    // Create a output file
    ofstream fileStream;
    size_t extensionStart = inputFilename.find('.');
    if (extensionStart == std::string::npos)
    {
        printf("Invalid input file format!\n");
        return false;
    }

    m_uncompressedFileExtension = inputFilename.substr(extensionStart + 1);
    string outputFilename = inputFilename.substr(0, extensionStart + 1);
    outputFilename.append(COMPRESSED_FILE_EXTENSION);
    printf("Output file: %s\n", outputFilename.c_str());

    fileStream.open(outputFilename, ios::out);
    if (!fileStream.is_open())
    {
        printf("Could not create [%s]!\n", outputFilename.c_str());
        return false;
    }

    // Write Header
    retval = WriteCompressedHeader(fileStream);
    if (!retval)
    {
        return false;
    }

    // Write actual data
    retval = WriteCompressedData(fileStream);
    if (!retval)
    {
        return false;
    }

    // Close the file
    fileStream.close();

    return true;
}

//----------------------------------------------------------------------------------------------
// @name                            : WriteCompressedHeader
//
// @description                     : Write some header information in the compressed file. These
//                                    will be used at the time of decompression to initialize
//                                    the system.
//
// @returns                         : true if successful, false otherwise
//----------------------------------------------------------------------------------------------
bool TextCompression::WriteCompressedHeader(ofstream & fileStream)
{
    // Algorithm used
    fileStream << m_alogrithm << endl;

    // File extension before compression
    fileStream << m_uncompressedFileExtension << endl;

    // Size of dictionary
    fileStream << m_patternDictionary.size() << endl;

    // Content of dictionary. Here we are storing in the format:
    // pattern<space>codeWord, where
    // key   : pattern (word)
    // value : codeWord
    for (auto it = m_patternDictionary.begin(); it != m_patternDictionary.end(); it++)
    {
        fileStream << it->first << " " << it->second << endl;
    }

    return true;
}

//----------------------------------------------------------------------------------------------
// @name                            : WriteCompressedData
//
// @description                     : Write down the coded form of the data into the 
//                                    compressed file.
//
// @returns                         : true if successful, false otherwise
//----------------------------------------------------------------------------------------------
bool TextCompression::WriteCompressedData(ofstream & fileStream)
{
    for (size_t i = 0; i < m_lines.size(); i++)
    {
        string line = m_lines[i];
        stringstream ss(line);
        string word;

        // Extract words from the line
        while (ss >> word)
        {
            // Find code of this word from dictionary
            string code = findCodeForPattern(word); 
            fileStream << code << " ";
        } // End of line

        fileStream << endl;
    }

    return true;
}


//************************************************************************************************************
// Decompression APIs
//************************************************************************************************************

//----------------------------------------------------------------------------------------------
// @name                            : Decompress
//
// @description                     : Creates a decompressed version of the provided file.
//
// @returns                         : true if successful, false otherwise
//----------------------------------------------------------------------------------------------
bool TextCompression::Decompress(const string & filename)
{
    // Clear all data structures first
    ClearData();

    // Read file content
    if (!ReadCompressedFile(filename))
    {
        return false;
    }

    // Write header and compressed data to file
    return CreateDecompressedFile(filename);
}

//----------------------------------------------------------------------------------------------
// @name                            : ReadCompressedFile
//
// @description                     : Reads the content of compressed file. 
//
// @returns                         : true if successful, false otherwise
//----------------------------------------------------------------------------------------------
bool TextCompression::ReadCompressedFile(const string & filename)
{
    ifstream fileStream;
    fileStream.open(filename, ios::in);
    if (!fileStream)
    {
        printf("File [ %s ] NOT found!\n", filename.c_str());
        return false;
    }

    bool retval = false;
    retval = ReadCompressedHeader(fileStream);
    if (!retval)
    {
        return false;
    }

    retval = ReadCompressedData(fileStream);
    if (!retval)
    {
        return false;
    }

    // Close the file
    fileStream.close();

    return true;
}

//----------------------------------------------------------------------------------------------
// @name                            : ReadCompressedHeader
//
// @description                     : Reads the header and validates it with the current system.
//
// @returns                         : true if successful, false otherwise
//----------------------------------------------------------------------------------------------
bool TextCompression::ReadCompressedHeader(ifstream & fileStream)
{
    // Algorithm used
    int algorithmFromFile;
    fileStream >> algorithmFromFile;
    printf("\nReading header from compressed file...\n");
    printf("algorithmFromFile: %d, m_alogrithm: %d\n", algorithmFromFile, m_alogrithm);
    if (algorithmFromFile != m_alogrithm)
    {
        printf("Algorithm mismatch!\n");
        return false;
    }

    // File extension before compression
    fileStream >> m_uncompressedFileExtension;
    cout << "m_uncompressedFileExtension: " << m_uncompressedFileExtension << endl;

    // Size of dictionary
    size_t dictSize = 0;
    fileStream >> dictSize;
    cout << "dictSize: " << dictSize << endl;

    // Content of dictionary
    for (size_t i = 0; i < dictSize; i++)
    {
        string pattern;
        long int code = 0;
        fileStream >> pattern >> code;
        
        // Preparing dictionary. Here we are storing in the format:
        // codeWord<space>pattern, where
        // key   : codeWord
        // value : pattern (word)
        // This is the reverse of that used at the time of creating the 
        // compressed file. This is being done for faster lookup at the 
        // time of decompression. This is possible because both pattern
        // and the codedWord are unique for each entry.
        m_patternDictionary[to_string(code)] = pattern;
    }

    printf("[ DONE ]\n");
    return true;
}

//----------------------------------------------------------------------------------------------
// @name                            : ReadCompressedData
//
// @description                     : Converts the coded words into their original form via
//                                    dictionary lookup. The entire data is converted and
//                                    all the lines are stored in a vector of strings - m_lines.
//
// @returns                         : true if successful, false otherwise
//----------------------------------------------------------------------------------------------
bool TextCompression::ReadCompressedData(ifstream & fileStream)
{
    std::string compressedLineContent;

    while (getline(fileStream, compressedLineContent))
    {
        //cout << compressedLineContent << endl;
        stringstream ss(compressedLineContent);
        string codedWord;
        string line;
        while (ss >> codedWord)
        {
            string word = findPatternFromCode(codedWord);
            line.append(word);
            line.append(" ");
        } // End of line

        m_lines.push_back(line);
    }// End of file

    cout << "Data lines: " << m_lines.size()<<endl;
    return true;
}

//----------------------------------------------------------------------------------------------
// @name                            : CreateDecompressedFile
//
// @description                     : Write the decompressed data in m_lines to the output
//                                    file.
//
// @returns                         : true if successful, false otherwise
//----------------------------------------------------------------------------------------------
bool TextCompression::CreateDecompressedFile(const string & inputFilename)
{
    bool retval = false;

    // Create a output file
    ofstream fileStream;
    size_t extensionStart = inputFilename.find('.');
    if (extensionStart == std::string::npos)
    {
        printf("Invalid input file format!\n");
        return false;
    }

    string outputFilename = inputFilename.substr(0, extensionStart);
    outputFilename.append("_decompressed.");
    outputFilename.append(m_uncompressedFileExtension);
    printf("Output file: %s\n", outputFilename.c_str());

    fileStream.open(outputFilename, ios::out);
    if (!fileStream.is_open())
    {
        printf("Could not create [%s]!\n", outputFilename.c_str());
        return false;
    }

    // Write the decompressed data
    retval = WriteDecompressedData(fileStream);
    if (!retval)
    {
        return false;
    }

    // Close the file
    fileStream.close();

    return true;
}

//----------------------------------------------------------------------------------------------
// @name                            : WriteDecompressedData
//
// @description                     : Write all the lines to the file.
//
// @returns                         : true if successful, false otherwise
//----------------------------------------------------------------------------------------------
bool TextCompression::WriteDecompressedData(ofstream & fileStream)
{
    // Writing data to file
    for (size_t i = 0; i < m_lines.size(); i++)
    {
        fileStream << m_lines[i] << endl;
    }

    return true;
}

//----------------------------------------------------------------------------------------------
// @name                            : findPatternFromCode
//
// @description                     : Fetch the word for the given code.
//
// @returns                         : if coded word is present in dictionary then word for the 
//                                    code is returned, else a codedWord itself is returned
//----------------------------------------------------------------------------------------------
string TextCompression::findPatternFromCode(const string & codedWord)
{
    auto it = m_patternDictionary.find(codedWord);
    if (it != m_patternDictionary.end())
    {
        return it->second;
    }

    // No pattern available for this codeWord
    return codedWord;
}

//----------------------------------------------------------------------------------------------
// @name                            : findCodeForPattern
//
// @description                     : Find out the code for the given pattern
//
// @returns                         : Code for the pattern if present, otherwise the pattern
//                                    itself is returned
//----------------------------------------------------------------------------------------------
string TextCompression::findCodeForPattern(const string & pattern)
{
    auto it = m_patternDictionary.find(pattern);
    if (it != m_patternDictionary.end())
    {
        return it->second;
    }

    // No code present for this pattern
    return pattern;
}