#include "RandomNumberGenerator.hpp"

//-----------------------------------------------------------------------------
RandomNumberGenerator::RandomNumberGenerator( unsigned int seed )
    : m_Seed( seed )
    , m_Position( 0 )
{
}

//-----------------------------------------------------------------------------
void RandomNumberGenerator::Reset( unsigned int seed, int position )
{
    m_Seed = seed;
    m_Position = position;
}

//-----------------------------------------------------------------------------
bool RandomNumberGenerator::FiftyFifty()
{
    return RawNoise::GetNoiseZeroToOne( m_Position++, m_Seed ) < .5;
}

//-----------------------------------------------------------------------------
bool RandomNumberGenerator::Chance( float probabiltyOfTrue )
{
    return RawNoise::GetNoiseZeroToOne( m_Position++, m_Seed ) <= probabiltyOfTrue;
}

//-----------------------------------------------------------------------------
unsigned char RandomNumberGenerator::RandomByte()
{
    return static_cast<unsigned char>(RawNoise::GetNoiseUint( m_Position++, m_Seed ));
}

//-----------------------------------------------------------------------------
char RandomNumberGenerator::RandomChar()
{
    return static_cast<char>(RawNoise::GetNoiseUint( m_Position++, m_Seed ));
}

//-----------------------------------------------------------------------------
unsigned short RandomNumberGenerator::RandomInt16()
{
    return static_cast<unsigned short>(RawNoise::GetNoiseUint( m_Position++, m_Seed ));
}

//-----------------------------------------------------------------------------
short RandomNumberGenerator::RandomShort()
{
    return static_cast<short>(RawNoise::GetNoiseUint( m_Position++, m_Seed ));
}

//-----------------------------------------------------------------------------
unsigned int RandomNumberGenerator::RandomInt32()
{
    return RawNoise::GetNoiseUint( m_Position++, m_Seed );
}

//-----------------------------------------------------------------------------
int RandomNumberGenerator::RandomInt()
{
    return static_cast<int>(RawNoise::GetNoiseUint( m_Position++, m_Seed ));
}

//-----------------------------------------------------------------------------
unsigned int RandomNumberGenerator::IntLestThan( unsigned int lessThan )
{
    return RawNoise::GetNoiseUint( m_Position++, m_Seed ) % lessThan;
}

//-----------------------------------------------------------------------------
int RandomNumberGenerator::IntLessThan( int lessThan )
{
    return static_cast<int>(RawNoise::GetNoiseUint( m_Position++, m_Seed ) % lessThan);
}

//-----------------------------------------------------------------------------
int RandomNumberGenerator::IntInRange( int lowerBoundInclusive,
                                       int upperBoundInclusive )
{
    return lowerBoundInclusive + IntLessThan( upperBoundInclusive - lowerBoundInclusive + 1 );
}

//-----------------------------------------------------------------------------
float RandomNumberGenerator::FloatZeroToOne()
{
    return RawNoise::GetNoiseZeroToOne( m_Position++, m_Seed );
}

//-----------------------------------------------------------------------------
float RandomNumberGenerator::FloatZeroToOneExclusive()
{
    constexpr double RATIO = 1.0 / (static_cast<double>(0xFFFFFFFF) + 1);
    double exclusive = static_cast<double>(RawNoise::GetNoiseUint( m_Position++, 
                                                                   m_Seed ));
    return static_cast<float>(exclusive * RATIO);
}

//-----------------------------------------------------------------------------
float RandomNumberGenerator::FloatLessThan( float lessThan )
{
    return RawNoise::GetNoiseZeroToOne( m_Position++, m_Seed ) * lessThan;
}

//-----------------------------------------------------------------------------
float RandomNumberGenerator::FloatInRange( float lowerBoundInclusive,
                                           float upperBoundInclusive )
{
    float zeroToOne = FloatZeroToOne();
    return lowerBoundInclusive + zeroToOne * (upperBoundInclusive - lowerBoundInclusive);
}

//-----------------------------------------------------------------------------
void RandomNumberGenerator::RandomDirection( OUT_PARAM float& x,
                                             OUT_PARAM float& y )
{
    // Get random angle
    float radians = FloatLessThan( g_PIf * 2.f );
    x = cosf( radians );
    y = sinf( radians );
}
