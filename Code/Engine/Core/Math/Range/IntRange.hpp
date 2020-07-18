#pragma once

#include <string>

class RandomNumberGenerator;

struct IntRange
{
    int minimum = 0;
    int maximum = 1;

public:
    IntRange() = default;
    explicit IntRange( int minAndMax );
    explicit IntRange( int min, int max );
    
    static IntRange CreateFromString( const char* data );

    // Accessors (const)
    bool IsInRange( int testPoint ) const;
    bool DoesOverlap( const IntRange& otherRange ) const;
    int GetRandomInRange( RandomNumberGenerator& rng ) const;

    // Mutators (non-const)
    void Set( int min, int max );

    const std::string ToString() const;
};