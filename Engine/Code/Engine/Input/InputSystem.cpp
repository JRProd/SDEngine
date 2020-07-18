#include "InputSystem.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/Primatives/AABB2.hpp"
#include "Engine/OS/Window.hpp"

// Unassigned hex codes. Use to have mouse button be stored with the same system as VK_BUTTONS
EXTERNED const unsigned char MOUSE_L = VK_RBUTTON;
EXTERNED const unsigned char MOUSE_R = VK_LBUTTON;
EXTERNED const unsigned char MOUSE_M = VK_MBUTTON;

EXTERNED const unsigned char ESC = VK_ESCAPE;
EXTERNED const unsigned char ENTER = VK_RETURN;
EXTERNED const unsigned char SPACE = VK_SPACE;
EXTERNED const unsigned char BACKSPACE = VK_BACK;
EXTERNED const unsigned char INSERT = VK_INSERT;
EXTERNED const unsigned char DEL = VK_DELETE;
EXTERNED const unsigned char HOME = VK_HOME;
EXTERNED const unsigned char END = VK_END;

EXTERNED const unsigned char RIGHT_ARROW = VK_RIGHT;
EXTERNED const unsigned char UP_ARROW = VK_UP;
EXTERNED const unsigned char LEFT_ARROW = VK_LEFT;
EXTERNED const unsigned char DOWN_ARROW = VK_DOWN;

EXTERNED const unsigned char GRAVE = VK_OEM_3;

EXTERNED const unsigned char PLUS = VK_OEM_PLUS;
EXTERNED const unsigned char MINUS = VK_OEM_MINUS;
EXTERNED const unsigned char L_BRACKET = VK_OEM_4;
EXTERNED const unsigned char R_BRACKET = VK_OEM_6;
EXTERNED const unsigned char SEMI_COLON = VK_OEM_1;
EXTERNED const unsigned char QUOTE = VK_OEM_7;
EXTERNED const unsigned char COMMA = VK_OEM_COMMA;
EXTERNED const unsigned char PERIOD = VK_OEM_PERIOD;

EXTERNED const unsigned char CTRL = VK_CONTROL;
EXTERNED const unsigned char SHIFT = VK_SHIFT;

EXTERNED const unsigned char F1 = VK_F1;
EXTERNED const unsigned char F2 = VK_F2;
EXTERNED const unsigned char F3 = VK_F3;
EXTERNED const unsigned char F4 = VK_F4;
EXTERNED const unsigned char F5 = VK_F5;
EXTERNED const unsigned char F6 = VK_F6;
EXTERNED const unsigned char F7 = VK_F7;
EXTERNED const unsigned char F8 = VK_F8;
EXTERNED const unsigned char F9 = VK_F9;
EXTERNED const unsigned char F10 = VK_F10;
EXTERNED const unsigned char F11 = VK_F11;
EXTERNED const unsigned char F12 = VK_F12;

STATIC InputSystem* INSTANCE = nullptr;

InputSystem::InputSystem()
{
    PushMouseState( MouseState() );

    g_EventSystem->Subscribe( "keyDown", HandleKeyDown );
    g_EventSystem->Subscribe( "keyUp", HandleKeyUp );
    g_EventSystem->Subscribe( "charInput", HandleBufferedInput );
    g_EventSystem->Subscribe( "mouseButtons", HandleMouseButtons );
    g_EventSystem->Subscribe( "mouseWheelScroll", HandleMouseWheelScroll );
}

InputSystem& InputSystem::INSTANCE()
{
    static InputSystem s;
    return s;
}

void InputSystem::ForceShowCursor()
{
    while ( ShowCursor( true ) < 0 );
}

void InputSystem::BeginFrame()
{
    for ( int controllerId = 0; controllerId < MAX_XBOX_CONTROLLERS; ++controllerId )
    {
        m_Controller[ controllerId ].Update();
    }

    UpdateMouse();
}

void InputSystem::EndFrame()
{
    // Updates the pressed states of all buttons
    for ( int buttonIndex = 0; buttonIndex < MAX_KEY_BUTTONS; ++buttonIndex )
    {
        KeyButtonState& currentButton = m_Buttons[ buttonIndex ];
        currentButton.m_WasJustPressed = currentButton.m_IsPressed;
    }

    // Clear the queue
    // https://stackoverflow.com/a/709161
    std::queue<char>().swap( m_BufferedInput );

    m_ScrollAmount = 0.f;
}

void InputSystem::PushMouseState( MouseState nextState )
{
    m_StackedMouseStates.push( nextState );
    m_NewMouseState = true;
}

void InputSystem::PopMouseState()
{
    if ( m_StackedMouseStates.size() <= 1 )
    {
#if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_WARNING, "PopMouseState: No state able to be popped." );
#endif // !defined(ENGINE_DISABLE_CONSOLE)
        return;
    }

    m_NewMouseState = true;
    m_StackedMouseStates.pop();
}

STATIC bool InputSystem::HandleKeyDown( EventArgs* args )
{
    const unsigned char keyCode = args->GetValue( "keyCode", static_cast<unsigned char>(255) );
    INSTANCE().m_Buttons[ keyCode ].m_IsPressed = true;

    return false;
}

STATIC bool InputSystem::HandleKeyUp( EventArgs* args )
{
    const unsigned char keyCode = args->GetValue( "keyCode", static_cast<unsigned char>(255) );
    INSTANCE().m_Buttons[ keyCode ].m_IsPressed = false;

    return false;
}

STATIC bool InputSystem::HandleBufferedInput( EventArgs* args )
{
    const char charCode = args->GetValue( "charCode", '\0' );
    // Characters that have special functionality such as Tab, Backspace, Enter
    if ( charCode != '\0' && charCode != '\t' && charCode != '\r' && charCode !=
        '\x1b' )
    {
        INSTANCE().m_BufferedInput.push( charCode );
    }

    return false;
}

STATIC bool InputSystem::HandleMouseButtons( EventArgs* args )
{
    const bool leftMouseButton = args->GetValue( "leftMouseButton", false );
    const bool rightMouseButton = args->GetValue( "rightMouseButton", false );
    const bool middleMouseButton = args->GetValue( "middleMouseButton", false );

    INSTANCE().m_Buttons[ MOUSE_L ].m_IsPressed = leftMouseButton;
    INSTANCE().m_Buttons[ MOUSE_R ].m_IsPressed = rightMouseButton;
    INSTANCE().m_Buttons[ MOUSE_M ].m_IsPressed = middleMouseButton;

    return false;
}

STATIC bool InputSystem::HandleMouseWheelScroll( EventArgs* args )
{
    const float scrollAmount = args->GetValue( "mouseScrollValue", 0.0f );
    INSTANCE().AddScrollAmount( scrollAmount );
    return false;
}

const KeyButtonState& InputSystem::GetVirtualKey( unsigned char keyCode ) const
{
    return m_Buttons[ keyCode ];
}

bool InputSystem::IsKeyPressed( unsigned char keyCode ) const
{
    return m_Buttons[ keyCode ].IsPressed();
}

bool InputSystem::WasKeyJustPressed( unsigned char keyCode ) const
{
    return m_Buttons[ keyCode ].WasJustPressed();
}

bool InputSystem::WasKeyJustReleased( unsigned char keyCode ) const
{
    return m_Buttons[ keyCode ].WasJustReleased();
}

char InputSystem::GetTopBufferedInput()
{
    const char front = m_BufferedInput.front();
    m_BufferedInput.pop();
    return front;
}

bool InputSystem::ConsumeIsKeyPressed( unsigned char keyCode )
{
    const bool savedValue = m_Buttons[ keyCode ].IsPressed();
    m_Buttons[ keyCode ].m_IsPressed = false;
    return savedValue;
}

std::string InputSystem::GetFullBufferedInput()
{
    std::string fullInput;
    while ( !m_BufferedInput.empty() )
    {
        fullInput += m_BufferedInput.front();
        m_BufferedInput.pop();
    }
    return fullInput;
}

bool InputSystem::ConsumeWasKeyJustPressed( unsigned char keyCode )
{
    const bool savedValue = m_Buttons[ keyCode ].WasJustPressed();
    m_Buttons[ keyCode ].m_IsPressed = false;

    return savedValue;
}

bool InputSystem::ConsumeWasKeyJustReleased( unsigned char keyCode )
{
    const bool savedValue = m_Buttons[ keyCode ].WasJustReleased();
    m_Buttons[ keyCode ].m_WasJustPressed = false;
    return savedValue;
}

bool InputSystem::IsAnyKeyDown() const
{
    for ( int buttonIndex = 0; buttonIndex < MAX_KEY_BUTTONS; ++buttonIndex )
    {
        if ( m_Buttons[ buttonIndex ].IsPressed() )
        {
            return true;
        }
    }

    for ( int controllerIndex = 0; controllerIndex < MAX_XBOX_CONTROLLERS; ++controllerIndex )
    {
        const XboxController& controller = m_Controller[ controllerIndex ];

        if ( controller.IsConnected() )
        {
            const KeyButtonState* const& buttons = controller.m_ButtonStates;
            for ( int buttonIndex = 0; buttonIndex < NUM_XBOX_BUTTONS; ++buttonIndex )
            {
                if ( buttons[ buttonIndex ].IsPressed() )
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool InputSystem::IsAnyKeyJustPressed() const
{
    for ( int buttonIndex = 0; buttonIndex < MAX_KEY_BUTTONS; ++buttonIndex )
    {
        if ( m_Buttons[ buttonIndex ].WasJustPressed() )
        {
            return true;
        }
    }

    for ( int controllerIndex = 0; controllerIndex < MAX_XBOX_CONTROLLERS; ++controllerIndex )
    {
        const XboxController& controller = m_Controller[ controllerIndex ];

        if ( controller.IsConnected() )
        {
            const KeyButtonState* const& buttons = controller.m_ButtonStates;
            for ( int buttonIndex = 0; buttonIndex < NUM_XBOX_BUTTONS; ++buttonIndex )
            {
                if ( buttons[ buttonIndex ].WasJustPressed() )
                {
                    return true;
                }
            }
        }
    }
    return false;
}

const XboxController& InputSystem::GetXboxController( ControllerId controllerId ) const
{
    return m_Controller[ controllerId ];
}

void InputSystem::SetXboxControllerVibration( int controllerId,
                                              float leftVibrationPercent,
                                              float rightVibrationPercent )
{
    XboxController& controller = m_Controller[ controllerId ];
    controller.SetLeftVibration( leftVibrationPercent );
    controller.SetRightVibration( rightVibrationPercent );
}

void InputSystem::UpdateMouse()
{
    if ( m_NewMouseState )
    {
        const MouseState& nextState = m_StackedMouseStates.top();
        UpdateMouseState( nextState );
    }

    switch ( m_CurrentMouseMode )
    {
    case MouseMode::MOUSE_ABSOLUTE:
        UpdateMousePosition();
        break;
    case MouseMode::MOUSE_RELATIVE:
        UpdateMouseDelta();
        break;
    default:
        break;
    }
}

void InputSystem::UpdateMouseState( const MouseState& mouseState )
{
    if ( mouseState.hidden )
    {
        while ( ShowCursor( false ) >= 0 );
    }
    else
    {
        ForceShowCursor();
    }

    if ( mouseState.clipped )
    {
        HWND hwnd = static_cast<HWND>(m_Window->m_Hwnd);
        RECT clipRect;
        GetClientRect( hwnd, &clipRect );
        ClipCursor( &clipRect );
    }
    else
    {
        ClipCursor( nullptr );
    }

    m_CurrentMouseMode = mouseState.mouseMode;
    if ( m_CurrentMouseMode == MouseMode::MOUSE_RELATIVE )
    {
        SetMouseToPosition( m_Window->GetCenter() );
    }

    m_NewMouseState = false;
}

void InputSystem::UpdateMousePosition()
{
    HWND hwnd = static_cast<HWND>(m_Window->m_Hwnd);

    // Get raw mouse point and make it a Vec2
    POINT mousePos;
    GetCursorPos( &mousePos );
    ScreenToClient( hwnd, &mousePos );
    const Vec2 mouseClientPos = Vec2( static_cast<float>(mousePos.x),
                                      static_cast<float>(mousePos.y) );

    // Get the full desktop rectangle and make it an AABB2
    RECT desktopRect;
    GetClientRect( hwnd, &desktopRect );
    const AABB2 clientBounds = AABB2( static_cast<float>(desktopRect.left),
                                      static_cast<float>(desktopRect.bottom),
                                      static_cast<float>(desktopRect.right),
                                      static_cast<float>(desktopRect.top) );

    m_MousePositionInWindow = mouseClientPos;
    m_MousePositionNormalized = clientBounds.GetUVAtPoint( mouseClientPos );
}

void InputSystem::UpdateMouseDelta()
{
    POINT mousePos;
    GetCursorPos( &mousePos );

    m_MouseDelta.x = mousePos.x - m_LastMousePosition.x;
    m_MouseDelta.y = -(mousePos.y - m_LastMousePosition.y);

    SetMouseToPosition( m_Window->GetCenter() );
}

void InputSystem::SetMouseToPosition( const Vec2& mousePos )
{
    SetCursorPos( static_cast<int>(mousePos.x), static_cast<int>(mousePos.y) );

    POINT actualPoint;
    GetCursorPos( &actualPoint );

    m_LastMousePosition.x = static_cast<float>(actualPoint.x);
    m_LastMousePosition.y = static_cast<float>(actualPoint.y);
}

void InputSystem::AddScrollAmount( float delta )
{
    m_ScrollAmount += delta;
}
