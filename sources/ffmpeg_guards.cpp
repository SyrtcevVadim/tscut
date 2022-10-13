#include "ffmpeg_guards.hpp"

namespace ffmpeg {
AVPacketWrapper::AVPacketWrapper() { packet = av_packet_alloc(); }

AVPacketWrapper::~AVPacketWrapper() {
    if (packet != nullptr) {
        av_packet_free(&packet);
    }
}

AVPacket* AVPacketWrapper::get_ptr() { return packet; }

bool AVPacketWrapper::is_valid() { return (packet != nullptr) ? true : false; }

AVPacket* AVPacketWrapper::operator->() { return packet; }

AVInputFormatContextWrapper::AVInputFormatContextWrapper(
    std::string& path_to_input_file) {
    this->path_to_input_file = path_to_input_file;
    avformat_open_input(&input_context, path_to_input_file.c_str(), 0, 0);
}

AVInputFormatContextWrapper::~AVInputFormatContextWrapper() {
    if (input_context != nullptr) {
        avformat_close_input(&input_context);
    }
}

AVFormatContext* AVInputFormatContextWrapper::get_ptr() {
    return input_context;
}

bool AVInputFormatContextWrapper::is_valid() {
    return (input_context != nullptr) ? true : false;
}

void AVInputFormatContextWrapper::dump_info_to_console() {
    av_dump_format(input_context, 0, path_to_input_file.c_str(), 0);
}

AVFormatContext* AVInputFormatContextWrapper::operator->() {
    return input_context;
}

AVOutputFormatContextWrapper::AVOutputFormatContextWrapper(
    std::string path_to_output_file) {
    this->path_to_output_file = path_to_output_file;
    avformat_alloc_output_context2(&output_context, nullptr, nullptr,
                                   path_to_output_file.c_str());
}

AVOutputFormatContextWrapper::~AVOutputFormatContextWrapper() {
    // Если выходной файл был создан, освобождаем поток вывода данных
    if (output_context != nullptr) {
        if (!output_context->oformat->flags & AVFMT_NOFILE) {
            avio_closep(&output_context->pb);
        }
        avformat_free_context(output_context);
    }
}

AVFormatContext* AVOutputFormatContextWrapper::get_ptr() {
    return output_context;
}

bool AVOutputFormatContextWrapper::is_valid() {
    return (output_context != nullptr) ? true : false;
}

bool AVOutputFormatContextWrapper::write_header(AVDictionary** options) {
    return avformat_write_header(output_context, options) >= 0;
}

bool AVOutputFormatContextWrapper::write_trailer() {
    return av_write_trailer(output_context) >= 0;
}

void AVOutputFormatContextWrapper::dump_info_to_console() {
    av_dump_format(output_context, 0, path_to_output_file.c_str(), 1);
}

AVFormatContext* AVOutputFormatContextWrapper::operator->() {
    return output_context;
}
} // namespace ffmpeg
