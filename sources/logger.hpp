#pragma once
#include <mutex>
#include <filesystem>
#include <string_view>
#include <fstream>

#include <fmt/format.h>
#include <fmt/std.h>
#include <fmt/chrono.h>

#include "singleton.hpp"


class Logger: public SingletonBase<Logger> {
    enum class MessageTag {
        Info, Warn, Err
    };

public:
    template<class... Args>
    void info(std::string_view fmt, Args... args) {
        log(MessageTag::Info, fmt::vformat(fmt, fmt::make_format_args(args...)));
    }

    template<class... Args>
    void warn(std::string_view fmt, Args... args) {
        log(MessageTag::Warn, fmt::vformat(fmt, fmt::make_format_args(args...)));
    }

    template<class... Args>
    void err(std::string_view fmt, Args... args) {
        log(MessageTag::Err, fmt::vformat(fmt, fmt::make_format_args(args...)));
    }

    void set_log_file_path(std::string &log_file_path) {
        const std::lock_guard<std::mutex> lock(_mutex);
        this->log_file_path = log_file_path;
    }

private:
    void log(MessageTag tag, std::string_view message) {
        const std::lock_guard<std::mutex> lock(_mutex);
        std::ofstream log_file(log_file_path, std::ios_base::app);
        /*
            Формат вывода логгирующих сообщений:
            [yyyy-mm-dd hh:MM:ss.ms] [thread id] [log tag] <message> \n
        */
        log_file << fmt::format("[{:%Y-%m-%d %R:%S}] [{}] [{}] {}",
            std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()),
            std::this_thread::get_id(),
            stringify_tag(tag),
            message) << '\n';
        log_file.close();
    }

    std::string stringify_tag(MessageTag tag) {
        switch (tag) {
            case MessageTag::Info: {
                return "INFO";
            }
            case MessageTag::Warn: {
                return "WARN";
            }
            case MessageTag::Err: {
                return "ERR";
            }
            default: {
                return "UNKNOWN";
            }
        }
    }

protected:
    std::mutex _mutex;
    std::filesystem::path log_file_path;

    Logger(): SingletonBase() {
        log_file_path = "tscut.log";
    }

    friend class SingletonBase<Logger>;
};