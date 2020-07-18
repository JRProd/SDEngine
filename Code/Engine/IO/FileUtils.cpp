#include "FileUtils.hpp"

#include <cstdio>
#include <fstream>

#include <io.h>

Strings GetAllFilesNamesInFolder( const char* folderPath, const char* filePattern )
{
    return GetAllFilesNamesInFolder( std::string( folderPath), std::string(filePattern) );
}

Strings GetAllFilesNamesInFolder( const char* folderPath, const std::string& filePattern )
{
    return GetAllFilesNamesInFolder( std::string( folderPath), filePattern );
}

Strings GetAllFilesNamesInFolder( const std::string& folderPath, const char* filePattern )
{
    return GetAllFilesNamesInFolder( folderPath, std::string(filePattern) );
}

Strings GetAllFilesNamesInFolder( const std::string& folderPath, const std::string& filePattern )
{
    Strings fileNames;
    std::string folder = folderPath;
    if( folder.at(folder.size() - 1 ) != '/' )
    {
        folder += "/";
    }
    const std::string pattern = filePattern.empty() ? "*" : filePattern;
    const std::string fileNamePattern = folder + pattern;

    _finddata_t fileInfo;
    const intptr_t searchHandle = _findfirst( fileNamePattern.c_str(), &fileInfo );
    while( searchHandle != -1 )
    {
        fileNames.push_back( fileInfo.name );
        const int errorCode = _findnext( searchHandle, &fileInfo );
        if( errorCode != 0 ) { break; }
    }

    return fileNames;
}

std::string TrimFileExtension( const std::string& filePath )
{
    const size_t indexOfPeriod = filePath.find_last_of( '.' );
    if( indexOfPeriod == std::string::npos ) { return filePath; }

    const size_t lenghtOfTrim = filePath.size() - ( filePath.size() - indexOfPeriod );
    return filePath.substr( 0, lenghtOfTrim );
}

void FileReadToVector( const std::string& fileName, OUT_PARAM Strings& fileData )
{
    std::ifstream fileStream( fileName, std::ios::in );

    GUARANTEE_OR_DIE( fileStream.is_open(), Stringf("FileUtils - Failed to open file %s", fileName.c_str() ) );

    std::string fileLine;
    while( std::getline( fileStream, fileLine) )
    {
        if( !fileLine.empty() )
        {
            fileData.push_back( fileLine );
        }
    }

    fileStream.close();
}

void* FileReadToNewBuffer( const std::string& fileName, OUT_PARAM size_t* outSize = nullptr )
{
    FILE* file;
    fopen_s( &file, fileName.c_str(), "r" );
    if ( file == nullptr ) { return nullptr; }

    //Get the size
    fseek( file, 0, SEEK_END );
    const long fileSize = ftell( file ) + 1;

    unsigned char* buffer = new unsigned char[ fileSize ];
    size_t bytesRead = 0;
    if ( buffer != nullptr )
    {
        fseek( file, 0, SEEK_SET );
        bytesRead = fread( buffer, 1, fileSize, file );
        buffer[ bytesRead ] = '\0';
    }

    if ( outSize != nullptr )
    {
        *outSize = bytesRead;
    }

    fclose( file );

    return buffer;
}
