#pragma once

#include "Engine/Console/Console.hpp"
#include "Engine/Core/EngineCommon.hpp"

//-----------------------------------------------------------------------------
// Engine Predefines
class InputSystem;

enum class WindowStyle
{
    BORDERED,
    BORDERLESS_FULLSCREEN,
};

class Window
{
public:
    void* m_Hwnd = nullptr;

    Window();
    ~Window();

    bool Open( const std::string& title, 
               float clientAspect = 16.0f / 9.f, 
               float maxClientFractionOfDesktop = .9f,
               const Vec2& alignment = Vec2::ALIGN_CENTERED );
    bool OpenFullscreen();

    void Close();

    void BeginFrame();

    Vec2 GetCenter() const;
    unsigned int GetClientWidth() const { return m_Width; }
    unsigned int GetClientHeight() const { return m_Height; }

    void SetTitle( const std::string& title );
    void SetWindowStyle( WindowStyle newStyle );

    void ToggleFullscreen();
    void SetFullscreenMode( bool newMode );

private:
    std::string m_WindowTitle = "";

    Vec2 m_Alignment = Vec2::ALIGN_CENTERED;
    unsigned int m_Width = 0;
    unsigned int m_Height = 0;

    bool m_IsFullscreen = false;

    bool CreateWindowFromStyle( unsigned long styleFlags, unsigned long extendedStyleFlags, long* clientRect );
    unsigned long GetWindowStyle( WindowStyle newStyle );
};