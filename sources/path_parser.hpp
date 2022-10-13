#pragma once

#include "cut_video_request.hpp"
#include <boost/regex.hpp>
#include <string>

namespace request_processing {
bool parse_cut_video_request(std::string& request_path,
                             cut_video_request& cut_video_request);
}
