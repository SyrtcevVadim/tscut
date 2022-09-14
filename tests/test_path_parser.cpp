#include<catch2/catch.hpp>
#include<string>
#include"path_parser.hpp"

TEST_CASE("Test parse_cut_video_request function", "[parse_cut_video_request]")
{
    video_processing::cut_video_request request;
    SECTION("Cut video from start")
    {
        std::string request_path{"/ts/mdb4/my_folder/abcd/1231324343.ts/0/3477"};
        REQUIRE(video_processing::parse_cut_video_request(request_path, request));
        REQUIRE(request.video_format == "ts");
        REQUIRE(request.path_to_video == "mdb4/my_folder/abcd/1231324343.ts");
        REQUIRE(request.video_start_point_ms ==  0);
        REQUIRE(request.video_length_ms == 3477);
    }

    SECTION("Path hasn't file with extension")
    {
        std::string request_path{"/ts/mdb4/my_folder/abcd/1231324343/0/3477"};
        REQUIRE_FALSE(video_processing::parse_cut_video_request(request_path, request));
    }
}
