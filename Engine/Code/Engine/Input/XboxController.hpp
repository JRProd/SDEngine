#pragma once

#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Input/KeyButtonState.hpp"

typedef int ControllerId;
constexpr ControllerId NO_CONTROLLER = -1;

enum XboxButtonID
{
    XBOX_BUTTON_ID_INVALID = -1,

    XBOX_BUTTON_A,
    XBOX_BUTTON_B,
    XBOX_BUTTON_X,
    XBOX_BUTTON_Y,
    XBOX_BUTTON_BACK,
    XBOX_BUTTON_START,
    XBOX_BUTTON_L_BUMPER,
    XBOX_BUTTON_R_BUMPER,
    XBOX_BUTTON_L_THUMB,
    XBOX_BUTTON_R_THUMB,
    XBOX_BUTTON_D_RIGHT,
    XBOX_BUTTON_D_UP,
    XBOX_BUTTON_D_LEFT,
    XBOX_BUTTON_D_DOWN,

    NUM_XBOX_BUTTONS,

};

class XboxController
{
    friend class InputSystem;

public:
    const ControllerId m_ControllerId = -1;
    bool m_IsConnected = false;
    KeyButtonState m_ButtonStates[ NUM_XBOX_BUTTONS ];
    AnalogJoystick m_LeftJoystick = AnalogJoystick( .275f, .95f );
    AnalogJoystick m_RightJoystick = AnalogJoystick( .275f, .95f );
    float m_LeftTriggerValue = 0.f;
    float m_RightTriggerValue = 0.f;

    explicit XboxController( ControllerId controllerId );
    ~XboxController();

    bool IsConnected() const { return m_IsConnected; };
    const AnalogJoystick& GetLeftJoystick() const { return m_LeftJoystick; };
    const AnalogJoystick& GetRightJoystick() const { return m_RightJoystick; };
    float GetLeftTrigger() const { return m_LeftTriggerValue; };
    float GetRightTrigger() const { return m_RightTriggerValue; };
    bool IsButtonPressed( XboxButtonID xboxButton ) const;
    bool IsButtonJustPressed( XboxButtonID xboxButton ) const;
    bool IsButtonJustReleased( XboxButtonID xboxButton ) const;
    void SetLeftVibration( float vibrationPercentage );
    void SetRightVibration( float vibrationPercentage );


private:
    unsigned short m_LeftVibration = 0;
    unsigned short m_RightVirbration = 0;

    void Update();
    void Reset();
    void UpdateTrigger( float& triggerValue, unsigned char rawValue );
    void UpdateJoystick( AnalogJoystick& joystick, short rawX, short rawY );
    void UpdateButton( XboxButtonID buttonId, 
                       unsigned short buttonFlags, 
                       unsigned short buttonFlag );

};
