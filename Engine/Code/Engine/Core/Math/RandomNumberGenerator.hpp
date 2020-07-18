#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/Noise/RawNoise.hpp"

class RandomNumberGenerator
{
public:
    RandomNumberGenerator() = default;
    RandomNumberGenerator( unsigned int seed );

    //-------------------------------------------------------------------------
    // State Functions
    void Reset( unsigned int seed, int position = 0 );
    unsigned int GetSeed() const { return m_Seed; }
    int GetPosition() const { return m_Position; }
    void SetPosition( int position ) { m_Position = position; }

    //-------------------------------------------------------------------------
    // Random Functions
    bool FiftyFifty();
    bool Chance( float probabiltyOfTrue );

    unsigned char RandomByte();
    char RandomChar();
    unsigned short RandomInt16();
    short RandomShort();
    unsigned int RandomInt32();
    int RandomInt();

    unsigned int IntLestThan( unsigned int lessThan );
    int IntLessThan( int lessThan );
    int IntInRange( int lowerBoundInclusive, 
                    int upperBoundInclusive );

    float FloatZeroToOne();
    float FloatZeroToOneExclusive();
    float FloatLessThan( float lessThan );
    float FloatInRange( float lowerBoundInclusive, 
                        float upperBoundInclusive );

    void RandomDirection( OUT_PARAM float& x, OUT_PARAM float& y );

private:
    unsigned int m_Seed = 0;
    int m_Position = 0;
};