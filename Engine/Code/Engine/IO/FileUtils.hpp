#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"

#include <string>

Strings GetAllFilesNamesInFolder( const char* folderPath, const char* filePattern );
Strings GetAllFilesNamesInFolder( const char* folderPath, const std::string& filePattern );
Strings GetAllFilesNamesInFolder( const std::string& folderPath, const char* filePattern );
Strings GetAllFilesNamesInFolder( const std::string& folderPath, const std::string& filePattern );

std::string TrimFileExtension( const std::string& filePath );

void FileReadToVector( const std::string& fileName, OUT_PARAM Strings& fileData );

void* FileReadToNewBuffer( const std::string& fileName, OUT_PARAM size_t* outSize );
