#pragma once
#include "cut_video_request.hpp"

namespace video_processing {
  void cut_video(std::string &doc_root, request_processing::cut_video_request &request);
}