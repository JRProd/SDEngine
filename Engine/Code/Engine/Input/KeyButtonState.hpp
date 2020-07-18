#pragma once

class KeyButtonState
{

    friend class InputSystem;

public:
    void Update();
    void UpdateStatus( bool isNowPressed );
    bool IsPressed() const { return m_IsPressed; };
    bool WasJustPressed() const { return m_IsPressed && !m_WasJustPressed; }
    bool WasJustReleased() const { return !m_IsPressed && m_WasJustPressed; }
    void Reset();
private:
    bool m_IsPressed = false;
    bool m_WasJustPressed = false;

};