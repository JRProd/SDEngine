#include "Engine/OS/Window.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Engine/Input/InputSystem.hpp"

static TCHAR const* WND_CLASS_NAME = TEXT( "Simple Window Class" );
static unsigned int g_WindowCount = 0;

static DWORD g_ExStyleFlags = WS_EX_APPWINDOW;

static size_t LEFT = 0;
static size_t TOP = 1;
static size_t RIGHT = 2;
static size_t BOTTOM = 3;

//-------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
static LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode,
                                                         WPARAM wParam, LPARAM lParam )
{
    switch ( wmMessageCode )
    {
    case WM_CREATE:
        {
            MouseState createdState;
            createdState.clipped = false;
            createdState.hidden = false;
            createdState.mouseMode = MouseMode::MOUSE_ABSOLUTE;
            InputSystem::INSTANCE().PushMouseState( createdState );
        }

    case WM_ACTIVATE:
        {
            if ( wParam == 0 )
            {
                MouseState deactivated;
                deactivated.clipped = false;
                deactivated.hidden = false;
                deactivated.mouseMode = MouseMode::MOUSE_ABSOLUTE;
                InputSystem::INSTANCE().PushMouseState( deactivated );
                return 0;
            }
            InputSystem::INSTANCE().PopMouseState();
            return 0;
        }

        // App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
    case WM_CLOSE:
        {
            // Does not use the EventSystem version because this should override any attempt to consume it before
            //  the app uses it.
            g_EventSystem->Publish( "WM_CLOSE" );
            return 0; // "Consumes" this message (tells Windows "okay, we handled it")
        }

        // Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
    case WM_KEYDOWN:
        {
            unsigned char asKey = static_cast<unsigned char>(wParam);
            EventArgs keyArgs;
            keyArgs.SetValue( "keyCode", asKey );
            g_EventSystem->Publish( "keyDown", &keyArgs );
            return 0;
        }

        // Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
    case WM_KEYUP:
        {
            unsigned char asKey = static_cast<unsigned char>(wParam);
            EventArgs keyArgs;
            keyArgs.SetValue( "keyCode", asKey );
            g_EventSystem->Publish( "keyUp", &keyArgs );
            return 0;
        }

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        {
            const bool leftButtonDown = wParam & MK_LBUTTON;
            const bool rightButtonDown = wParam & MK_RBUTTON;
            const bool middleButtonDown = wParam & MK_MBUTTON;

            EventArgs mouseArgs;
            mouseArgs.SetValue( "leftMouseButton", leftButtonDown );
            mouseArgs.SetValue( "rightMouseButton", rightButtonDown );
            mouseArgs.SetValue( "middleMouseButton", middleButtonDown );

            g_EventSystem->Publish( "mouseButtons", &mouseArgs );
            break;
        }

    case WM_MOUSEWHEEL:
        {
            constexpr float FIXED_POINT_SCROLL_VALUE = 1.f / 120.f;
            const float scrollFixPoint = static_cast<float>(GET_WHEEL_DELTA_WPARAM( wParam ));
            const float scrollValue = scrollFixPoint * FIXED_POINT_SCROLL_VALUE;

            EventArgs mouseArgs;
            mouseArgs.SetValue( "mouseScrollValue", scrollValue );

            g_EventSystem->Publish( "mouseWheelScroll", &mouseArgs );
            break;
        }

    case WM_CHAR:
        {
            const char asChar = static_cast<char>(wParam);
            EventArgs charArgs;
            charArgs.SetValue( "charCode", asChar );
            g_EventSystem->Publish( "charInput", &charArgs );
        }
    }

    // Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
    return ::DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}

static void RegisterWindowClass()
{
    // Define a window style/class
    WNDCLASSEX windowClassDescription;
    memset( &windowClassDescription, 0, sizeof(windowClassDescription) );
    windowClassDescription.cbSize = sizeof(windowClassDescription);
    windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
    windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure
    ); // Register our Windows message-handling function
    windowClassDescription.hInstance = ::GetModuleHandle( nullptr );
    windowClassDescription.hIcon = nullptr;
    windowClassDescription.hCursor = nullptr;
    windowClassDescription.lpszClassName = WND_CLASS_NAME;
    ::RegisterClassEx( &windowClassDescription );
}

static void UnregisterWindowClass()
{
    ::UnregisterClass( WND_CLASS_NAME, ::GetModuleHandle( nullptr ) );
}

Window::Window()
    : m_Hwnd( nullptr )
{
    if ( g_WindowCount == 0 )
    {
        RegisterWindowClass();
    }
    ++g_WindowCount;
}

Window::~Window()
{
    Close();

    --g_WindowCount;
    if ( g_WindowCount == 0 )
    {
        UnregisterWindowClass();
    }
}

bool Window::Open( const std::string& title,
                   float clientAspect,
                   float maxClientFractionOfDesktop,
                   const Vec2& alignment )
{
    // Get desktop rect, dimensions, aspect
    RECT desktopRect;
    HWND desktopWindowHandle = GetDesktopWindow();
    GetClientRect( desktopWindowHandle, &desktopRect );
    float desktopWidth = static_cast<float>(desktopRect.right - desktopRect.left);
    float desktopHeight = static_cast<float>(desktopRect.bottom - desktopRect.top);
    float desktopAspect = desktopWidth / desktopHeight;

    // Calculate maximum client size (as some % of desktop size)
    float clientWidth = desktopWidth * maxClientFractionOfDesktop;
    float clientHeight = desktopHeight * maxClientFractionOfDesktop;
    if ( clientAspect > desktopAspect )
    {
        // Client window has a wider aspect than desktop; shrink client height to match its width
        clientHeight = clientWidth / clientAspect;
    }
    else
    {
        // Client window has a taller aspect than desktop; shrink client width to match its height
        clientWidth = clientHeight * clientAspect;
    }

    // Calculate client rect margins by the given alignment
    m_Alignment = alignment;
    float clientMarginX = m_Alignment.x * (desktopWidth - clientWidth);
    float clientMarginY = (1.f - m_Alignment.y) * (desktopHeight - clientHeight);

    // Calculate the final client rect
    long finalSize[ 4 ];
    finalSize[ LEFT ] = static_cast<int>(clientMarginX);
    finalSize[ RIGHT ] = finalSize[ LEFT ] + static_cast<int>(clientWidth);
    finalSize[ TOP ] = static_cast<int>(clientMarginY);
    finalSize[ BOTTOM ] = finalSize[ TOP ] + static_cast<int>(clientHeight);

    m_Width = finalSize[ RIGHT ] - finalSize[ LEFT ];
    m_Height = finalSize[ BOTTOM ] - finalSize[ TOP ];

    // Create the window
    bool success = CreateWindowFromStyle( GetWindowStyle( WindowStyle::BORDERED ), g_ExStyleFlags,
                                          finalSize );
    SetTitle( title );

    return success;
}

bool Window::OpenFullscreen()
{
    RECT desktopRect;
    HWND desktopClientHandle = GetDesktopWindow();
    GetClientRect( desktopClientHandle, &desktopRect );

    long desktop[ 4 ];
    desktop[ LEFT ] = desktopRect.left;
    desktop[ TOP ] = desktopRect.top;
    desktop[ RIGHT ] = desktopRect.right;
    desktop[ BOTTOM ] = desktopRect.bottom;

    return CreateWindowFromStyle( GetWindowStyle( WindowStyle::BORDERLESS_FULLSCREEN ),
                                  g_ExStyleFlags, desktop );
}

void Window::Close()
{
    HWND hwnd = static_cast<HWND>(m_Hwnd);
    if ( hwnd == nullptr )
    {
        return;
    }

    DestroyWindow( hwnd );
    m_Hwnd = nullptr;
}

//-------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
void Window::BeginFrame()
{
    MSG queuedMessage;
    for ( ;; )
    {
        const BOOL wasMessagePresent = PeekMessage( &queuedMessage, nullptr, 0, 0, PM_REMOVE );
        if ( !wasMessagePresent )
        {
            break;
        }

        TranslateMessage( &queuedMessage );
        DispatchMessage(
            &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
    }
}

Vec2 Window::GetCenter() const
{
    RECT clientRect;
    GetWindowRect( static_cast<HWND>(m_Hwnd), &clientRect );

    return Vec2( clientRect.left + (clientRect.right - clientRect.left) * .5f,
                 clientRect.top + (clientRect.bottom - clientRect.top) * .5f );
}

void Window::SetTitle( const std::string& title )
{
    m_WindowTitle = title;
    SetWindowTextA( static_cast<HWND>(m_Hwnd), title.c_str() );
}

void Window::SetWindowStyle( WindowStyle newStyle )
{
    DWORD windowStyle = GetWindowStyle( newStyle );
    SetWindowLongPtr( static_cast<HWND>(m_Hwnd), GWL_STYLE, windowStyle );

    HMONITOR monitorHandle = MonitorFromWindow( static_cast<HWND>(m_Hwnd),
                                                MONITOR_DEFAULTTOPRIMARY );
    MONITORINFOEX monitorInfo;
    ::GetMonitorInfo( monitorHandle, &monitorInfo );

    RECT desktopRect;
    HWND desktopClientHandle = GetDesktopWindow();
    GetClientRect( desktopClientHandle, &desktopRect );

    RECT newWindowSize = desktopRect;
    switch ( newStyle )
    {
    case WindowStyle::BORDERED:
        {
            int desktopWidth = newWindowSize.right - newWindowSize.left;
            int desktopHeight = newWindowSize.bottom - newWindowSize.top;
            long clientMarginX = static_cast<long>(m_Alignment.x * (desktopWidth - m_Width));
            long clientMarginY = static_cast<long>((1.f - m_Alignment.y) * (desktopHeight - m_Height
            ));

            newWindowSize.left = clientMarginX;
            newWindowSize.right = newWindowSize.left + m_Width;
            newWindowSize.top = clientMarginY;
            newWindowSize.bottom = newWindowSize.top + m_Height;

            SetWindowPos( static_cast<HWND>(m_Hwnd),
                          nullptr,
                          newWindowSize.left,
                          newWindowSize.top,
                          m_Width,
                          m_Height,
                          0 );
        }
        break;
    case WindowStyle::BORDERLESS_FULLSCREEN:
        SetWindowPos( static_cast<HWND>(m_Hwnd),
                      nullptr,
                      0,
                      0,
                      newWindowSize.right,
                      newWindowSize.bottom,
                      0 );
        break;
    default:
        break;
    }

    ShowWindow( static_cast<HWND>(m_Hwnd), SW_SHOW );
    SetForegroundWindow( static_cast<HWND>(m_Hwnd) );
    SetFocus( static_cast<HWND>(m_Hwnd) );
}

void Window::ToggleFullscreen()
{
    SetFullscreenMode( !m_IsFullscreen );
}

void Window::SetFullscreenMode( bool newMode )
{
    if ( m_IsFullscreen != newMode )
    {
        m_IsFullscreen = newMode;

        if ( m_IsFullscreen )
        {
            SetWindowStyle( WindowStyle::BORDERLESS_FULLSCREEN );
        }
        else
        {
            SetWindowStyle( WindowStyle::BORDERED );
        }
    }
}

bool Window::CreateWindowFromStyle( unsigned long styleFlags, unsigned long extendedStyleFlags,
                                    long* clientRect )
{
    // Calculate the outer dimensions of the physical window, including frame et. al.
    RECT windowRect;
    windowRect.left = clientRect[ LEFT ];
    windowRect.top = clientRect[ TOP ];
    windowRect.right = clientRect[ RIGHT ];
    windowRect.bottom = clientRect[ BOTTOM ];
    AdjustWindowRectEx( &windowRect, styleFlags, FALSE, extendedStyleFlags );

    HWND hwnd = CreateWindowEx(
        extendedStyleFlags,
        WND_CLASS_NAME,
        nullptr,
        styleFlags,
        windowRect.left,
        windowRect.top,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        static_cast<HINSTANCE>(::GetModuleHandle( nullptr )),
        nullptr );

    m_Hwnd = static_cast<void*>(hwnd);
    m_Width = static_cast<unsigned int>(clientRect[ RIGHT ] - clientRect[ LEFT ]);
    m_Height = static_cast<unsigned int>(clientRect[ BOTTOM ] - clientRect[ TOP ]);

    ::SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG_PTR)this );

    if ( hwnd == nullptr )
    {
        return false;
    }

    ShowWindow( hwnd, SW_SHOW );
    SetForegroundWindow( hwnd );
    SetFocus( hwnd );

    HCURSOR cursor = ::LoadCursor( nullptr, IDC_ARROW );
    SetCursor( cursor );

    return true;
}

unsigned long Window::GetWindowStyle( WindowStyle newStyle )
{
    switch ( newStyle )
    {
    case WindowStyle::BORDERED:
        return WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
    case WindowStyle::BORDERLESS_FULLSCREEN:
        return WS_POPUP;
    default:
        return GetWindowStyle( WindowStyle::BORDERED );
    }
}
