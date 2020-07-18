#include "KeyButtonState.hpp"

void KeyButtonState::Update()
{
    m_WasJustPressed = m_IsPressed;
}

void KeyButtonState::UpdateStatus( bool isNowPressed )
{
    m_WasJustPressed = m_IsPressed;
    m_IsPressed = isNowPressed;
}

void KeyButtonState::Reset()
{
    m_IsPressed = false;
    m_WasJustPressed = false;
}
