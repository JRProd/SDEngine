#pragma once

#include "Engine/Core/Time/Timer.hpp"
#include "Engine/Event/EventSystem.hpp"

#include <string>
#include <vector>
#include "Engine/Renderer/Shaders/Shader.hpp"


#include "Command.hpp"

struct VertexMaster;
struct AABB2;
class BitmapFont;
class Camera;
class RenderContext;

enum LogLevel
{
    LOG_USER,
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARNING,
    LOG_LOG,
    LOG_INFO,
    LOG_VERBOSE,
};

enum class ConsoleView
{
    FULL,
    BOTTOM_HALF,
    TOP_HALF,
    CLOSED,
};


class Console
{
public:
    Console();
    ~Console() = default;

    static void RegisterCommand( const std::string& commandName, EventCallback* callbackFunction );
    static void RegisterCommand( const std::string& commandName, const std::string& commandDescription, EventCallback* callbackFunction );
    static void RegisterCommand( const Command& command, EventCallback* callbackFunction );

    template <typename ObjectType>
    static void RegisterCommand( const std::string& commandName, ObjectType* object, bool( ObjectType::* methodCallback )( EventArgs* ) );
    template <typename ObjectType>
    static void RegisterCommand( const std::string& commandName, const std::string& commandDescription, ObjectType* object, bool( ObjectType::* methodCallback )( EventArgs* ) );
    template <typename ObjectType>
    static void RegisterCommand( const Command& command, ObjectType* object, bool( ObjectType::* methodCallback )( EventArgs* ) );

    static bool CommandHelp( EventArgs* args );

    void Startup();
    void BeginFrame();
    void EndFrame();
    void Shutdown();

    void UpdateInput();
    void Update( float deltaSeconds );

    //-------------------------------------------------------------------------
    // Logging Functions
    void Log( LogLevel level, const std::string& log, bool sanitize = true );
    __declspec(noreturn) void LogWTF( const std::string& log);
    void GuaranteeOrLog( bool condition, LogLevel level, const std::string& log );
    void GuaranteeOrWTF( bool condition, const std::string& log );

    //-------------------------------------------------------------------------
    // Rendering Functions
    void Render( RenderContext& renderer, 
                 const Camera& camera);
    void RenderFilter( RenderContext& renderer,
                       const Camera& camera,
                       LogLevel levelFilter);

    bool IsActive() const { return m_IsActive; }

    void Toggle();
    void SetActive( bool active );

    void SetConsoleView( ConsoleView newView );
    void IncrementConsoleView();
    void SetConsoleLogLevel( LogLevel newLevel );
    void IncrementConsoleLogLevel();
    void SetConsoleText( BitmapFont* newFont );

    void SetLineHeight( float lineHeight );

    void UnknownCommand( const EventName& command );
    void InvalidArgument( const EventName& command, const std::string& description );

private:
    struct LogElement
    {
        LogLevel level;
        std::string message;
        

    public:
        LogElement( LogLevel lvl, const std::string& logMessage );

        std::string GetMarkedTag() const;
    };

    static EventSystem* s_CommandList;
    static std::vector<Command> s_RegisteredCommands;

    Shader* m_ConsoleShader = nullptr;
    bool m_IsFinalized = false;

    float m_LastMoveTime = 0.f;
    Timer* m_DeleteTimer = nullptr;
    unsigned int m_CurrentCarotIndex = 0;
    std::string m_FrameInput = "";
    std::string m_CurrentCommand = "";
    bool m_SubmitCommand = false;
    std::vector<std::string> m_History;
    int m_HistoryIndex = 0;

    Vec2 m_Padding = Vec2( 5.f, 5.f );
    std::vector<LogElement> m_Logs;
    std::vector<VertexMaster> m_LogVisual;
    size_t m_TotalLinesLogged = 0u;
    float m_CurrentVerticalOffset = 0.f;

    BitmapFont* m_ConsoleFont = nullptr;
    float m_ConsoleLineHeight = 24.f;
    bool m_ConsoleFontError = false;

    ConsoleView m_ConsoleView = ConsoleView::CLOSED;
    LogLevel m_CurrentFilter = LOG_INFO;

    bool m_IsActive = false;

    void AddLogToVisual( const LogElement& logElement,
                         const Vec2& textPosition);

    void AddLogLevelToVector( std::vector<VertexMaster>& textVisual,
                              const AABB2& overlayBounds,
                              const Vec2& alignment,
                              float lineHeight ) const;

    void AddCommandToVector( std::vector<VertexMaster>& textVisual,
                             const Vec2 & bottomLeft,
                             float lineHeight ) const;

    void RenderOverlayAndText( RenderContext& renderer,
                               const AABB2& overlayBounds ) const;

    void UpdateInputNoMod();
    void UpdateInputCtrl();
    void ParseCommand();
    void AddToHistory( const std::string& nextCommand );
    void Reset();

    void UpdateLogVisualToNewState();
    void UpdateLogVisualToNewLineHeight( float newLineHeight );
};

template <typename ObjectType>
void Console::RegisterCommand( const std::string& commandName, ObjectType* object, bool( ObjectType::* methodCallback )( EventArgs* ) )
{
    RegisterCommand( commandName, "", object, methodCallback );
}

template <typename ObjectType>
void Console::RegisterCommand( const std::string& commandName, const std::string& commandDescription, ObjectType* object, bool( ObjectType::* methodCallback )( EventArgs* ) )
{
    Command cmd;
    cmd.commandName = commandName;
    cmd.description = commandDescription;

    RegisterCommand( cmd, object, methodCallback );
}

template <typename ObjectType>
void Console::RegisterCommand( const Command& command, ObjectType* object, bool( ObjectType::* methodCallback )( EventArgs* ) )
{
    s_CommandList->SubscribeUnique( command.commandName, object, methodCallback );
    s_RegisteredCommands.push_back( command );
}