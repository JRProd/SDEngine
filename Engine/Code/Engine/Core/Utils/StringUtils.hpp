#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"

#include <string>
#include <vector>

struct MarkdownInfo
{
    Rgba8 color;
};
struct MarkedString
{
    bool isMarked = false;
    std::string string;
    MarkdownInfo info;

public:
    MarkedString() = default;
    MarkedString( bool marked, const std::string& markedString, const MarkdownInfo& markdown);
};

typedef std::vector<std::string> Strings;
typedef std::vector<MarkedString> MarkedStrings;

//-----------------------------------------------------------------------------
// Formatting escape characters
// %? - All escape characters are prefixed by '%'
//
//  % - The literal '%'
//  c - Single character, converted to unsigned char
//  s - Character string
//d,i - Signed integer
//  o - Unsigned integer, converted to octal representation
//x,X - Unsigned integer, converted to hexadecimal representation
//  u - Unsigned integer
//f,F - Floating point number
//e,E - Floating point number, expressed as decimal exponent notation
//a,A - Floating point number, expressed as hexadecimal exponent notation
//g,G - Floating point number, expressed as decimal or decimal exponent notation
//  n - Returns the number of characters written so far
//  p - Pointer
//
// %??<escape> - Arguments can added before the escape character to specify conversion type
//  hh - HalfHalf, signed char, unsigned char, and signed char*
//   h - Half, short, unsigned short, and short*
//     - None, int, char*, int, unsigned int, double, int*
//   l - Long, wint_t, wchar_t*, long, unsigned long, double, long*
//  ll - LongLong, long long, unsigned long long, long long*
//   L - DoubleLong, long double

//-----------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
const std::string Stringv( const char* format, va_list args );

const std::string ToLowerCase( const std::string& string );

int Stricmp( const std::string& a, const std::string& b );
int Stricmp( const std::string& a, const char* b );
int Stricmp( const char* a, const std::string& b );
int Stricmp( const char* a, const char* b );

char GetFirstNonWhitespaceCharacter( const std::string& str );

void Trim( std::string& str );
void ReplaceAll( std::string& str, char value, char replace );

unsigned char GetBaseFromString( const char* stringToNumeric );
unsigned char GetBaseFromString( const std::string& stringToNumeric );
bool GetNumericValueFromChar( const char& character, OUT_PARAM unsigned char& value );

bool ConvertToNumeric( const char* stringToNumeric, OUT_PARAM unsigned long long& value );
bool ConvertToNumeric( const std::string& stringToNumeric, OUT_PARAM unsigned long long& value );
bool ConvertToNumeric( const char* stringToNumeric, unsigned char base, OUT_PARAM unsigned long long& value );
bool ConvertToNumeric( const std::string& stringToNumeric, unsigned char base, OUT_PARAM unsigned long long& value, bool forceFloatingPoint = false );

bool ConvertToNumeric( const char* stringToNumeric, OUT_PARAM long long& value, bool forceFloatingPoint = false );
bool ConvertToNumeric( const std::string& stringToNumeric, OUT_PARAM long long& value, bool forceFloatingPoint = false );
bool ConvertToNumeric( const char* stringToNumeric, unsigned char base, OUT_PARAM long long& value, bool forceFloatingPoint = false );
bool ConvertToNumeric( const std::string& stringToNumeric, unsigned char base, OUT_PARAM long long& value, bool forceFloatingPoint = false );

unsigned char StringToHex( const char both );
unsigned char StringToHex( const char high, const char low );
Strings SplitStringOnDelimiter( const std::string& string,
                                                 char delimiter );

std::string GetMarkedString( const std::string& markedString );
std::string GetMarkdownSequence( const std::string& markedString, size_t& endOfSequence );
std::string SanitizeForMarkdown( const std::string& unsanitized );
MarkedStrings ParseMarkedString( const std::string& string );


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++                                          Type Functions                                 +++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
std::string GetTypeName( const char* value );
std::string ConvertToString( const char* value );
const char* ConvertFromString( const char* string, const char* defaultValue );
const char* ConvertFromString( const std::string& string, const char* defaultValue );

std::string GetTypeName( const std::string& value );
std::string ConvertToString( const std::string& value );
std::string ConvertFromString( const char* string, const std::string& defaultValue );
std::string ConvertFromString( const std::string& string, const std::string& defaultValue );

std::string ConvertToString( bool value );
std::string GetTypeName( bool value );
bool ConvertFromString( const char* string, bool defaultValue );
bool ConvertFromString( const std::string& string, bool defaultValue );

std::string ConvertToString( unsigned char value );
std::string GetTypeName( unsigned char value );
unsigned char ConvertFromString( const char* string, unsigned char defaultValue );
unsigned char ConvertFromString( const std::string& string, unsigned char defaultValue );

std::string ConvertToString( char value );
std::string GetTypeName( char value );
char ConvertFromString( const char* string, char defaultValue );
char ConvertFromString( const std::string& string, char defaultValue );

std::string ConvertToString( short value);
std::string GetTypeName( short value );
short ConvertFromString( const char* string, short defaultValue );
short ConvertFromString( const std::string& string,  short defaultValue );

std::string ConvertToString( unsigned int value );
std::string GetTypeName( unsigned int value );
unsigned int ConvertFromString( const char* string, unsigned int defaultValue );
unsigned int ConvertFromString( const std::string& string, unsigned int defaultValue );

std::string ConvertToString( int value );
std::string GetTypeName( int value );
int ConvertFromString( const char* string, int defaultValue );
int ConvertFromString( const std::string& string, int defaultValue );

std::string ConvertToString( size_t value );
std::string GetTypeName( size_t value );
size_t ConvertFromString( const char* string, size_t defaultValue );
size_t ConvertFromString( const std::string& string, size_t defaultValue );

std::string ConvertToString( float value );
std::string GetTypeName( float value );
float ConvertFromString( const char* string, float defaultValue );
float ConvertFromString( const std::string& string, float defaultValue );

std::string ConvertToString( double value );
std::string GetTypeName( double value );
double ConvertFromString( const char* string, double defaultValue );
double ConvertFromString( const std::string& string, double defaultValue );