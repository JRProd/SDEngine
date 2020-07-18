#include "StringUtils.hpp"

#include <cstdarg>
#include <cstring>
#include <limits>

#include "Engine/Console/Console.hpp"
#include "Engine/Core/EngineCommon.hpp"

//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
    char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
    va_list variableArgumentList;
    va_start( variableArgumentList, format );
    vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );
    va_end( variableArgumentList );
    textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

    return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
    char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
    char* textLiteral = textLiteralSmall;
    if ( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
        textLiteral = new char[ maxLength ];

    va_list variableArgumentList;
    va_start( variableArgumentList, format );
    vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );
    va_end( variableArgumentList );
    textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

    std::string returnValue( textLiteral );
    if ( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
        delete[] textLiteral;

    return returnValue;
}

const std::string Stringv( char const* format, va_list args )
{
    char buffer[ 1024 ];
    vsnprintf_s( buffer, 1024, format, args );
    return buffer;
}

const std::string ToLowerCase( const std::string& string )
{
    std::string lowerCase;
    for ( char c : string )
    {
        lowerCase += static_cast<char>(std::tolower( static_cast<int>(c) ));
    }

    return lowerCase;
}

int Stricmp( const std::string& a, const std::string& b )
{
    return Stricmp( a.c_str(), b.c_str() );
}

int Stricmp( const std::string& a, const char* b )
{
    return Stricmp( a.c_str(), b );
}

int Stricmp( const char* a, const std::string& b )
{
    return Stricmp( a, b.c_str() );
}

int Stricmp( const char* a, const char* b )
{
    return _stricmp( a, b );
}

char GetFirstNonWhitespaceCharacter( const std::string& str )
{
    for( const char& c : str )
    {
        if( c != ' ' && c != '\t' && c != '\n' && c != '\v' && c != '\f' && c != '\r' )
        {
            return c;
        }
    }

    return 0;
}

void Trim( std::string& str )
{
    size_t lastLeftSpace = 0;
    for ( size_t leftIndex = 0; leftIndex < str.size(); ++leftIndex )
    {
        if ( std::isspace( str.at( leftIndex ) ) )
        {
            lastLeftSpace++;
        }
        else
        {
            break;
        }
    }
    str.erase( 0, lastLeftSpace );

    size_t lastRightSpace = str.size() - 1;
    for ( int rightIndex = static_cast<int>(lastRightSpace); rightIndex >= 0; --rightIndex )
    {
        if ( std::isspace( str.at( rightIndex ) ) )
        {
            lastRightSpace--;
        }
        else
        {
            break;
        }
    }
    str.erase( lastRightSpace + 1 );
}

void ReplaceAll( std::string& str, const char value, const char replace )
{
    for( char& c : str )
    {
        if( c == value )
        {
            c = replace;
        }
    }
}

unsigned char GetBaseFromString( const char* stringToNumeric )
{
    return GetBaseFromString( std::string(stringToNumeric) );
}

unsigned char GetBaseFromString( const std::string& stringToNumeric )
{
    // The negative sign must always be first
    size_t offset = 0;
    if( stringToNumeric.at( 0 ) == '-' )
    {
        offset = 1;
    }

    if ( stringToNumeric.size() > 1 + offset && stringToNumeric.at( 0 + offset ) == '0' )
    {
        const char& baseChar = stringToNumeric.at( 1 + offset );
        if ( baseChar == 'b' )
        {
            return 2;
        }

        if ( baseChar == 'x' )
        {
            return 16;
        }

        // If the zero is simply there to start a float consider it base 10
        if( baseChar == '.' )
        {
            return 10;
        }

        return 8;
    }

    return 10;
}

bool GetNumericValueFromChar( const char& character, OUT_PARAM unsigned char& value )
{
    if( character >= '0' && character <= '9')
    {
        value = character & 0x0f;                // 0-9 is represented in the last 4 bits of the character
        return true;
    }
    if ( character >= 'A' && character <= 'F' || character >= 'a' && character <= 'f' )
    {
        value = 9 + (character & 0x1f);          // A-F and a-f is identical in the last 5 bits of the character
        return true;
    }
    return false;
}

static bool ConvertStringToNumericFloatingPointWithBase( const std::string& number, unsigned char base, unsigned long long& value)
{
    long double floatingPointValue = 0;
    bool isNegative = false;
    bool isPastPoint = false;
    long double floatingPointMultiplier = 0;
    const long double castBase = static_cast<long double>(base);

    for( const char& numeral : number )
    {
        if( numeral == '-' )
        {
            if( floatingPointValue == 0 )
            {
                isNegative = true;
                continue;
            }
            else
            {
                return false;
            }
        }

        if( numeral == '.' )
        {
            // Double . found in string
            if( isPastPoint )
            {
                return false;
            }

            isPastPoint = true;
            floatingPointMultiplier = 1.f / castBase;
            continue;
        }

        unsigned char numericValue;
        const bool success = GetNumericValueFromChar( numeral, numericValue );
        if( success )
        {
            const long double castValue = static_cast<long double>(numericValue) *(isNegative ? -1 : 1);
            if( isPastPoint )
            {

                floatingPointValue += castValue * floatingPointMultiplier;
                floatingPointMultiplier /= castBase;
            }
            else
            {
                floatingPointValue *= castBase;
                floatingPointValue += castValue;
            }
        }
        else
        {
            return false;
        }
    }

    // Convert long double to a unsigned long long literally byte by byte
    value = *(unsigned long long*)&floatingPointValue;
    return true;
}

static bool ConvertStringToNumericWithBase( const std::string& number, unsigned char base, unsigned long long& value, const bool forceFloatingPoint )
{
    GUARANTEE_OR_DIE( base <= 16, Stringf( "ConvertToNumeric: Cannot use base %u. Base must be [2,16]", base ) );

    std::string trimmed = number;
    Trim( trimmed );

    // Check if the number is a floating point
    const size_t isFloatingPoint = number.find_first_of( '.' );
    if( forceFloatingPoint || isFloatingPoint != std::string::npos )
    {
        return ConvertStringToNumericFloatingPointWithBase( number, base, value );
    }

    bool isNegative = false;

    value = 0;
    for( const char& numeral : number )
    {
        if ( numeral == '-' )
        {
            if ( value == 0 )
            {
                isNegative = true;
                continue;
            }
            else
            {
                return false;
            }
        }

        value *= base;

        unsigned char numericValue;
        const bool success = GetNumericValueFromChar( numeral, numericValue );
        if ( success )
        {
            // Cannot have a number larger than the base
            if ( numericValue >= base )
            {
                return false;
            }

            value += numericValue;
        }
        else
        {
            return false;
        }
    }

    // Convert to twos complement if zero
    if( isNegative )
    {
        value = ~0ull ^ value;
        value += 1ull;
    }

    return true;
}

static std::string GetPureNumberFromBase( const std::string& string, const unsigned char base)
{
    std::string pureNumber;
    size_t offset = 0;

    if( string.at( 0 )  == '-' )
    {
        pureNumber = "-";
        offset = 1;
    }

    if ( base == 10 )
    {
        pureNumber += string;
    }
    else
    {
        if ( base == 8 )
        {
            pureNumber += string.substr( 1 + offset );
        }
        else
        {
            pureNumber += string.substr( 2 + offset );
        }
    }

    return pureNumber;
}

bool ConvertToNumeric( const char* stringToNumeric, OUT_PARAM unsigned long long& value )
{
    return ConvertToNumeric( std::string(stringToNumeric), value );
}

bool ConvertToNumeric( const std::string& stringToNumeric, OUT_PARAM unsigned long long& value )
{
    const unsigned char base = GetBaseFromString( stringToNumeric );
    const std::string pureNumber = GetPureNumberFromBase( stringToNumeric, base );

    // If after removal of base, string is empty, cannot parse
    if ( pureNumber.empty() ) { return  false; }

    return ConvertToNumeric( pureNumber, base, value );
}

bool ConvertToNumeric( const char* stringToNumeric, unsigned char base, OUT_PARAM unsigned long long& value )
{
    return ConvertToNumeric( std::string(stringToNumeric), base, value );
}

bool ConvertToNumeric( const std::string& stringToNumeric, unsigned char base, OUT_PARAM unsigned long long& value, const bool forceFloatingPoint )
{
    return ConvertStringToNumericWithBase( stringToNumeric, base, value, forceFloatingPoint );
}

bool ConvertToNumeric( const char* stringToNumeric, OUT_PARAM long long& value, const bool forceFloatingPoint )
{
    return ConvertToNumeric( std::string(stringToNumeric), value, forceFloatingPoint );
}

bool ConvertToNumeric( const std::string& stringToNumeric, OUT_PARAM long long& value, const bool forceFloatingPoint )
{
    const unsigned char base = GetBaseFromString( stringToNumeric );
    const std::string pureNumber = GetPureNumberFromBase( stringToNumeric, base );

    // If after removal of base, string is empty, cannot parse
    if ( pureNumber.empty() ) { return  false; }

    return ConvertToNumeric( pureNumber, base, value, forceFloatingPoint );
}
bool ConvertToNumeric( const char* stringToNumeric, unsigned char base, OUT_PARAM long long& value, const bool forceFloatingPoint )
{
    return ConvertToNumeric( std::string(stringToNumeric), base, value, forceFloatingPoint );
}

bool ConvertToNumeric( const std::string& stringToNumeric, unsigned char base, OUT_PARAM long long& value, const bool forceFloatingPoint )
{
    unsigned long long unsignedValue;
    const bool result = ConvertToNumeric( stringToNumeric, base, unsignedValue, forceFloatingPoint );

    if( result )
    {
        value = static_cast<long long>(unsignedValue);
    }

    return result;
}

unsigned char StringToHex( const char both )
{
    return StringToHex( both, both );
}

unsigned char StringToHex( const char high, const char low )
{
    unsigned char c = 0;
    switch ( high )
    {
        case '0': c |= (0 << 4); break;
        case '1': c |= (1 << 4); break;
        case '2': c |= (2 << 4); break;
        case '3': c |= (3 << 4); break;
        case '4': c |= (4 << 4); break;
        case '5': c |= (5 << 4); break;
        case '6': c |= (6 << 4); break;
        case '7': c |= (7 << 4); break;
        case '8': c |= (8 << 4); break;
        case '9': c |= (9 << 4); break;
        case 'a': c |= (10 << 4); break;
        case 'b': c |= (11 << 4); break;
        case 'c': c |= (12 << 4); break;
        case 'd': c |= (13 << 4); break;
        case 'e': c |= (14 << 4); break;
        case 'f': c |= (15 << 4); break;
        case 'A': c |= (10 << 4); break;
        case 'B': c |= (11 << 4); break;
        case 'C': c |= (12 << 4); break;
        case 'D': c |= (13 << 4); break;
        case 'E': c |= (14 << 4); break;
        case 'F': c |= (15 << 4); break;
        default:
            ERROR_RECOVERABLE( "Unknown Hex sequence: Using 0b00000000" );
            c |= (0 << 8);
            break;
    }
    switch ( low )
    {
        case '0': c |= (0); break;
        case '1': c |= (1); break;
        case '2': c |= (2); break;
        case '3': c |= (3); break;
        case '4': c |= (4); break;
        case '5': c |= (5); break;
        case '6': c |= (6); break;
        case '7': c |= (7); break;
        case '8': c |= (8); break;
        case '9': c |= (9); break;
        case 'a': c |= (10); break;
        case 'b': c |= (11); break;
        case 'c': c |= (12); break;
        case 'd': c |= (13); break;
        case 'e': c |= (14); break;
        case 'f': c |= (15); break;
        case 'A': c |= (10); break;
        case 'B': c |= (11); break;
        case 'C': c |= (12); break;
        case 'D': c |= (13); break;
        case 'E': c |= (14); break;
        case 'F': c |= (15); break;
        default:
            ERROR_RECOVERABLE( "Unknown Hex sequence: Using 0b00000000" );
            c |= (0 << 8);
            break;
    }

    return c;
}

Strings SplitStringOnDelimiter( const std::string& string,
                                char delimiter )
{
    Strings split;

    size_t afterLastDelimiterIndex = 0;
    for ( size_t charIndex = 0; charIndex < string.size(); ++charIndex )
    {
        const char& currentChar = string.at( charIndex );
        if ( currentChar == delimiter )
        {
            const size_t count = charIndex - afterLastDelimiterIndex;
            if( count > 0 )
            {
                const std::string splitString = string.substr( afterLastDelimiterIndex,
                                                         count);
                split.push_back( splitString );
            }

            afterLastDelimiterIndex = charIndex + 1;
        }
    }

    // There is one last string on the end
    const size_t lastIndex = string.size();
    const size_t count = lastIndex - afterLastDelimiterIndex;
    if( count > 0 )
    {
        const std::string lastEntry = string.substr( afterLastDelimiterIndex,
                                               lastIndex - afterLastDelimiterIndex );
        split.push_back( lastEntry );
    }

    return split;
}

static MarkdownInfo ParseMarkedSequence( const std::string& string )
{
    MarkdownInfo info;
    Strings split = SplitStringOnDelimiter( string, ' ' );

    for( const std::string& s : split )
    {
        if( s.at(0) == 'c' )
        {
            info.color = Rgba8::CreateFromString( s.substr( 1 ).c_str() );
        }
    }

    return info;
}

std::string GetMarkedString( const std::string& markedString )
{
    std::string buildString;
    size_t sequenceStart = 0;
    bool escapeNext = false;

    for ( size_t sequenceIndex = 0; sequenceIndex < markedString.size(); ++sequenceIndex )
    {
        if ( markedString.at( sequenceIndex ) == ']' && !escapeNext )
        {
            const size_t count = sequenceIndex - sequenceStart;
            buildString += markedString.substr( sequenceStart, count );
            return  buildString;
        }

        if ( markedString.at( sequenceIndex ) == '\\' )
        {
            escapeNext = true;

            if ( sequenceIndex > 0 )
            {
                const size_t count = sequenceIndex - sequenceStart;
                buildString += markedString.substr( sequenceStart, count );
                sequenceStart = sequenceIndex + 1;
            }
            continue;
        }

        escapeNext = false;
    }

    ERROR_AND_DIE( "MarkdownSequence has no closing bracket" );
}

std::string GetMarkdownSequence( const std::string& markedString, size_t& endOfSequence )
{
    GUARANTEE_OR_DIE( markedString.at( 0 ) == '[', "MarkdownSequence must begin with a bracket" );

    std::string builtSequence;
    size_t sequenceStart = 1;
    bool escapeNext = false;
    for( size_t sequenceIndex = 1; sequenceIndex < markedString.size(); ++sequenceIndex)
    {
        if( markedString.at(sequenceIndex) == '[' && !escapeNext )
        {
            sequenceStart = sequenceIndex + 1;
        }

        if( markedString.at(sequenceIndex) == ']' && !escapeNext )
        {
            const size_t count = sequenceIndex - sequenceStart;
            builtSequence += markedString.substr( sequenceStart, count );
            endOfSequence = sequenceIndex + 1;
            return  builtSequence;
        }

        if ( markedString.at(sequenceIndex) == '\\' )
        {
            escapeNext = true;

            if( sequenceIndex > 1 )
            {
                const size_t count = sequenceIndex - sequenceStart;
                builtSequence += markedString.substr( sequenceStart, count );
                sequenceStart = sequenceIndex + 1;
            }
            continue;
        }

        escapeNext = false;
    }

    ERROR_AND_DIE( "MarkdownSequence has no closing bracket" );
}

std::string SanitizeForMarkdown( const std::string& unsanitized )
{
    std::string sanitized;
    for( const char& c : unsanitized )
    {
        if( c == '[' || c == ']' )
        {
            sanitized.push_back( '\\' );
        }
        sanitized.push_back( c );
    }
    return sanitized;
}

MarkedStrings ParseMarkedString( const std::string& string )
{
    MarkedStrings markedStrings;

    std::string builtSequence;
    size_t sequenceStart = 0;
    bool escapeNext = false;
    for ( size_t charIndex = 0; charIndex < string.size(); ++charIndex )
    {
        if ( string.at( charIndex ) == '[' && !escapeNext )
        {
            size_t count = charIndex - sequenceStart;
            if( count > 0 )
            {
                builtSequence += string.substr( sequenceStart, count );
                markedStrings.emplace_back( false, builtSequence, MarkdownInfo() );
                builtSequence.clear();
            }
            sequenceStart = charIndex;

            // Search for marked sequence;
            int numberBracketsFound = 0;
            for( ; charIndex < string.size(); ++charIndex )
            {
                if( string.at(charIndex)  == ']' && !escapeNext )
                {
                    numberBracketsFound++;
                    if( numberBracketsFound < 2 )
                    {
                        continue;
                    }

                    // Take into account the final bracket for this sequence
                    charIndex += 1;
                    count = charIndex - sequenceStart + 1;
                    std::string fullMarkedSequence = string.substr( sequenceStart, count );

                    size_t sequenceEnd;
                    MarkdownInfo markInfo = ParseMarkedSequence( GetMarkdownSequence( fullMarkedSequence, sequenceEnd ) );
                    std::string markedString = GetMarkedString( fullMarkedSequence.substr( sequenceEnd ) );
                    markedStrings.emplace_back( true, markedString, markInfo );
                    sequenceStart = charIndex;
                    break;
                }

                // Handles the escape character
                if( string.at(charIndex) == '\\' )
                {
                    escapeNext = true;
                    continue;
                }

                escapeNext = false;
            }

            // If the function did not find two un escaped brackets before parsing the string fully
            if( numberBracketsFound < 2 )
            {
                ERROR_AND_DIE( "ParseMarkedSequence has no final ending bracket" );
            }

            // If the function fully parsed the string with the final sequence
            if( charIndex >= string.size() )
            {
                break;
            }

            sequenceStart = charIndex;
        }

        // Handles the escape character
        if ( string.at( charIndex ) == '\\' )
        {
            escapeNext = true;

            if ( charIndex > 1 )
            {
                // Removes the escape character
                const size_t count = charIndex - sequenceStart;
                builtSequence += string.substr( sequenceStart, count );
                sequenceStart = charIndex + 1;
            }
            continue;
        }

        escapeNext = false;
    }

    // Get the last potential sequence
    if( sequenceStart < string.size() - 1)
    {
        builtSequence += string.substr( sequenceStart );
        markedStrings.emplace_back( false, builtSequence, MarkdownInfo() );
    }

    return markedStrings;
}

template <typename Value>
Value UnsignedPrimitiveConvertFromString( const std::string& string, const Value defaultValue )
{
    unsigned long long value;
    const bool validReturn = ConvertToNumeric( string, value );
    if ( validReturn )
    {
        if ( value >= std::numeric_limits<Value>::lowest() &&
             value <= std::numeric_limits<Value>::max() )
        {
            return static_cast<Value>(value);
        }
        return defaultValue;
    }

    return defaultValue;
}

template <typename Value>
Value PrimitiveConvertFromString( const std::string& string, const Value defaultValue )
{
    long long value;
    const bool isFloatingPointType = std::is_floating_point<Value>::value;

    const bool validReturn = ConvertToNumeric( string, value, isFloatingPointType );
    if ( validReturn )
    {
        if( isFloatingPointType )
        {
            // Convert long long to a long double literally byte by byte
            long double floatingPointValue = *(long double*) &value;
            if ( floatingPointValue >= std::numeric_limits<Value>::lowest() &&
                 floatingPointValue <= std::numeric_limits<Value>::max() )
            {
                return static_cast<Value>(floatingPointValue);
            }

            return defaultValue;
        }

        if ( value >= std::numeric_limits<Value>::lowest() &&
             value <= std::numeric_limits<Value>::max() )
        {
            return static_cast<Value>(value);
        }

        return defaultValue;
    }

    return defaultValue;
}

std::string ConvertToString( bool value )
{
    return value ? "True" : "False";
}

std::string GetTypeName( bool value )
{
    UNUSED( value );

    return std::string();
}

bool ConvertFromString( const char* string, const bool defaultValue )
{
    return ConvertFromString( std::string( string ), defaultValue );
}

bool ConvertFromString( const std::string& string, const bool defaultValue )
{
    bool validReturn;
    if ( string == "1" ||
         Stricmp( string, "t" ) == 0 ||
         Stricmp( string, "true" ) == 0 )
    {
        validReturn = true;
    }
    else if ( string == "0" ||
              Stricmp( string, "f" ) == 0 ||
              Stricmp( string, "false" ) == 0 )
    {
        validReturn = false;
    }
    else
    {
        return defaultValue;
    }

    return  validReturn;
}

std::string GetTypeName( const char* value )
{
    UNUSED( value );

    return std::string("cString");
}

std::string ConvertToString( const char* value )
{
    return std::string( value );
}

const char* ConvertFromString( const char* string, const char* defaultValue )
{
    UNUSED( defaultValue );

    return string;
}

const char* ConvertFromString( const std::string& string, const char* defaultValue )
{
    UNUSED( defaultValue );

    return string.c_str();
}

std::string GetTypeName( const std::string& value )
{
    UNUSED( value );


    return std::string("String");
}

std::string ConvertToString( const std::string& value )
{
    return value;
}

std::string ConvertFromString( const char* string, const std::string& defaultValue )
{
    return ConvertFromString( std::string(string), defaultValue );
}

std::string ConvertFromString( const std::string& string, const std::string& defaultValue )
{
    UNUSED( defaultValue );

    return string;
}

std::string ConvertToString( const unsigned char value )
{
    return std::to_string( value );
}

std::string GetTypeName( unsigned char value )
{
    UNUSED( value );


    return std::string("uChar");
}

unsigned char ConvertFromString( const char* string, const unsigned char defaultValue )
{
    return ConvertFromString( std::string(string), defaultValue );
}

unsigned char ConvertFromString( const std::string& string, const unsigned char defaultValue )
{
    return UnsignedPrimitiveConvertFromString( string, defaultValue );
}

std::string ConvertToString( const char value )
{
    return std::to_string( value );
}

std::string GetTypeName( char value )
{
    UNUSED( value );


    return std::string("Char");
}

char ConvertFromString( const char* string, const char defaultValue )
{
    return ConvertFromString( std::string( string ), defaultValue );
}

char ConvertFromString( const std::string& string, const char defaultValue )
{
    return PrimitiveConvertFromString( string, defaultValue );
}

std::string ConvertToString( short value )
{
    return std::to_string( value );
}

std::string GetTypeName( short value )
{
    UNUSED( value );


    return std::string("Short");
}

short ConvertFromString( const char* string, const short defaultValue )
{
    return ConvertFromString( std::string( string ), defaultValue );
}

short ConvertFromString( const std::string& string, const short defaultValue )
{
    return PrimitiveConvertFromString( string, defaultValue );
}

std::string ConvertToString( unsigned int value )
{
    return std::to_string( value );
}

std::string GetTypeName( unsigned int value )
{
    UNUSED( value );


    return std::string("uInt");
}

unsigned int ConvertFromString( const char* string, const unsigned int defaultValue )
{
    return ConvertFromString( std::string( string ), defaultValue );
}

unsigned int ConvertFromString( const std::string& string, const unsigned int defaultValue )
{
    return UnsignedPrimitiveConvertFromString( string, defaultValue );
}

std::string ConvertToString( int value )
{
    return std::to_string( value );
}

std::string GetTypeName( int value )
{
    UNUSED( value );


    return std::string("Int");
}

int ConvertFromString( const char* string, const int defaultValue )
{
    return ConvertFromString( std::string( string ), defaultValue );
}

int ConvertFromString( const std::string& string, const int defaultValue )
{
    return PrimitiveConvertFromString( string, defaultValue );
}

std::string ConvertToString( size_t value )
{
    return std::to_string(value );
}

std::string GetTypeName( size_t value )
{
    UNUSED( value );

    return std::string("Size_t");
}

size_t ConvertFromString( const char* string, size_t defaultValue )
{
    return ConvertFromString( std::string(string), defaultValue );
}

size_t ConvertFromString( const std::string& string, size_t defaultValue )
{
    return UnsignedPrimitiveConvertFromString( string, defaultValue );
}

std::string ConvertToString( float value )
{
    return std::to_string( value );
}

std::string GetTypeName( float value )
{
    UNUSED( value );


    return std::string("float");
}

float ConvertFromString( const char* string, const float defaultValue )
{
    return ConvertFromString( std::string( string ), defaultValue );
}

float ConvertFromString( const std::string& string, const float defaultValue )
{
    return PrimitiveConvertFromString( string, defaultValue );
}

std::string ConvertToString( double value )
{
    return std::to_string( value );
}

std::string GetTypeName( double value )
{
    UNUSED( value );


    return std::string("double");
}

double ConvertFromString( const char* string, double defaultValue )
{
    return ConvertFromString( std::string( string ), defaultValue );
}

double ConvertFromString( const std::string& string, double defaultValue )
{
    return PrimitiveConvertFromString( string, defaultValue );
}


MarkedString::MarkedString( bool marked, const std::string& markedString, const MarkdownInfo& markdown )
    : isMarked( marked )
    , string(markedString)
    , info( markdown )
{
}
