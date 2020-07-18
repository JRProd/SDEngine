#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Core/Math/Range/FloatRange.hpp"

struct Disc;
class RandomNumberGenerator;

struct AABB2
{
    Vec2 mins;
    Vec2 maxs;

public:
    AABB2() = default;
    ~AABB2() = default;
    AABB2( const AABB2& copy ) = default;
    
    explicit AABB2( float minX, float minY, float maxX, float maxY );
    explicit AABB2( const Vec2& min, const Vec2& max );
    explicit AABB2( const IntVec2& min, const IntVec2& max );

    static const AABB2 UNIT_BOX;
    static const AABB2 UNIT_AROUND_ZERO;

    static const AABB2 MakeFromAspect( float aspectRatio );
    static const AABB2 MakeFromUnitBoxAround( const Vec2& bottomLeft );
    static const AABB2 MakeFromCenterBoxAround( const Vec2& center );
    static const AABB2 CreateFromString( const char* data );

    //-------------------------------------------------------------------------
    // Accessors (const)
    const Vec2 GetCenter() const;
    const Vec2 GetDimensions() const;
    const Vec2 GetTopLeft() const;
    const Vec2 GetBotRight() const;
    void GetCornerPoints(Vec2* output) const; // Top Right, Top Left, Bottom Left, Bottom Right
    const AABB2 GetTranslation( const Vec2& translation ) const;
    const AABB2 GetScaled( const Vec2& scale ) const;

    Vec2 GetPointAtUV( const Vec2& uvPoint ) const;
    Vec2 GetUVAtPoint( const Vec2& point ) const;

    // Bounded Boxes
    const AABB2 GetBoxFromRight( float percent, float extra = 0.f ) const;
    const AABB2 GetBoxFromTop( float percent, float extra = 0.f ) const;
    const AABB2 GetBoxFromLeft( float percent, float extra = 0.f ) const;
    const AABB2 GetBoxFromBottom( float percent, float extra = 0.f ) const;

    //-------------------------------------------------------------------------
    // Mutators (non-const)
    void SetCenter( const Vec2& newCenter );
    void SetDimensions( const Vec2& newDimension );
    void Translate( const Vec2& translation );
    void TranslateCenter( const Vec2& translation );
    void ScaleFromMin( float uniformScale );
    void ScaleFromMin( const Vec2& scale );
    void ScaleFromMax( float uniformScale );
    void ScaleFromMax( const Vec2& scale );
    void ExpandToFitPoint( const Vec2& point );
    void FitWithinBounds( const AABB2& bounds );
    void ScaleToFitWithinBounds( const AABB2& bounds, const Vec2& alignment = Vec2::ALIGN_CENTERED );
    void AlignWithinBounds( const AABB2& bounds, const Vec2& alignment = Vec2::ALIGN_CENTERED );

    // Bounding Boxes
    const AABB2 CarveBoxFromRight( float percent, float extra = 0.f );
    const AABB2 CarveBoxFromTop( float percent, float extra = 0.f );
    const AABB2 CarveBoxFromLeft( float percent, float extra = 0.f );
    const AABB2 CarveBoxFromBottom( float percent, float extra = 0.f );

    //-------------------------------------------------------------------------
    // Operators (const)
    bool		operator==( const AABB2& compare ) const;		// AABB2 == AABB2
    bool		operator!=( const AABB2& compare ) const;		// AABB2 != AABB2

    //-------------------------------------------------------------------------
    // Operators (self-mutating / non-const)
    void        operator*=( float uniformScale );               // Scale around center uniformly
    void        operator*=( const Vec2& scale );                // Scale around center
    AABB2&		operator=( const AABB2& copyFrom ); 			// AABB2 = AABB2
    
    //-------------------------------------------------------------------------
    // Collisions
    const Disc GetInnerDisc() const;
    const Disc GetOuterDisc() const;
    bool IsPointInside( const Vec2& point ) const;
    Vec2 GetNearestPointInAABB2( const Vec2& from ) const;
    bool CheckSimpleSeparationAgainstPoints( int numPoints, Vec2* points, float radius = 0.f ) const;
    bool GetRayCrossRange( OUT_PARAM FloatRange& result, const Vec2& start, const Vec2& end ) const;

    //-------------------------------------------------------------------------
    // Utilities
    const Vec2 RollRandomPointInside( RandomNumberGenerator& rng ) const;

private:
    void ExpandXToFit( float x );
    void ExpandYToFit( float y );
};

std::string ConvertToString( const AABB2& value );
std::string GetTypeName( const AABB2& value );
AABB2 ConvertFromString( const char* string, const AABB2& defaultValue );
AABB2 ConvertFromString( const std::string& string, const AABB2& defaultValue );
