#pragma once

//-----------------------------------------------------------------------------
// Common used macros
// Use to specify unused variables.
#define UNUSED(x) (void) x
// Used to specify out parameters
//  Compiles to nothing
#define OUT_PARAM
// Used to delineate static in cpp files, does nothing
#define STATIC
// Used to delineate extern variables in cpp files, does nothing
#define EXTERNED

// Used to create bit flags
template <unsigned int b>
constexpr unsigned int BIT_FLAG = 1 << b;

//-----------------------------------------------------------------------------
// Universal #includes
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/Primatives/Mat44.hpp"

#include "Game/EngineBuildPreferences.hpp"

#include <stdint.h>
//-----------------------------------------------------------------------------
// Engine wide systems
#if !defined(ENGINE_DISABLE_CONSOLE)
class Console;
extern Console* g_Console;
#endif // !defined(ENGINE_DISABLE_CONSOLE)

#if !defined(ENGINE_DISABLE_EVENT_SYSTEM)
class EventSystem;
extern EventSystem* g_EventSystem;
#endif // !defined(ENGINE_DISABLE_EVENT_SYSTEM)

extern NamedStrings g_GameConfigBlackboard;
