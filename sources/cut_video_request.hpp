#pragma once

#include <cstdint>
#include <string>

namespace request_processing {
/// @brief Запрос на обрезку видео
struct cut_video_request {
    /// @brief Формат контейнера видео
    std::string video_format;
    /// @brief Путь к видео-файлу на сервере
    std::string path_to_video;
    /// @brief Начальное время в миллисекундах, начиная с которого нужно
    /// вырезать часть видео
    uint64_t video_start_point_ms;
    /// @brief Длительность видео в миллисекундах
    uint64_t video_duration_ms;
};
} // namespace request_processing
