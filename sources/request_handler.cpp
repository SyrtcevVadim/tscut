//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_handler.hpp"
#include "mime_types.hpp"
#include "path_parser.hpp"
#include "reply.hpp"
#include "request.hpp"
#include "video_processing.hpp"
#include "logger.hpp"

#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace http {
namespace server {

request_handler::request_handler(const std::string& doc_root)
    : doc_root_(doc_root) {}

void request_handler::handle_request(const request& req, reply& rep) {
    // Decode url to path.
    std::string request_path;
    if (!url_decode(req.uri, request_path)) {
        Logger::get_instance().err("Requested uri hadn't been decoded properly!");
        rep = reply::stock_reply(reply::bad_request);
        return;
    }

    // Request path must be absolute and not contain "..".
    if (request_path.empty() || request_path[0] != '/' ||
        request_path.find("..") != std::string::npos) {
        Logger::get_instance().err("Path shouldn't contain .. sequence");
        rep = reply::stock_reply(reply::bad_request);
        return;
    }
    Logger::get_instance().info("requested path: {}", request_path);

    request_processing::cut_video_request cut_video_request;
    std::string path_to_result_file{};
    if (request_processing::parse_cut_video_request(request_path,
                                                    cut_video_request)) {
        Logger::get_instance().info("video format: {}, path to video: {}, start point: {} ms, duration {} ms",
            cut_video_request.video_format,
            cut_video_request.path_to_video,
            cut_video_request.video_start_point_ms,
            cut_video_request.video_duration_ms);

        std::optional<std::string> result{
            video_processing::extract_video_segment_from_beginning(
                doc_root_, cut_video_request)};
        if (result) {
            Logger::get_instance().info(R"(Processing of "{}" video has completed successfully)", cut_video_request.path_to_video);
            path_to_result_file = *result;
        } else {
            Logger::get_instance().err(R"(Unable to process video file: "{}". No such video)", cut_video_request.path_to_video);
            rep = reply::stock_reply(reply::bad_request);
            return;
        }
    } else {
        Logger::get_instance().err(R"(Received path doesn't satisfy the required format: "<video_format>/<path_to_video>/<start_ms>/<duration_ms>")");
    }

    std::ifstream is(path_to_result_file, std::ios::in | std::ios::binary);
    if (!is) {
        rep = reply::stock_reply(reply::not_found);
        return;
    }

    // Fill out the reply to be sent to the client.
    rep.status = reply::ok;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        rep.content.append(buf, is.gcount());
    rep.headers.resize(2);
    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
    rep.headers[1].name = "Content-Type";
    rep.headers[1].value =
        mime_types::extension_to_type(cut_video_request.video_format);

    Logger::get_instance().info(R"(Fragment of "{}" has been returned to a client)", cut_video_request.path_to_video);
}

bool request_handler::url_decode(const std::string& in, std::string& out) {
    out.clear();
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i) {
        if (in[i] == '%') {
            if (i + 3 <= in.size()) {
                int value = 0;
                std::istringstream is(in.substr(i + 1, 2));
                if (is >> std::hex >> value) {
                    out += static_cast<char>(value);
                    i += 2;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else if (in[i] == '+') {
            out += ' ';
        } else {
            out += in[i];
        }
    }
    return true;
}

} // namespace server
} // namespace http
