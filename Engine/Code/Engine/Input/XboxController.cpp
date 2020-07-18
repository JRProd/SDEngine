#include "XboxController.hpp"

#include "Engine/Core/Math/MathUtils.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput9_1_0")

XboxController::XboxController( ControllerId controllerId )
    : m_ControllerId( controllerId )
{
}

XboxController::~XboxController()
{
}

bool XboxController::IsButtonPressed( XboxButtonID xboxButton ) const
{
    return m_ButtonStates[ xboxButton ].IsPressed();
}

bool XboxController::IsButtonJustPressed( XboxButtonID xboxButton ) const
{
    return m_ButtonStates[ xboxButton ].WasJustPressed();
}

bool XboxController::IsButtonJustReleased( XboxButtonID xboxButton ) const
{
    return m_ButtonStates[ xboxButton ].WasJustReleased();
}

void XboxController::SetLeftVibration( float vibrationPercentage )
{
    m_LeftVibration = static_cast<unsigned short>(Interpolate( 0.f,
                                                               65535.f,
                                                               vibrationPercentage ));
}

void XboxController::SetRightVibration( float vibrationPercentage )
{
    m_RightVirbration = static_cast<unsigned short>(Interpolate( 0.f,
                                                                 65535.f,
                                                                 vibrationPercentage ));
}

void XboxController::Update()
{
    XINPUT_STATE xboxControllerState;
    memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
    DWORD errorStatus = XInputGetState( m_ControllerId, &xboxControllerState );

    XINPUT_VIBRATION vibrationInput;
    memset( &vibrationInput, 0, sizeof( vibrationInput ) );

    if ( errorStatus == ERROR_SUCCESS )
    {
        m_IsConnected = true;
        XINPUT_GAMEPAD& gamepad = xboxControllerState.Gamepad;
        UpdateJoystick( m_LeftJoystick, gamepad.sThumbLX, gamepad.sThumbLY );
        UpdateJoystick( m_RightJoystick, gamepad.sThumbRX, gamepad.sThumbRY );
        UpdateTrigger( m_LeftTriggerValue, gamepad.bLeftTrigger );
        UpdateTrigger( m_RightTriggerValue, gamepad.bRightTrigger );
        UpdateButton( XBOX_BUTTON_A, gamepad.wButtons, XINPUT_GAMEPAD_A );
        UpdateButton( XBOX_BUTTON_B, gamepad.wButtons, XINPUT_GAMEPAD_B );
        UpdateButton( XBOX_BUTTON_X, gamepad.wButtons, XINPUT_GAMEPAD_X );
        UpdateButton( XBOX_BUTTON_Y, gamepad.wButtons, XINPUT_GAMEPAD_Y );
        UpdateButton( XBOX_BUTTON_BACK, gamepad.wButtons, XINPUT_GAMEPAD_BACK );
        UpdateButton( XBOX_BUTTON_START, gamepad.wButtons, XINPUT_GAMEPAD_START );
        UpdateButton( XBOX_BUTTON_L_BUMPER, gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER );
        UpdateButton( XBOX_BUTTON_R_BUMPER, gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER );
        UpdateButton( XBOX_BUTTON_L_THUMB, gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB );
        UpdateButton( XBOX_BUTTON_R_THUMB, gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB );
        UpdateButton( XBOX_BUTTON_D_RIGHT, gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT );
        UpdateButton( XBOX_BUTTON_D_UP, gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP );
        UpdateButton( XBOX_BUTTON_D_LEFT, gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT );
        UpdateButton( XBOX_BUTTON_D_DOWN, gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN );

        // Sets the controller vibration

        vibrationInput.wLeftMotorSpeed = m_LeftVibration;
        vibrationInput.wRightMotorSpeed = m_RightVirbration;

    }
    else
    {
        m_IsConnected = false;
        Reset();

    }

    XInputSetState( m_ControllerId, &vibrationInput );
}

void XboxController::Reset()
{
    for ( int buttonId = 0; buttonId < NUM_XBOX_BUTTONS; ++buttonId )
    {
        m_ButtonStates[ buttonId ].Reset();
    }

    m_LeftJoystick.Reset();
    m_RightJoystick.Reset();

    m_LeftTriggerValue = 0.f;
    m_RightTriggerValue = 0.f;

    m_LeftVibration = 0;
    m_LeftVibration = 0;
}

void XboxController::UpdateTrigger( float& triggerValue, unsigned char rawValue )
{
    triggerValue = RangeMapFloat( 0.f, 255.f, 0.f, 1.f, rawValue );
}

void XboxController::UpdateJoystick( AnalogJoystick& joystick, 
                                     short rawX, 
                                     short rawY )
{
    float rawNormalizedX = RangeMapFloat( -32768.f, 32767.f, -1.f, 1.f, rawX );
    float rawNormalizedY = RangeMapFloat( -32768.f, 32767.f, -1.f, 1.f, rawY );

    joystick.UpdatePosition( rawNormalizedX, rawNormalizedY );
}

void XboxController::UpdateButton( XboxButtonID buttonId, 
                                   unsigned short buttonFlags, 
                                   unsigned short buttonFlag )
{
    m_ButtonStates[ buttonId ].UpdateStatus( (buttonFlags & buttonFlag) == buttonFlag );
}
