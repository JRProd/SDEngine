#include "Console.hpp"

//-----------------------------------------------------------------------------------------------
// To disable console entirely (and remove requirement to link global console) for any game,
//	#define ENGINE_DISABLE_CONSOLE in your game's Code/Game/EngineBuildPreferences.hpp file.
//
// Note that this #include is an exception to the rule "engine code doesn't know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
#include "Game/EngineBuildPreferences.hpp"



#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Renderer/Mesh/MeshUtils.hpp"

#if !defined(ENGINE_DISABLE_CONSOLE)

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Fonts/SimpleTriangleFont.hpp"
#include "Engine/Renderer/Fonts/BitmapFont.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"

EXTERNED Console* g_Console = nullptr;

static const char* FATAL_COLOR = "[c255,0,255]";
static const char* ERROR_COLOR = "[c255,0,0]";
static const char* WARNING_COLOR = "[c255,215,40]";
static const char* LOG_COLOR = "[c0,255,0]";
static const char* VERBOSE_COLOR = "[c200,200,200]";
static const char* INFO_COLOR = "[c150,200,255]";

static const Rgba8 COMMAND_COLOR = Rgba8::CreateFromString( "0xc1803e" );
static const Rgba8 OPTIONAL_ARGUMENT_COLOR = Rgba8::CreateFromString( "0x3e7fc1" );
static const Rgba8 REQUIRED_ARGUMENT_COLOR = Rgba8::CreateFromString( "0xc13ec1" );
static const Rgba8 DESCRIPTION_COLOR = Rgba8::CreateFromString( "0x3ec13e" );

static std::string GetLogLevelColor( LogLevel level )
{
    switch ( level )
    {
    case LOG_FATAL:
        return FATAL_COLOR;
    case LOG_ERROR:
        return ERROR_COLOR;
    case LOG_WARNING:
        return WARNING_COLOR;
    case LOG_LOG:
        return LOG_COLOR;
    case LOG_INFO:
        return INFO_COLOR;
    case LOG_VERBOSE:
        return VERBOSE_COLOR;
    default:
        return VERBOSE_COLOR;
    }
}

static std::string GetLogLevelToString( LogLevel level )
{
    std::string logLevelString;
    switch ( level )
    {
    case LOG_FATAL:
        logLevelString = "FATAL";
        break;
    case LOG_ERROR:
        logLevelString = "ERROR";
        break;
    case LOG_WARNING:
        logLevelString = "WARNING";
        break;
    case LOG_LOG:
        logLevelString = "LOG";
        break;
    case LOG_INFO:
        logLevelString = "INFO";
        break;
    case LOG_VERBOSE:
        logLevelString = "VERBOSE";
        break;
    default:
        logLevelString = "";
        break;
    }
    return logLevelString;
}

STATIC EventSystem* Console::s_CommandList = nullptr;
STATIC std::vector<Command> Console::s_RegisteredCommands;

Console::Console()
{
}

STATIC void Console::RegisterCommand( const std::string& commandName, EventCallback* callbackFunction )
{
    RegisterCommand( commandName, "", callbackFunction );
}

void Console::RegisterCommand( const std::string& commandName, const std::string& commandDescription, EventCallback* callbackFunction )
{
    Command cmd;
    cmd.commandName = commandName;
    cmd.description = commandDescription;

    RegisterCommand( cmd, callbackFunction );
}

STATIC void Console::RegisterCommand( const Command& command,
                                      EventCallback* callbackFunction )
{
    s_CommandList->SubscribeUnique( command.commandName, callbackFunction );
    s_RegisteredCommands.push_back( command );
}

bool Console::CommandHelp( EventArgs* args )
{
    if ( args != nullptr )
    {
        const std::string searchQuery = args->GetValue( "Arg1", "" );
        if ( !searchQuery.empty() )
        {
            for ( Command& command : s_RegisteredCommands )
            {
                if ( Stricmp( searchQuery, command.commandName ) == 0 )
                {
                    g_Console->Log( LOG_USER, command.ToMarkdownString(
                                        COMMAND_COLOR, OPTIONAL_ARGUMENT_COLOR,
                                        REQUIRED_ARGUMENT_COLOR, DESCRIPTION_COLOR ), false );
                    return true;
                }
            }

            g_Console->InvalidArgument(
                "Help", Stringf( "%s is not a registered command", searchQuery.c_str() ) );
            return false;
        }
    }

    for ( Command& command : s_RegisteredCommands )
    {
        g_Console->Log( LOG_USER, command.ToMarkdownString(
                            COMMAND_COLOR, OPTIONAL_ARGUMENT_COLOR,
                            REQUIRED_ARGUMENT_COLOR, DESCRIPTION_COLOR ), false );
    }

    return true;
}

bool CommandClear( EventArgs* args )
{
    int numberOfLines = 50;
    if ( args != nullptr )
    {
        numberOfLines = args->GetValue( "lines", numberOfLines );
    }

    for ( int lineNumber = 0; lineNumber < numberOfLines; ++lineNumber )
    {
        g_Console->Log( LOG_USER, "" );
    }

    return true;
}

bool CommandEcho( EventArgs* args )
{
    const std::string echo = args->GetValue<std::string>( "value", "" );

    g_Console->Log( LOG_USER, echo, false );
    return true;
}

void Console::Startup()
{
    if ( s_CommandList == nullptr )
    {
        s_CommandList = new EventSystem();
    }

    RegisterCommand( "Help", &CommandHelp );

    Command clear;
    clear.commandName = "Clear";
    clear.arguments.push_back( new TypedArgument<int>("lines", false));
    clear.description = "Clears the screen a certain number of lines";
    RegisterCommand( clear, CommandClear );

    Command echo;
    echo.commandName = "Echo";
    echo.arguments.push_back( new TypedArgument<std::string>( "value" ) );
    echo.description = "Echo a string to the console";
    RegisterCommand( echo, CommandEcho );

    m_ConsoleShader = new Shader( nullptr );
    m_DeleteTimer = new Timer( &Clock::MASTER, .2f);
}

void Console::BeginFrame()
{
}

void Console::EndFrame()
{
}

void Console::Shutdown()
{
    if ( s_CommandList != nullptr )
    {
        delete s_CommandList;
        s_CommandList = nullptr;
    }
}

void Console::Update( float deltaSeconds )
{
    if ( !m_IsActive ) { return; }

    UNUSED( deltaSeconds );

    if ( m_SubmitCommand )
    {
        ParseCommand();
    }

    for( const char& frameCharacter : m_FrameInput )
    {
        if( frameCharacter == '\b' )
        {
            if( !m_CurrentCommand.empty() && m_CurrentCarotIndex > 0 )
            {
                m_CurrentCommand = m_CurrentCommand.erase( m_CurrentCarotIndex - 1, 1 );
                m_CurrentCarotIndex--;
            }
        }
        else
        {
            m_CurrentCommand.insert( m_CurrentCarotIndex, 1, frameCharacter );
            m_CurrentCarotIndex++;
        }
    }
}

void Console::Log( LogLevel level, const std::string& log, const bool sanitize )
{
    std::string logString = log;
    if ( sanitize )
    {
        logString = SanitizeForMarkdown( logString );
    }

    if( !m_IsActive && level <= LOG_ERROR )
    {
        SetActive( true );
    }

    m_Logs.emplace_back( level, logString );

    if( level <= m_CurrentFilter )
    {
        if( m_ConsoleFont != nullptr )
        {
            const size_t linesAdded = ceili(m_ConsoleFont->GetDimensionForMarkedText( logString, m_ConsoleLineHeight ).y / m_ConsoleLineHeight);
            AddLogToVisual( m_Logs.back(), Vec2( 0, m_TotalLinesLogged * -m_ConsoleLineHeight ) );
            m_TotalLinesLogged += linesAdded;
            m_CurrentVerticalOffset += m_ConsoleLineHeight * linesAdded;
        }
        else
        {
            m_TotalLinesLogged += 1;
            m_CurrentVerticalOffset += m_ConsoleLineHeight;
        }
    }
}

void Console::LogWTF( const std::string& log )
{
    Log( LOG_FATAL, log );
    ERROR_AND_DIE( log );
}

void Console::GuaranteeOrLog( bool condition, LogLevel level, const std::string& log )
{
    if( !condition )
    {
        Log( level, log );
    }
}

void Console::GuaranteeOrWTF( bool condition, const std::string& log )
{
    if ( !condition )
    {
        LogWTF( log );
    }
}

void Console::Render( RenderContext& renderer,
                      const Camera& camera)
{
    if ( !m_IsActive || m_ConsoleView == ConsoleView::CLOSED ) { return; }
    RenderFilter( renderer, camera, m_CurrentFilter );
}

void Console::RenderFilter( RenderContext& renderer,
                            const Camera& camera,
                            LogLevel levelFilter)
{
    if ( !m_IsFinalized )
    {
        *m_ConsoleShader = *renderer.CreateOrGetShaderFromFile( "DEFAULT" );
        m_IsFinalized = true;
    }

    AABB2 cameraBounds = camera.GetOrthoView();
    if ( m_ConsoleView == ConsoleView::BOTTOM_HALF )
    {
        cameraBounds.maxs.y = cameraBounds.maxs.y * .5f;
    }
    else if ( m_ConsoleView == ConsoleView::TOP_HALF )
    {
        cameraBounds.mins.y += cameraBounds.GetDimensions().y * .5f;
    }

    cameraBounds.mins += m_Padding;
    cameraBounds.maxs -= m_Padding;

    int linesVisable = floori( cameraBounds.GetDimensions().y / (m_ConsoleLineHeight + 1) );
    if ( m_ConsoleFont == nullptr )
    {
        linesVisable = 10;
    }

    size_t currentIndex = m_Logs.size() - 1;
    int linesSelected = 0;

    

    RenderOverlayAndText( renderer, cameraBounds );
}

void Console::Toggle()
{
    SetActive( !m_IsActive );
}

void Console::SetActive( bool active )
{
    if ( active && m_ConsoleFont == nullptr && !m_ConsoleFontError )
    {
        m_ConsoleFontError = true;
        Log( LOG_FATAL, "Console: No font set for console, using built-in font" );
    }

    if ( active )
    {
        MouseState consoleState;
        consoleState.clipped = false;
        consoleState.hidden = false;
        consoleState.mouseMode = MouseMode::MOUSE_ABSOLUTE;
        InputSystem::INSTANCE().PushMouseState( consoleState );

        m_ConsoleView = ConsoleView::FULL;
    }
    else
    {
        InputSystem::INSTANCE().PopMouseState();
        Reset();
    }
    m_IsActive = active;
}

void Console::SetConsoleView( ConsoleView newView )
{
    m_ConsoleView = newView;
}

void Console::IncrementConsoleView()
{
    switch ( m_ConsoleView )
    {
    case ConsoleView::FULL:
        m_ConsoleView = ConsoleView::BOTTOM_HALF;
        break;
    case ConsoleView::BOTTOM_HALF:
        m_ConsoleView = ConsoleView::TOP_HALF;
        break;
    case ConsoleView::TOP_HALF:
        m_ConsoleView = ConsoleView::CLOSED;
        SetActive( false );
        break;
    case ConsoleView::CLOSED:
        m_ConsoleView = ConsoleView::FULL;
        SetActive( true );
        break;
    }
}

void Console::SetConsoleLogLevel( LogLevel newLevel )
{
    m_CurrentFilter = newLevel;

    UpdateLogVisualToNewState();
}

void Console::IncrementConsoleLogLevel()
{
    switch ( m_CurrentFilter )
    {
    case LOG_FATAL:
        SetConsoleLogLevel( LOG_VERBOSE );
        break;
    case LOG_ERROR:
        SetConsoleLogLevel( LOG_FATAL );
        break;
    case LOG_WARNING:
        SetConsoleLogLevel( LOG_ERROR );
        break;
    case LOG_LOG:
        SetConsoleLogLevel( LOG_WARNING );
        break;
    case LOG_INFO:
        SetConsoleLogLevel( LOG_LOG );
        break;
    case LOG_VERBOSE:
        SetConsoleLogLevel( LOG_INFO );
        break;
    }
}

void Console::SetConsoleText( BitmapFont* newFont )
{
    m_ConsoleFont = newFont;

    UpdateLogVisualToNewState();
}

void Console::SetLineHeight( float lineHeight )
{
    UpdateLogVisualToNewLineHeight( lineHeight );
    m_ConsoleLineHeight = lineHeight;
}

void Console::InvalidArgument( const EventName& command, const std::string& description )

{
    g_Console->Log( LOG_ERROR,
                    Stringf( "[[c255,0,0]Invalid Argument] for %s:\n%s", command.c_str(),
                             description.c_str() ), false );
}

void Console::UnknownCommand( const EventName& command )
{
    g_Console->Log( LOG_ERROR, Stringf(
                        "[[c255,0,0]Unknown Command] %s\n%s is not a registered command",
                        command.c_str(), command.c_str() ), false );
}

void Console::UpdateInput()
{
    InputSystem* inputSystem = &InputSystem::INSTANCE();

    if ( inputSystem->IsAnyKeyJustPressed() )
    {
        m_LastMoveTime = static_cast<float>(GetCurrentTimeSeconds());
    }

    if ( IsActive() )
    {
        m_FrameInput = InputSystem::INSTANCE().GetFullBufferedInput();

        if ( inputSystem->ConsumeWasKeyJustPressed( ESC ) )
        {
            Toggle();
        }
    }

    if ( !m_CurrentCommand.empty() )
    {
        if ( inputSystem->ConsumeWasKeyJustPressed( ESC ) )
        {
            Reset();
        }
    }

    if ( inputSystem->IsKeyPressed( CTRL ) )
    {
        UpdateInputCtrl();
    }
    else
    {
        UpdateInputNoMod();
    }

    if ( inputSystem->WasKeyJustPressed( UP_ARROW ) )
    {
        if ( !m_History.empty() )
        {
            m_HistoryIndex--;
            if ( m_HistoryIndex < 0 )
            {
                m_HistoryIndex = static_cast<int>(m_History.size()) - 1;
            }

            m_CurrentCommand = m_History.at( m_HistoryIndex );
            m_CurrentCarotIndex = static_cast<int>(m_CurrentCommand.size());
        }
    }

    if ( inputSystem->WasKeyJustPressed( DOWN_ARROW ) )
    {
        if ( !m_History.empty() )
        {
            m_HistoryIndex++;
            if ( m_HistoryIndex >= m_History.size() )
            {
                m_HistoryIndex = 0;
            }

            m_CurrentCommand = m_History.at( m_HistoryIndex );
            m_CurrentCarotIndex = static_cast<int>(m_CurrentCommand.size());
        }
    }

    if ( inputSystem->WasKeyJustPressed( GRAVE ) )
    {
        Toggle();
    }

    if ( inputSystem->WasKeyJustPressed( HOME ) )
    {
        m_CurrentCarotIndex = 0;
    }

    if ( inputSystem->WasKeyJustPressed( END ) )
    {
        m_CurrentCarotIndex = static_cast<unsigned int>(m_CurrentCommand.length());
    }

    if ( inputSystem->WasKeyJustPressed( ENTER ) )
    {
        if ( !m_CurrentCommand.empty() )
        {
            m_SubmitCommand = true;
        }
    }

    if ( inputSystem->IsKeyPressed( DEL ) )
    {
        if( inputSystem->WasKeyJustPressed( DEL ) || ( m_DeleteTimer->CheckAndDecrement() &&
            !m_CurrentCommand.empty() && m_CurrentCarotIndex < m_CurrentCommand.length() ) );
        {
            m_CurrentCommand = m_CurrentCommand.erase( m_CurrentCarotIndex, 1 );
        }
    }
    else
    {
        m_DeleteTimer->Lap();
    }
}


void Console::AddLogToVisual( const LogElement& logElement,
                              const Vec2& textPosition )
{
    std::string logColor = GetLogLevelColor( logElement.level );
    std::string message;
    if ( logElement.level == LOG_USER )
    {
        message = logElement.message;
    }
    else
    {
        message = logElement.GetMarkedTag() + ": " + logElement.message;
    }
    if ( m_ConsoleFont != nullptr )
    {
        m_ConsoleFont->AddVertsForMarkedText( m_LogVisual,
                                              message,
                                              textPosition,
                                              m_ConsoleLineHeight,
                                              Rgba8::WHITE );
    }
    else
    {
        AppendTextTriangles2D( m_LogVisual,
                               message,
                               textPosition,
                                m_ConsoleLineHeight,
                               Rgba8::WHITE );
    }
}

void Console::AddLogLevelToVector( std::vector<VertexMaster>& textVisual,
                                   const AABB2& overlayBounds,
                                   const Vec2& alignment,
                                   float lineHeight ) const
{
    AABB2 acceptableBounds = overlayBounds;
    acceptableBounds.mins.y += lineHeight + m_Padding.y;
    std::string logLevelString = "[" + GetLogLevelColor( m_CurrentFilter ) + GetLogLevelToString(
        m_CurrentFilter ) + "]";

    if ( m_ConsoleFont != nullptr )
    {
        m_ConsoleFont->AddVertsForMarkedTextInBox( textVisual,
                                                   logLevelString,
                                                   acceptableBounds,
                                                   lineHeight,
                                                   alignment,
                                                   Rgba8::WHITE );
    }
}

void Console::AddCommandToVector( std::vector<VertexMaster>& textVisual, const Vec2& bottomLeft,
                                  float lineHeight ) const
{
    m_ConsoleFont->AddVertsForText( textVisual, m_CurrentCommand, bottomLeft, lineHeight,
                                    Rgba8::YELLOW );
}

void Console::RenderOverlayAndText( RenderContext& renderer,
                                    const AABB2& overlayBounds ) const
{
    renderer.BindShader( m_ConsoleShader );

    // Add overlay
    std::vector<VertexMaster> background;
    AppendAABB2( background, overlayBounds, Rgba8::DARK_GRAY_75 );

    // Add console entry location
    AABB2 consoleEntry = overlayBounds.GetBoxFromBottom( 0.f, m_ConsoleLineHeight + m_Padding.y );
    AppendAABB2( background, consoleEntry, Rgba8::DARK_BLUE );

    // Add carot 
    float changeInTime = m_LastMoveTime - static_cast<float>(GetCurrentTimeSeconds());
    float carotAlpha = ClampZeroToOne( cosfDegrees( 500.f * changeInTime ) * 5.f );
    Rgba8 carotColor = Rgba8::YELLOW;
    carotColor.SetAlphaAsPercent( carotAlpha );

    ////////////////////////////////////////////////////////////////////////////////
    // TEMP HACK
    // TODO: This currently only works for monospaced fonts
    ////////////////////////////////////////////////////////////////////////////////
    Vec2 actualCarotPosition = overlayBounds.mins;
    actualCarotPosition.x += m_CurrentCarotIndex * m_ConsoleLineHeight * m_ConsoleFont->GetAspect();

    Vec2 singleCharacterSize = m_ConsoleFont->GetDimensionForText( " ", m_ConsoleLineHeight );
    AABB2 singleCharacterBox = AABB2( actualCarotPosition,
                                      actualCarotPosition + singleCharacterSize );
    AppendAABB2( background, singleCharacterBox, carotColor );

    renderer.BindTexture( nullptr );
    renderer.DrawVertexArray( background );

    // Draw the text on screen
    std::vector<VertexMaster> nonLogText;
    AddCommandToVector( nonLogText, overlayBounds.mins, m_ConsoleLineHeight );
    AddLogLevelToVector( nonLogText, overlayBounds, Vec2::ALIGN_BOTTOM_RIGHT, m_ConsoleLineHeight );

    // Render Text
    if ( m_LogVisual.empty() ) { return; } // Dont render if no text will be shown
    if ( m_ConsoleFont != nullptr )
    {
        renderer.BindTexture( m_ConsoleFont->GetTexture() );
    }
    renderer.DrawVertexArray( nonLogText );
    renderer.SetModelUBO( Mat44::CreateFromTranslation( Vec2( m_Padding.x, m_CurrentVerticalOffset + m_ConsoleLineHeight ) ) );
    renderer.DrawVertexArray( m_LogVisual );
    renderer.BindTexture( nullptr );
}

void Console::UpdateInputNoMod()
{
    InputSystem* inputSystem = &InputSystem::INSTANCE();

    if ( inputSystem->WasKeyJustPressed( LEFT_ARROW ) )
    {
        if ( m_CurrentCarotIndex > 0 )
        {
            m_CurrentCarotIndex--;
        }
    }

    if ( inputSystem->WasKeyJustPressed( RIGHT_ARROW ) )
    {
        if ( m_CurrentCarotIndex < m_CurrentCommand.length() )
        {
            m_CurrentCarotIndex++;
        }
    }
}

void Console::UpdateInputCtrl()
{
    if ( m_CurrentCommand.empty() ) { return; }

    InputSystem* inputSystem = &InputSystem::INSTANCE();

    if ( inputSystem->WasKeyJustPressed( LEFT_ARROW ) )
    {
        if ( m_CurrentCarotIndex == 0 ) { return; }

        std::string carotToEnd = m_CurrentCommand.substr( 0, m_CurrentCarotIndex - 1 );
        int nextSpace = static_cast<int>(carotToEnd.find_last_of( ' ' ));
        m_CurrentCarotIndex = nextSpace + 1;
    }

    if ( inputSystem->WasKeyJustPressed( RIGHT_ARROW ) )
    {
        if ( m_CurrentCarotIndex == m_CurrentCommand.size() ) { return; }

        std::string carotToEnd = m_CurrentCommand.substr( m_CurrentCarotIndex );
        int nextSpace = static_cast<int>(carotToEnd.find_first_of( ' ' ));
        if ( nextSpace == std::string::npos )
        {
            m_CurrentCarotIndex = static_cast<int>(m_CurrentCommand.size());
        }
        else
        {
            m_CurrentCarotIndex += nextSpace + 1;
        }
    }
}

void Console::ParseCommand()
{
    const size_t firstSpaceIndex = m_CurrentCommand.find_first_of( ' ' );
    const std::string command = m_CurrentCommand.substr( 0, firstSpaceIndex );
    const std::string args = firstSpaceIndex == std::string::npos
                                 ? ""
                                 : m_CurrentCommand.substr( firstSpaceIndex + 1 );

    NamedStrings namedArgs = NamedStrings::MakeFromString( args );

    // Log the submitted command
    const std::string submittedCommand = "> " + m_CurrentCommand;
    Log( LOG_USER, submittedCommand );

    bool foundCommand = false;
    
    for( const Command& registeredCommand : s_RegisteredCommands )
    {
        if( Stricmp( command, registeredCommand.commandName ) == 0 )
        {
            registeredCommand.FormatNamedString( namedArgs );

            NamedProperties commandArguments( namedArgs, registeredCommand.GetArgumentTypes() );
            s_CommandList->Publish( command, &commandArguments );
    
            AddToHistory( m_CurrentCommand );
    
            foundCommand = true;
            break;
        }
    }

    if ( !foundCommand )
    {
        UnknownCommand( command );
    }

    Reset();
}

void Console::AddToHistory( const std::string& nextCommand )
{
    std::string lastCommand;
    if ( !m_History.empty() )
    {
        lastCommand = m_History.at( m_History.size() - 1 );
    }
    if ( Stricmp( lastCommand, nextCommand ) != 0 )
    {
        m_History.push_back( nextCommand );
        m_HistoryIndex++;
    }
}

void Console::Reset()
{
    m_CurrentCarotIndex = 0;
    m_FrameInput.clear();
    m_CurrentCommand.clear();
    m_SubmitCommand = false;
}

void Console::UpdateLogVisualToNewState()
{
    const size_t size = m_LogVisual.size();
    m_LogVisual.clear();
    m_LogVisual.reserve( size );

    Vec2 location = Vec2::ZERO;
    for( const LogElement& log : m_Logs )
    {
        if( log.level <= m_CurrentFilter )
        {
            AddLogToVisual( log, location );
            location.y -= m_ConsoleFont->GetDimensionForMarkedText( log.message, m_ConsoleLineHeight ).y;
        }
    }
}

void Console::UpdateLogVisualToNewLineHeight( float newLineHeight )
{
    const float ratioToNew = newLineHeight / m_ConsoleLineHeight;

    m_CurrentVerticalOffset *= ratioToNew;

    TransformVertexArray( m_LogVisual, Vec3::ZERO, Vec3::ZERO, Vec3( 1.f, ratioToNew, 1.f ) );
}

Console::LogElement::LogElement( LogLevel lvl, const std::string& logMessage )
    : level( lvl )
      , message( logMessage )
{
}

std::string Console::LogElement::GetMarkedTag() const
{
    std::string marked = "[" + GetLogLevelColor( level ) + GetLogLevelToString( level ) + "]";
    return marked;
}
#endif // !defined( ENGINE_DISABLE_CONSOLE )
