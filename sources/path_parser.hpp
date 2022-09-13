#pragma once

#include <boost/regex.hpp>
#include <string>
#include "cut_video_request.hpp"

namespace video_processing 
{
  bool parse_cut_video_request(std::string &request_path, cut_video_request &cut_video_request);
}
