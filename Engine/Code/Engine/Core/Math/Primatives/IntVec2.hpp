#pragma once

#include <string>

// Copy of Vec2 with name changed
// From Prof. Eiserloh

struct Vec2;

/**
    Two Dimensional Integer Vector
*/
struct IntVec2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
    int x = 0;
    int y = 0;

public:
    // Construction/Destruction
    ~IntVec2() {}												// destructor (do nothing)
    IntVec2() {}												// default constructor (do nothing)
    IntVec2( const IntVec2& copyFrom );							// copy constructor (from another vec2)
    explicit IntVec2( int initialX, int initialY );		        // explicit constructor (from x, y)
    explicit IntVec2( const Vec2& copyFrom );

    // Static Methods
    static IntVec2 CreateFromString( const char* data );
    static int GetTaxicabDistance( const IntVec2& posA, const IntVec2& posB );
    
    // Static Vec2s
    static IntVec2 ZERO;
    static IntVec2 ONE;
    static IntVec2 UNIT_EAST;
    static IntVec2 UNIT_NORTH;
    static IntVec2 UNIT_WEST;
    static IntVec2 UNIT_SOUTH;

    // Accessors (const methods)
    float		    GetLength() const;
    int             GetLengthSquared() const;
    int             GetTaxiCabLength() const;
    float           GetAngleDegrees() const;
    float           GetAngleRadians() const;
    const IntVec2	GetRotated90Degrees() const;
    const IntVec2	GetRotatedMinus90Degrees() const;

    // Modifiers (non-const methods)
    void		Rotate90Degrees();
    void		RotateMinus90Degrees();

    // Operators (const)
    bool		    operator==( const IntVec2& compare ) const;		    // IntVec2 == IntVec2
    bool		    operator!=( const IntVec2& compare ) const;		    // IntVec2 != IntVec2
    const IntVec2	operator+( const IntVec2& vecToAdd ) const;		    // IntVec2 + IntVec2
    const IntVec2	operator-( const IntVec2& vecToSubtract ) const;	// IntVec2 - IntVec2
    const IntVec2	operator-() const;								    // -IntVec2, i.e. "unary negation"
    const IntVec2	operator*( int uniformScale ) const;			    // IntVec2 * int
    const IntVec2	operator*( const IntVec2& vecToMultiply ) const;	// IntVec2 * IntVec2
    const IntVec2	operator/( int inverseScale ) const;			    // IntVec2 / int

    // Operators (self-mutating / non-const)
    void		    operator+=( const IntVec2& vecToAdd );			    // vec2 += vec2
    void		    operator-=( const IntVec2& vecToSubtract );		    // vec2 -= vec2
    void		    operator*=( int uniformScale );			            // vec2 *= float
    void		    operator/=( int uniformDivisor );		            // vec2 /= float
    void		    operator=( const IntVec2& copyFrom );			    // vec2 = vec2

    const std::string ToString() const;
    
    // Standalone "friend" functions that are conceptually, but not actually, part of Vec2::
    friend const IntVec2 operator*( int uniformScale, const IntVec2& vecToScale );	// float * vec2
};

std::string ConvertToString( const IntVec2& value );
std::string GetTypeName( const IntVec2& value );
IntVec2 ConvertFromString( const char* string, const IntVec2& defaultValue );
IntVec2 ConvertFromString( const std::string& string, const IntVec2& defaultValue );

