#include "video_processing.hpp"
#include "ffmpeg_guards.hpp"
#include "ffmpeg_headers.hpp"
#include "logger.hpp"

#include <algorithm>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

using namespace ffmpeg;

namespace video_processing {
const int64_t MS_IN_SECOND{1000};
/*
По какой-то причине при обрезании видео оно получается примерно на 60мс длиннее,
чем надо Я буду учитывать эту величину при обрезании видео
*/
const int64_t EXTRA_VIDEO_DURATION_MS{60};

std::optional<std::string> extract_video_segment_from_beginning(
    std::string& doc_root, request_processing::cut_video_request& request) {
    std::string path_to_input_file{doc_root + request.path_to_video};

    std::stringstream output_file_constructor;
    output_file_constructor
        << doc_root << "out_" << request.video_start_point_ms << "_"
        << request.video_duration_ms << "." << request.video_format;
    std::string path_to_output_file;
    output_file_constructor >> path_to_output_file;

    int64_t requested_duration_ms{request.video_duration_ms};

    // Проверяем, может ли система выделить память для одного сжатого фрейма ??
    {
        AVPacketWrapper test_packet;
        if (!test_packet.is_valid()) {
            Logger::get_instance().err(
                R"(Couldn't allocate AVPacket while processing "{}" video)",
                request.path_to_video);
            return std::nullopt;
        }
    }

    // Считываем заголовок из входного видеофайла
    AVInputFormatContextWrapper input_video_context{path_to_input_file};
    if (!input_video_context.is_valid()) {
        Logger::get_instance().err(R"(Couldn't open input file: "{}")",
                                   path_to_input_file);
        return std::nullopt;
    }

    // Извлекаем данные о потоках из видеофайла
    if (avformat_find_stream_info(input_video_context.get_ptr(), 0) < 0) {
        Logger::get_instance().err(
            R"(Failed to retrieve input stream information while processing "{}" video)",
            request.path_to_video);
        return std::nullopt;
    }

    // Выводим метаданные о входном видеофайле
    // input_video_context.dump_info_to_console();

    AVOutputFormatContextWrapper output_video_context{path_to_output_file};
    if (!output_video_context.is_valid()) {
        Logger::get_instance().err("Couldn't create output context");
        return std::nullopt;
    }

    uint32_t stream_mapping_size{input_video_context->nb_streams};
    std::vector<int32_t> stream_mapping(stream_mapping_size, 0);

    std::vector<bool> stream_has_been_cut(stream_mapping_size, false);

    int32_t stream_index{0};
    for (int32_t current_stream_index{0};
         current_stream_index < stream_mapping.size(); ++current_stream_index) {

        AVStream* in_stream{input_video_context->streams[current_stream_index]};
        AVCodecParameters* in_codec_parameters{in_stream->codecpar};

        if (in_codec_parameters->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codec_parameters->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codec_parameters->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            stream_mapping[current_stream_index] = -1;
            continue;
        }

        stream_mapping[current_stream_index] = stream_index++;
        stream_has_been_cut[current_stream_index] = false;

        AVStream* out_stream{
            avformat_new_stream(output_video_context.get_ptr(), nullptr)};
        if (out_stream == nullptr) {
            Logger::get_instance().err("Failed to allocate output stream");
            return std::nullopt;
        }
        if (avcodec_parameters_copy(out_stream->codecpar, in_codec_parameters) <
            0) {
            Logger::get_instance().err("Failed to copy codec parameters");
        }
        out_stream->codecpar->codec_tag = 0;
    }

    // Выводим метаданные о выходном файле
    // output_video_context.dump_info_to_console();

    if (!(output_video_context->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&output_video_context->pb, path_to_output_file.c_str(),
                      AVIO_FLAG_WRITE) < 0) {
            Logger::get_instance().err(R"(Couldn't open output file: "{}")",
                                       path_to_output_file);
            return std::nullopt;
        }
    }

    // Пишем в выходной файл заголовки
    if (!output_video_context.write_header(nullptr)) {
        Logger::get_instance().err("Error occured while openning output file");
        return std::nullopt;
    }

    AVPacketWrapper current_packet;
    if (!current_packet.is_valid()) {
        Logger::get_instance().err("Couldn't allocate AVPacket");
        return std::nullopt;
    }

    int64_t start_time_ms{input_video_context.get_ptr()->start_time};
    while (true) {
        // Проверяем, не достигли ли мы конца видео контейнера
        if (av_read_frame(input_video_context.get_ptr(),
                          current_packet.get_ptr()) < 0) {
            break;
        }

        // Проверяем, обрезаны ли все потоки. Если так, обработка оставшегося
        // видео потока не имеет смысла
        if (std::find(stream_has_been_cut.begin(), stream_has_been_cut.end(),
                      false) == stream_has_been_cut.end()) {
            break;
        }

        // Не обрабатываем пакеты потока, который уже был обрезан
        if (stream_has_been_cut[current_packet->stream_index]) {
            continue;
        }

        AVStream* in_stream{
            input_video_context->streams[current_packet->stream_index]};

        // Видимо, избавляемся от битых пакетов
        if (current_packet->stream_index >= stream_mapping_size ||
            stream_mapping[current_packet->stream_index] < 0) {
            av_packet_unref(current_packet.get_ptr());
            continue;
        }

        // Обрезаем видео
        AVRational time_base{in_stream->time_base};
        int64_t pts_time_ms{
            (static_cast<double>(current_packet->pts - in_stream->start_time) *
             time_base.num / time_base.den) *
                MS_IN_SECOND +
            EXTRA_VIDEO_DURATION_MS};

        if (pts_time_ms >= requested_duration_ms) {
            stream_has_been_cut[current_packet->stream_index] = true;
        }

        current_packet->stream_index =
            stream_mapping[current_packet->stream_index];
        AVStream* out_stream{
            output_video_context->streams[current_packet->stream_index]};

        av_packet_rescale_ts(current_packet.get_ptr(), in_stream->time_base,
                             out_stream->time_base);
        current_packet->pos = -1;

        if (av_interleaved_write_frame(output_video_context.get_ptr(),
                                       current_packet.get_ptr()) < 0) {
            Logger::get_instance().err("Error has occured while muxing packet");
            break;
        }
    }
    output_video_context.write_trailer();

    return path_to_output_file;
}
} // namespace video_processing