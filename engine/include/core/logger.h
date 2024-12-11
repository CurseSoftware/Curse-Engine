#pragma once
#include <cstdio>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include "defines.h"
#include "platform.h"

namespace gravity {
namespace core {

namespace logger {
constexpr int BUFSIZE = 200; // bytes to allocate on the stack for writing

class Logger {
public:
    static void startup() {
        if (!Logger::instance) {
            Logger::instance = new Logger();
        } else {
            std::cout << "NO LOGGER";
            exit(1);
        }
    }

    /// @brief Shutdown the logger subsystem
    static void shutdown() {
        delete Logger::instance;
        Logger::instance = nullptr;
    }

    static Logger* get() {
        if (Logger::instance) {
            return Logger::instance;
        } else {
            exit(1);
            return nullptr;
        }
    }

    // Set the log file that we want to write the logs to
    bool set_log_file(const std::string& log_file_path) {
        m_log_file.open(log_file_path);

        if (!m_log_file.is_open()) {
            fatal("Unable to open log file: '%s'", log_file_path.c_str());
            return false;
        }

        return true;
    }

    // Whether we want to use colors or not.
    // NOTE: Colors will not be written to the log file regardless
    void use_colors(bool use_colors) {
        m_use_colors = use_colors;
    }

    // Whether we want to write to the console
    void use_console(bool use_console) {
        m_use_console = use_console;
    }


    // Avoid format compiler error
    void fatal(const char* fmt) {
        platform::color color = platform::color::RED;
        char final[BUFSIZE];
        std::snprintf(final, BUFSIZE, "[FATAL]%s\n", fmt);
        
        // If we have a log file open, write to that
        if (m_log_file.is_open()) {
            m_log_file << final;
        }

        if (m_use_console) {
            platform::Platform::get()->console_write(color, final);
        }
    }
    template <typename ...Args>
    void fatal(const char* fmt, Args&&... args) {
        platform::color color = platform::color::RED;
        char prefix[BUFSIZE];
        std::snprintf(prefix, BUFSIZE, "[FATAL]%s\n", fmt);
       
        char final[2 * BUFSIZE];
        std::snprintf(final, 2*BUFSIZE, prefix, args...);
       
        // if (m_log_file.is_open()) {
        //     m_log_file << final;
        // }

        if (m_use_console) {
            platform::Platform::get()->console_write(color, final);
        }
    }

    // Avoid format compiler error
    void error(const char* fmt) {
        platform::color color = platform::color::YELLOW;
        char final[BUFSIZE];
        std::snprintf(final, BUFSIZE, "[ERROR]%s\n", fmt);
        
        // If we have a log file open, write to that
        // if (m_log_file.is_open()) {
        //     m_log_file << final;
        // }

        if (m_use_console) {
            platform::Platform::get()->console_write(color, final);
            // platform::console_error(color, final);
        }
    }

    template <typename ...Args>
    void error(const char* fmt, Args&&... args) {
        platform::color color = platform::color::YELLOW;
        char prefix[BUFSIZE];
        std::snprintf(prefix, BUFSIZE, "[ERROR]%s\n", fmt);
       
        char final[2 * BUFSIZE];
        std::snprintf(final, 2*BUFSIZE, prefix, args...);
       
        // If we have a log file open, write to that
        // if (m_log_file.is_open()) {
        //     m_log_file << final;
        // }

        if (m_use_console) {
            platform::Platform::get()->console_write(color, final);
            // platform::console_error(color, final);
        }
    }

    // Avoid format compiler error
    void warn(const char* fmt) {
        platform::color color = platform::color::MAGENTA;
        char final[BUFSIZE];
        std::snprintf(final, BUFSIZE, "[WARN]%s\n", fmt);
        
        // If we have a log file open, write to that
        // if (m_log_file.is_open()) {
        //     m_log_file << final;
        // }

        if (m_use_console) {
            platform::Platform::get()->console_write(color, final);
            // platform::console_error(color, final);
        }
    }

    template <typename ...Args>
    void warn(const char* fmt, Args&&... args) {
        platform::color color = m_use_colors ? platform::color::MAGENTA : platform::color::NONE;
        char prefix[BUFSIZE];
        std::snprintf(prefix, BUFSIZE, "[WARN]%s\n", fmt);

        char final[2 * BUFSIZE];
        std::snprintf(final, 2*BUFSIZE, prefix, args...);
       
        // If we have a log file open, write to that
        // if (m_log_file.is_open()) {
        //     m_log_file << final;
        // }

        if (m_use_console) {
            platform::Platform::get()->console_write(color, final);
        }
    }

    // Avoid format compiler error
    void debug(const char* fmt) {
        platform::color color = platform::color::CYAN;
        char final[BUFSIZE];
        std::snprintf(final, BUFSIZE, "[DEBUG]%s\n", fmt);
        
        // If we have a log file open, write to that
        // if (m_log_file.is_open()) {
        //     m_log_file << final;
        // }

        if (m_use_console) {
            platform::Platform::get()->console_write(color, final);
        }
    }

    template <typename ...Args>
    void debug(const char* fmt, Args&&... args) {
        platform::color color = m_use_colors ? platform::color::CYAN : platform::color::NONE;
        char prefix[BUFSIZE];
        std::snprintf(prefix, BUFSIZE, "[DEBUG]%s\n", fmt);
       
        // weird bullshit to avoid format compiler warnings
        if (sizeof ...(Args) == 0) {
            // if (m_log_file.is_open()) {
            //     m_log_file << prefix;
            // }

            if (m_use_console) {
                platform::Platform::get()->console_write(color, prefix);
            }
            return;
        }

        char final[2 * BUFSIZE];
        std::snprintf(final, 2*BUFSIZE, prefix, args...);
       
        // If we have a log file open, write to that
        if (m_log_file.is_open()) {
            try {
                m_log_file << final;
            } catch (auto err) {

            }
        }

        if (m_use_console) {
            platform::Platform::get()->console_write(color, final);
        }
    }

    // Avoid format compiler error
    void info(const char* fmt) {
        platform::color color = platform::color::GREEN;
        char final[BUFSIZE];
        std::snprintf(final, BUFSIZE, "[INFO]%s\n", fmt);
        
        // If we have a log file open, write to that
        // if (m_log_file.is_open()) {
        //     m_log_file << final;
        // }

        if (m_use_console) {
            platform::Platform::get()->console_write(color, final);
        }
    }

    template <typename ...Args>
    void info(const char* fmt, Args&&... args) {
        platform::color color = m_use_colors ? platform::color::GREEN : platform::color::NONE;
        char prefix[BUFSIZE];
        std::snprintf(prefix, BUFSIZE, "[INFO]%s\n", fmt);

        char final[2 * BUFSIZE];
        std::snprintf(final, 2*BUFSIZE, prefix, args...);
       
        // If we have a log file open, write to that
        // if (m_log_file.is_open()) {
        //     m_log_file << final;
        // }

        if (m_use_console) {
            platform::Platform::get()->console_write(color, final);
        }
    }

    // Avoid format compiler error
    void trace(const char* fmt) {
        platform::color color = platform::color::BLUE;
        char final[BUFSIZE];
        std::snprintf(final, BUFSIZE, "[TRACE]%s\n", fmt);
        
        // If we have a log file open, write to that
        // if (m_log_file.is_open()) {
        //     m_log_file << final;
        // }

        if (m_use_console) {
            platform::Platform::get()->console_write(color, final);
        }
    }

    template <typename ...Args>
    void trace(const char* fmt, Args&&... args) {
        platform::color color = m_use_colors ? platform::color::BLUE : platform::color::NONE;

        char prefix[BUFSIZE];
        std::snprintf(prefix, BUFSIZE, "[TRACE]%s\n", fmt);

        char final[2 * BUFSIZE];
        std::snprintf(final, 2*BUFSIZE, prefix, args...);
        
        // If we have a log file open, write to that
        // if (m_log_file.is_open()) {
        //     m_log_file << final;
        // }

        if (m_use_console) {
            platform::Platform::get()->console_write(color, final);
        }
    }
private:
    Logger()
    : m_use_colors(true)
    , m_use_console(true)
    {}

    ~Logger() {
        m_log_file.close();
    }

    std::ofstream m_log_file; // the file stream for the output file
    bool m_use_colors;        // whether to use colors when logging
    bool m_use_console;       // whether to write to the console when logging

    template <typename ...Args>
    std::string format_string(const char* fmt, Args&& ...args) {
        int size = std::snprintf(nullptr, 0, fmt, args...) + 1;

        // TODO: account for formatting error here

        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, fmt, args...);

        return std::string(buf.get(), buf.get() + size - 1);
    }

    static Logger* instance;
};


} // logger namespace
} // core namespace
} // gravity namespace 