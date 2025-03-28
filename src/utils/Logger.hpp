#pragma once
#include <string>
#include <string_view>
#include <filesystem>
#include <memory>
#include <source_location>
#include <format>
#include <chrono>
#include <cstdio>
#include <stdexcept>
#include <Windows.h>

#include <SDL3/SDL_log.h>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

class Logger {
public:
    static Logger& GetInstance();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    static void SDLLogOutputFunction(void* userdata, int category, SDL_LogPriority priority, const char* message);
    void InitializeSDLLogging();

    template<typename... Args>
    void Debug(std::format_string<Args...> fmt, Args&&... args);

    template<typename... Args>
    void Info(std::format_string<Args...> fmt, Args&&... args);

    template<typename... Args>
    void Warning(std::format_string<Args...> fmt, Args&&... args);

    template<typename... Args>
    void Error(std::format_string<Args...> fmt, Args&&... args);

    template<typename... Args>
    void Critical(std::format_string<Args...> fmt, Args&&... args);

    // �ʱ�ȭ �� ����
    bool Initialize(const std::filesystem::path& logDir = "logs");
    void Shutdown();

    // ����
    void SetLogToConsole(bool enable) { log_to_console_ = enable; }
    void SetLogToFile(bool enable) { log_to_file_ = enable; }
    void SetLogToDebugger(bool enable) { log_to_debugger_ = enable; }
    void SetLogLevel(LogLevel level) { log_level_ = level; }

    // SDL �α� ������ �´� �Լ���
    void SDLLogVerbose(int category, const char* fmt, ...);
    void SDLLogDebug(int category, const char* fmt, ...);
    void SDLLogInfo(int category, const char* fmt, ...);
    void SDLLogWarn(int category, const char* fmt, ...);
    void SDLLogError(int category, const char* fmt, ...);
    void SDLLogCritical(int category, const char* fmt, ...);

private:
    Logger() = default;
    ~Logger() = default;

    // ���� �α� ����
    template<typename... Args>
    void Log(LogLevel level,
        const std::source_location& location,
        std::format_string<Args...> fmt,
        Args&&... args);

    // �α� ���� ����
    void RotateLogFiles();
    [[nodiscard]] std::filesystem::path GetCurrentLogFilePath() const;

    // ��ƿ��Ƽ �Լ�
    [[nodiscard]] static std::string GetTimestamp();
    [[nodiscard]] static std::string_view LogLevelToString(LogLevel level);

    // ��� ����
    std::filesystem::path log_directory_;
    std::unique_ptr<std::FILE, decltype(&std::fclose)> current_log_file_{ nullptr, std::fclose };

    LogLevel log_level_{ LogLevel::Info };
    bool log_to_console_{ true };
    bool log_to_file_{ true };
    bool log_to_debugger_{ true };

    static constexpr size_t MAX_LOG_FILES = 5;
    static constexpr size_t MAX_LOG_SIZE = 10 * 1024 * 1024; // 10MB
};

// ���ø� ����
template<typename... Args>
void Logger::Debug(std::format_string<Args...> fmt, Args&&... args) 
{
    Log(LogLevel::Debug, std::source_location::current(), fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Info(std::format_string<Args...> fmt, Args&&... args) 
{
    Log(LogLevel::Info, std::source_location::current(), fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Warning(std::format_string<Args...> fmt, Args&&... args) 
{
    Log(LogLevel::Warning, std::source_location::current(), fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Error(std::format_string<Args...> fmt, Args&&... args) 
{
    Log(LogLevel::Error, std::source_location::current(), fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Critical(std::format_string<Args...> fmt, Args&&... args) 
{
    Log(LogLevel::Critical, std::source_location::current(), fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Log(LogLevel level,
    const std::source_location& location,
    std::format_string<Args...> fmt,
    Args&&... args) 
{
    if (level < log_level_)
    {
        return;
    }

    try {
        // �α� �޽��� ����
        std::string message = std::format("[{}] [{}] [{}:{}] {}\n",
            GetTimestamp(),
            LogLevelToString(level),
            location.file_name(),
            location.line(),
            std::format(fmt, std::forward<Args>(args)...));

        // �ܼ� ���
        if (log_to_console_) 
        {
            std::printf("%s", message.c_str());
            std::fflush(stdout);
        }

        // ���� ���
        if (log_to_file_ && current_log_file_) 
        {
            std::fprintf(current_log_file_.get(), "%s", message.c_str());
            std::fflush(current_log_file_.get());
        }

        // ����� ���
        if (log_to_debugger_) 
        {
            OutputDebugStringA(message.c_str());
        }
    }
    catch (const std::exception& e) 
    {
        // �α� ���� �� �ּ����� ���� ���
        std::fprintf(stderr, "Logging failed: %s\n", e.what());
    }
}

// �۷ι� �׼��� ��ũ��
#define LOGGER Logger::GetInstance()

// ���Ǽ� ��ũ��
#define LOG_DEBUG(...) LOGGER.Debug(__VA_ARGS__)
#define LOG_INFO(...) LOGGER.Info(__VA_ARGS__)
#define LOG_WARNING(...) LOGGER.Warning(__VA_ARGS__)
#define LOG_ERROR(...) LOGGER.Error(__VA_ARGS__)
#define LOG_CRITICAL(...) LOGGER.Critical(__VA_ARGS__)

#define SDL_LOG_VERBOSE(category, ...) LOGGER.SDLLogVerbose(category, __VA_ARGS__)
#define SDL_LOG_DEBUG(category, ...) LOGGER.SDLLogDebug(category, __VA_ARGS__)
#define SDL_LOG_INFO(category, ...) LOGGER.SDLLogInfo(category, __VA_ARGS__)
#define SDL_LOG_WARN(category, ...) LOGGER.SDLLogWarn(category, __VA_ARGS__)
#define SDL_LOG_ERROR(category, ...) LOGGER.SDLLogError(category, __VA_ARGS__)
#define SDL_LOG_CRITICAL(category, ...) LOGGER.SDLLogCritical(category, __VA_ARGS__)