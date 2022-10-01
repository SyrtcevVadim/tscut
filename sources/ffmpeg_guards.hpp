#pragma once
#include "ffmpeg_headers.hpp"
#include <string>

namespace ffmpeg {
  class AVPacketWrapper {
  private:
    AVPacket *packet{nullptr};

  public:
    AVPacketWrapper();

    ~AVPacketWrapper();

    AVPacket *get_ptr();
    
    bool is_valid();

    AVPacket* operator->();
  };

  class AVInputFormatContextWrapper {
  private:
    AVFormatContext *input_context{nullptr};
    std::string path_to_input_file;

  public:
    AVInputFormatContextWrapper(std::string &path_to_input_file);
    ~AVInputFormatContextWrapper();

    AVFormatContext *get_ptr();

    bool is_valid();

    void dump_info_to_console();

    AVFormatContext* operator->();
  };

  class AVOutputFormatContextWrapper {
  private:
    AVFormatContext *output_context{nullptr};
    std::string path_to_output_file;

  public:
    AVOutputFormatContextWrapper(std::string path_to_output_file);

    ~AVOutputFormatContextWrapper();

    AVFormatContext *get_ptr();

    bool is_valid();

    bool write_header(AVDictionary **options);

    bool write_trailer();

    void dump_info_to_console();

    AVFormatContext* operator->();
  };
}

