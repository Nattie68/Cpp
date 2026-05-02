#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

enum LogLevel { LOG_NORMAL, LOG_WARNING, LOG_ERROR };

class Log {
private:
    struct LogEntry {
        std::string timestamp;
        LogLevel level;
        std::string message;
    };

    std::vector<LogEntry> entries;
    static const size_t MAX_ENTRIES = 10;

    Log() = default;

    std::string currentTime() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

public:
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    static Log* Instance() {
        static Log instance;
        return &instance;
    }

    void message(LogLevel level, const std::string& msg) {
        entries.push_back({currentTime(), level, msg});
        if (entries.size() > MAX_ENTRIES) {
            entries.erase(entries.begin());
        }
    }

    void print() const {
        for (const auto& entry : entries) {
            std::string levelStr;
            switch (entry.level) {
                case LOG_NORMAL: levelStr = "NORMAL"; break;
                case LOG_WARNING: levelStr = "WARNING"; break;
                case LOG_ERROR: levelStr = "ERROR"; break;
            }
            std::cout << "[" << entry.timestamp << "] " << levelStr << ": " << entry.message << std::endl;
        }
    }
};
