#include "video_processing.hpp"
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
extern "C" {
#endif

#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <inttypes.h>
#include <cstdlib>
#ifdef __cplusplus
}
#endif
#include <iostream>
#include <sstream>


namespace video_processing {
  const int64_t MS_IN_SECOND{1000};

  std::optional<std::string> extract_video_segment_from_beginning(std::string &doc_root, request_processing::cut_video_request &request) {
    std::string path_to_input_file{doc_root+request.path_to_video};
    std::cout << "input_path: " << path_to_input_file << std::endl;
    const char* input_file_name{path_to_input_file.c_str()};

    std::stringstream output_file_constructor;
    output_file_constructor << doc_root << "out_" << request.video_start_point_ms << "_" << request.video_duration_ms << "." <<request.video_format;
    std::string path_to_output_file;
    output_file_constructor >> path_to_output_file;
    const char* output_file_name{path_to_output_file.c_str()};
    std::cout << "output_path: " << output_file_name << std::endl;
    
    int64_t requested_duration_ms{request.video_duration_ms};
    
    {
      AVPacket *test_packet{av_packet_alloc()};
      if (test_packet == nullptr) {
          std::cerr << "Couldn't allocate AVPacket!\n";
          return std::nullopt;
      } else {
          // Освобождаем память, выделенную под сжатый фрейм
          av_packet_free(&test_packet);
      }
    }
    
    AVFormatContext *input_video_format_context{nullptr};
    // Считываем заголовок из первого видеофайла
    if (avformat_open_input(&input_video_format_context, input_file_name, 0, 0) < 0) {
        std::cerr << "Couldn't open input file " << input_file_name << std::endl;
        return std::nullopt;
    }

    // Извлекаем данные о потоках из видеофайла
    if (avformat_find_stream_info(input_video_format_context, 0) < 0) {
        std::cerr << "Failed to retrieve input stream information\n";
        return std::nullopt;
    }

    // Выводим метаданные о входном видеофайле
    av_dump_format(input_video_format_context, 0, input_file_name, 0);

    AVFormatContext *output_video_format_context{nullptr};
    // Выделяем память для информации о выходном видео файле
    avformat_alloc_output_context2(&output_video_format_context, nullptr, nullptr, output_file_name);
    if (output_video_format_context == nullptr) {
        std::cerr << "Couldn't create output context\n";
        return std::nullopt;
    }

    
    int32_t stream_mapping_size{input_video_format_context->nb_streams};
    int32_t *stream_mapping{static_cast<int32_t*>(av_calloc(stream_mapping_size, sizeof(int32_t*)))};
    if (stream_mapping == nullptr) {
        std::cerr << "Couldn't allocate memory for stream mapping\n";
        return std::nullopt;
    }

    bool *stream_has_been_cut{static_cast<bool*>(av_calloc(stream_mapping_size, sizeof(bool*)))};

    const AVOutputFormat *output_container_format{output_video_format_context->oformat};
    int32_t stream_index{0};
    for (int32_t current_stream_index{0}; current_stream_index < input_video_format_context->nb_streams; ++current_stream_index) {
        
        AVStream *in_stream{input_video_format_context->streams[current_stream_index]};
        AVCodecParameters *in_codec_parameters{in_stream->codecpar};

        if (in_codec_parameters->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codec_parameters->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codec_parameters->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            stream_mapping[current_stream_index] = -1;
            continue;
        }

        stream_mapping[current_stream_index] = stream_index++;
        stream_has_been_cut[current_stream_index] = false;

        AVStream *out_stream{avformat_new_stream(output_video_format_context, nullptr)};
        if (out_stream == nullptr) {
            std::cerr << "Failed allocating output stream\n";
            return std::nullopt;
        }
        if (avcodec_parameters_copy(out_stream->codecpar, in_codec_parameters) < 0) {
            std::cerr << "Failed to copy codec parameters\n";
        }
        out_stream->codecpar->codec_tag = 0;
    }

    // Выводим метаданные о выходном файле
    av_dump_format(output_video_format_context, 0, output_file_name, 1);

    // for (int64_t i{0}; i < stream_mapping_size; ++i) {
    //     std::cout << stream_mapping[i] << " ";
    // }
    // std::cout << std::endl;

    if (!(output_container_format->flags & AVFMT_NOFILE)) {
        if (avio_open(&output_video_format_context->pb, output_file_name, AVIO_FLAG_WRITE) < 0) {
            std::cerr << "Couldn't open output file " << output_file_name;
            return std::nullopt;
        }
    }

    // Пишем в выходной файл заголовки
    if (avformat_write_header(output_video_format_context, nullptr) < 0) {
        std::cerr << "Error occured when opening output file\n";
        return std::nullopt;
    }

    AVPacket *current_packet{av_packet_alloc()};
    if (current_packet == nullptr) {
        std::cerr << "Couldn't allocate AVPacket\n";
        return std::nullopt;
    }
    
    int64_t start_time_ms{input_video_format_context->start_time};
    while (true) {
      // Проверяем, не достигли ли мы конца видео контейнера
      if (av_read_frame(input_video_format_context, current_packet) < 0) {
        break;
      }

      if (stream_has_been_cut[current_packet->stream_index]) {
        continue;
      }

      AVStream *in_stream{input_video_format_context->streams[current_packet->stream_index]};
      std::cout << "current stream index: " << current_packet->stream_index << std::endl;
      // Видимо, избавляемся от битых пакетов
      if (current_packet->stream_index >= stream_mapping_size || 
        stream_mapping[current_packet->stream_index] < 0) {
          av_packet_unref(current_packet);
          continue;
      }

      // Обрезаем видео
      AVRational time_base{in_stream->time_base};
      int64_t pts_time_ms{(static_cast<double>(current_packet->pts - in_stream->start_time)*time_base.num/time_base.den)*MS_IN_SECOND};
      std::cout << "pts_time_ms: " << pts_time_ms << std::endl;

      if (pts_time_ms >= requested_duration_ms) {
          stream_has_been_cut[current_packet->stream_index] = true;
          std::cout << "Stream: " << current_packet->stream_index << " has been cut!\n";
      }


      current_packet->stream_index = stream_mapping[current_packet->stream_index];
      AVStream *out_stream{output_video_format_context->streams[current_packet->stream_index]};

      av_packet_rescale_ts(current_packet, in_stream->time_base, out_stream->time_base);
      current_packet->pos = -1;
      
      if (av_interleaved_write_frame(output_video_format_context, current_packet) < 0) {
          std::cerr << "Error muxing packet\n";
          break;
      }
    }
    av_write_trailer(output_video_format_context);
    av_packet_free(&current_packet);
    avformat_close_input(&input_video_format_context);

    if (output_video_format_context != nullptr && 
        !(output_container_format->flags & AVFMT_NOFILE)) {
        avio_closep(&output_video_format_context->pb);
    }
    avformat_free_context(output_video_format_context);

    av_freep(&stream_mapping);
    av_freep(&stream_has_been_cut);

    return path_to_output_file;
  }
}