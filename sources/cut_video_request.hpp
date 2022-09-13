#pragma once

#include<string>
#include<cstdint>

namespace video_processing
{
  struct cut_video_request
  {
    std::string video_format;
    std::string path_to_video;
    uint64_t video_start_point_ms;
    uint64_t video_length_ms;
  };
}
