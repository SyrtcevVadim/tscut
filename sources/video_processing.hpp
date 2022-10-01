#pragma once
#include "cut_video_request.hpp"
#include <string>
#include <optional>

namespace video_processing {
  std::optional<std::string> extract_video_segment_from_beginning(std::string &doc_root, request_processing::cut_video_request &request);
}