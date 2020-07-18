#pragma once

#include "Engine/Core/Math/Primatives/Vec2.hpp"

class AnalogJoystick
{
    friend class XboxController;

public:
    const float m_InnerDeadzoneFraction;
    const float m_OuterDeadzoneFraction;

    explicit AnalogJoystick( float innerDeadzone, float outerDeadzone );

    Vec2 GetPosition() const { return m_CorrectedPosition; }
    float GetMagnitude() const { return m_CorrectedMagnitude; }
    float GetAngleDegrees() const { return m_CorrectedAngleDegrees; }

    Vec2 GetRawPosition() const { return m_RawPosition; }

    void Reset();

private:
    Vec2 m_RawPosition = Vec2( 0.f, 0.f );
    Vec2 m_CorrectedPosition = Vec2( 0.f, 0.f );
    float m_CorrectedAngleDegrees = 0.f;
    float m_CorrectedMagnitude = 0.f;


    void UpdatePosition( float rawNormalizedX, float rawNormalizedY );
};