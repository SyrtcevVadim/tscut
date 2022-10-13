#include "path_parser.hpp"

namespace request_processing {
bool parse_cut_video_request(std::string& request_path,
                             cut_video_request& cut_video_request) {
    /*
      Объяснение данного regex-шаблона:
      ^/(?<video_format>[^/]+)/ - именованная группа для сохранения формата
      видео из url. Пример: /ts/ знак ^ указывает на начало URL-строки, а шаблон
      [^/]+ позволяет выбрать все символы, кроме символа косой черты, т.к.
      данный символ является разделителем
      (?<path_to_video>.+\.[^/]+)/ - именованная группа для сохранения пути к
      видео из url.
      (?<video_start_point_ms>[[:digit:]]+)/ - именованная группа для сохранения
      времени начала видео в мс. [[:digit:]]+ означает последовательность цифр,
      т.е. число
      (?<video_length_ms>[[:digit:]]+)$ - именованная группа для сохранения
      длительности видео в мс. Знак $ обозначает конец url
    */
    boost::regex cut_video_regex(
        R"(^/(?<video_format>[^/]+)/(?<path_to_video>.+\.[^/]+)/(?<video_start_point_ms>[[:digit:]]+)/(?<video_length_ms>[[:digit:]]+)$)");
    boost::smatch search_result;
    if (boost::regex_search(request_path.cbegin(), request_path.cend(),
                            search_result, cut_video_regex)) {
        cut_video_request.video_format = search_result["video_format"];
        cut_video_request.path_to_video = search_result["path_to_video"];
        cut_video_request.video_start_point_ms =
            std::stoull(search_result["video_start_point_ms"]);
        cut_video_request.video_duration_ms =
            std::stoull(search_result["video_length_ms"]);
        return true;
    }
    return false;
}
} // namespace request_processing