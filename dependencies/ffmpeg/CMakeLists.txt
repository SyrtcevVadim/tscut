set(FFMPEG_PREFIX $ENV{HOME}/libraries/ffmpeg)
# Ищем библиотеку ffmpeg::libavutil, ffmpeg::libavformat
find_library(FFMPEG avutil avformat PATHS ${FFMPEG_PREFIX}/lib)
include_directories(${FFMPEG_PREFIX}/include)