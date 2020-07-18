#pragma once

#include "Engine/Event/EventSystem.hpp"
#include "Engine/Input/XboxController.hpp"

#include <queue>
#include <stack>

constexpr int MAX_XBOX_CONTROLLERS = 4;
constexpr int MAX_KEY_BUTTONS = 256;

extern const unsigned char MOUSE_L;
extern const unsigned char MOUSE_R;
extern const unsigned char MOUSE_M;

extern const unsigned char ESC;
extern const unsigned char ENTER;
extern const unsigned char SPACE;
extern const unsigned char BACKSPACE;
extern const unsigned char INSERT;
extern const unsigned char DEL;
extern const unsigned char HOME;
extern const unsigned char END;

extern const unsigned char RIGHT_ARROW;
extern const unsigned char UP_ARROW;
extern const unsigned char LEFT_ARROW;
extern const unsigned char DOWN_ARROW;

extern const unsigned char GRAVE;

extern const unsigned char PLUS;
extern const unsigned char MINUS;
extern const unsigned char L_BRACKET;
extern const unsigned char R_BRACKET;
extern const unsigned char SEMI_COLON;
extern const unsigned char QUOTE;
extern const unsigned char COMMA;
extern const unsigned char PERIOD;

extern const unsigned char CTRL;
extern const unsigned char SHIFT;

extern const unsigned char F1;
extern const unsigned char F2;
extern const unsigned char F3;
extern const unsigned char F4;
extern const unsigned char F5;
extern const unsigned char F6;
extern const unsigned char F7;
extern const unsigned char F8;
extern const unsigned char F9;
extern const unsigned char F10;
extern const unsigned char F11;
extern const unsigned char F12;

class Window;

enum class MouseMode
{
    MOUSE_RELATIVE,
    MOUSE_ABSOLUTE,
};

struct MouseState
{
    bool hidden = false;
    bool clipped = false;
    MouseMode mouseMode = MouseMode::MOUSE_ABSOLUTE;
};

class InputSystem
{
public:
    static InputSystem& INSTANCE();
    static void ForceShowCursor();

    InputSystem( const InputSystem& ) = delete;
    void operator=( const InputSystem& ) = delete;

    void BeginFrame();
    void EndFrame();

    void SetWindow( Window* window ) { m_Window = window; }
    void PushMouseState( MouseState nextState );
    void PopMouseState();

    //-----------------------------------------------------------------------------
    // Event based key inputs
    static bool HandleKeyDown( EventArgs* args );
    static bool HandleKeyUp( EventArgs* args );
    static bool HandleBufferedInput( EventArgs* args );
    static bool HandleMouseButtons( EventArgs* args );
    static bool HandleMouseWheelScroll( EventArgs* args );

    //-----------------------------------------------------------------------------
    // Check various button and input states
    const KeyButtonState& GetVirtualKey( unsigned char keyCode ) const;
    bool IsKeyPressed( unsigned char keyCode ) const;
    bool WasKeyJustPressed( unsigned char keyCode ) const;
    bool WasKeyJustReleased( unsigned char keyCode ) const;
    bool ConsumeIsKeyPressed( unsigned char keyCode );
    bool ConsumeWasKeyJustPressed( unsigned char keyCode );
    bool ConsumeWasKeyJustReleased( unsigned char keyCode );

    char GetTopBufferedInput();
    std::string GetFullBufferedInput();
    float GetScrollAmount() const { return m_ScrollAmount; }
    Vec2 GetMousePositionInWindow() const { return m_MousePositionInWindow; }
    Vec2 GetMousePositionNormalized() const { return m_MousePositionNormalized; }
    Vec2 GetMouseDelta() const { return m_MouseDelta; }

    //-----------------------------------------------------------------------------
    // Check if any keys are down
    bool IsAnyKeyDown() const;
    bool IsAnyKeyJustPressed() const;

    //-----------------------------------------------------------------------------
    // Xbox controller support
    const XboxController& GetXboxController( ControllerId controllerId ) const;
    void SetXboxControllerVibration( int controllerId,
                                     float leftVibrationPercent,
                                     float rightVibrationPercent );

private:
    Window* m_Window = nullptr;

    KeyButtonState m_Buttons[ MAX_KEY_BUTTONS ];
    XboxController m_Controller[ MAX_XBOX_CONTROLLERS ] =
    {
        XboxController( 0 ),
        XboxController( 1 ),
        XboxController( 2 ),
        XboxController( 3 ),
    };

    std::queue<char> m_BufferedInput;

    std::stack<MouseState> m_StackedMouseStates;
    MouseMode m_CurrentMouseMode = MouseMode::MOUSE_ABSOLUTE;
    bool m_NewMouseState = false;

    Vec2 m_MousePositionNormalized = Vec2::ZERO;
    Vec2 m_MousePositionInWindow = Vec2::ZERO;
    Vec2 m_LastMousePosition = Vec2::ZERO;
    Vec2 m_MouseDelta = Vec2::ZERO;
    float m_ScrollAmount = 0;

    InputSystem();

    void UpdateMouse();
    void UpdateMouseState(const MouseState& mouseState);
    void UpdateMousePosition();
    void UpdateMouseDelta();
    void SetMouseToPosition(const Vec2& mousePos);

    void AddScrollAmount( float delta );
};
