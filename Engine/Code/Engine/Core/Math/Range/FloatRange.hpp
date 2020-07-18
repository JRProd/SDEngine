#pragma once

#include <string>

struct Vec2;
class RandomNumberGenerator;

struct FloatRange
{
    float minimum = 0.f;
    float maximum = 1.f;

public:
    FloatRange() = default;
    explicit FloatRange( float minAndMax );
    explicit FloatRange( float min, float max );

    static FloatRange CreateFromString( const char* data );
    static FloatRange CreateFromPointsOnAxis( int numPoints, 
                                              const Vec2* points, 
                                              const Vec2& relativePoint, 
                                              const Vec2& normalizedAxis, 
                                              float radius = 0.f );

    // Accessors (const)
    bool IsInRange( float testPoint ) const;
    bool DoesOverlap( const FloatRange& otherRange ) const;
    float GetRandomInRange( RandomNumberGenerator & rng ) const;

    // Mutators (non-const)
    void Set( float min, float max );
    void ClipTo( const FloatRange& otherRange );

    const std::string ToString() const;

    void Fix();
};