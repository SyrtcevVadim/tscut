set(FFMPEG_PREFIX $ENV{HOME}/libraries/ffmpeg)
# Ищем библиотеки libavutil libavformat libavcodec
# NO_DEFAULT отключает поиск библиотек в путях по умолчанию.
# Например, у меня брались библиотеки из директории /usr/local/lib и брали статические библиотеки
# Данные же конструкции берут библиотеки по пути ${FFMPEG_PREFIX}/lib
find_library(FFMPEG_AVUTIL avutil PATHS ${FFMPEG_PREFIX}/lib NO_DEFAULT_PATH)
find_library(FFMPEG_AVFORMAT avformat PATHS ${FFMPEG_PREFIX}/lib NO_DEFAULT_PATH)
find_library(FFMPEG_AVCODED avcodec PATHS ${FFMPEG_PREFIX}/lib NO_DEFAULT_PATH)
# Проверяем пути
message(STATUS ${FFMPEG_AVUTIL})
message(STATUS ${FFMPEG_AVFORMAT})
message(STATUS ${FFMPEG_AVCODED})
include_directories(${FFMPEG_PREFIX}/include)