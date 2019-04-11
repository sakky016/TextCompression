#include<iostream>
#include <sstream>
#include<stdio.h>
#include  <stdlib.h>
#include"text_compression.h"

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
    size_t uniquePatterns = PrepareWordDictionary();
    //DisplayDictionary();

    // Write header and compressed data to file
    return CreateCompressedFile(filename);
}

//----------------------------------------------------------------------------------------------
// @name                            : PrepareWordDictionary
//
// @description                     : Prepares the dictionary of all the words and their 
//                                    coded representation.
//
// @returns                         : Size of the dictionary formed
//----------------------------------------------------------------------------------------------
size_t TextCompression::PrepareWordDictionary()
{
    printf("Preparing pattern dictionary...\n");
    for (int i = 0; i < m_lines.size(); i++)
    {
        string line = m_lines[i];
        stringstream ss(line);
        string word;

        // Extract words from the line
        while (ss >> word)
        {
            m_totalPatterns++;

            // Check if this word is already present in our map
            auto it = m_patternDictionary.find(word);
            if (it == m_patternDictionary.end())
            {
                // Not present, add it to our dictionary
                m_patternDictionary[word] = m_patternCodeIndex;
                m_patternCodeIndex++;
            }
            else
            {
                // DO NOTHING. Ignore this word.
            }
        }
    }

    printf("\n");
    printf("Total words   : %ld\n", m_totalPatterns);
    printf("Unique words  : %ld\n", m_patternDictionary.size());
    printf("Ratio         : %.2f\n", (float)m_totalPatterns/m_patternDictionary.size());
    printf("\n");

    return m_patternDictionary.size();
}

//----------------------------------------------------------------------------------------------
// @name                            : DisplayDictionary
//
// @description                     : Display all the words and their corresponding codes.
//
// @returns                         : Nothing
//----------------------------------------------------------------------------------------------
void TextCompression::DisplayDictionary()
{
    printf("\n");
    printf("+----------------------------------------------+\n");
    printf("| Word                            Code         |\n");
    printf("+----------------------------------------------+\n");
    for (auto it = m_patternDictionary.begin(); it != m_patternDictionary.end(); it++)
    {
        printf("%-30s : %ld\n", it->first.c_str(), it->second);
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

    // Content of dictionary
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
    for (int i = 0; i < m_lines.size(); i++)
    {
        string line = m_lines[i];
        stringstream ss(line);
        string word;

        // Extract words from the line
        while (ss >> word)
        {
            // Find code of this word from dictionary
            long int code = m_patternDictionary[word];
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
        m_patternDictionary[pattern] = code;
    }

    cout << "dictionary size: " << m_patternDictionary.size() << endl;
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
            string word = findCodedWordInDictionary(atol(codedWord.c_str()));
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
    for (int i = 0; i < m_lines.size(); i++)
    {
        fileStream << m_lines[i] << endl;
    }

    return true;
}

//----------------------------------------------------------------------------------------------
// @name                            : findCodedWordInDictionary
//
// @description                     : Fetch the word for the given code.
//
// @returns                         : if coded word is present in dictionary then word for the 
//                                    code is returned, else a NOT FOUND marker
//----------------------------------------------------------------------------------------------
string TextCompression::findCodedWordInDictionary(long int codedWord)
{
    for (auto it = m_patternDictionary.begin(); it != m_patternDictionary.end(); it++)
    {
        if (it->second == codedWord)
        {
            return it->first;
        }
    }

    return CODED_WORD_NOT_FOUND_MARKER;
}