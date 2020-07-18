#include "AnalogJoystick.hpp"

#include "Engine/Core/Math/MathUtils.hpp"

AnalogJoystick::AnalogJoystick( float innerDeadzone, float outerDeadzone )
    : m_InnerDeadzoneFraction(innerDeadzone)
    , m_OuterDeadzoneFraction(outerDeadzone)
{
}

void AnalogJoystick::Reset()
{
    m_RawPosition = Vec2( 0.f, 0.f );
    m_CorrectedPosition = Vec2( 0.f, 0.f );
    m_CorrectedAngleDegrees = 0.f;
    m_CorrectedMagnitude = 0.f;
}

void AnalogJoystick::UpdatePosition( float rawNormalizedX, 
                                     float rawNormalizedY )
{
    m_RawPosition = Vec2( rawNormalizedX, rawNormalizedY );

    float rawR = sqrtf( rawNormalizedX * rawNormalizedX + rawNormalizedY * rawNormalizedY );
    float rawThetaDegree = atan2fDegrees( rawNormalizedY, rawNormalizedX );

    float unclampedCorrectedR = RangeMapFloat( m_InnerDeadzoneFraction, 
                                               m_OuterDeadzoneFraction, 
                                               0.f, 
                                               1.f, 
                                               rawR );
    m_CorrectedMagnitude = ClampZeroToOne( unclampedCorrectedR );

    if ( m_CorrectedMagnitude > 0.f )
    {
        m_CorrectedAngleDegrees = rawThetaDegree;
    }
    else
    {
        m_CorrectedMagnitude = 0.f;
    }
    
    m_CorrectedPosition = Vec2::MakeFromPolarDegrees( m_CorrectedAngleDegrees, 
                                                      m_CorrectedMagnitude );

}
