# Клонирование данного репозитория
Данный проект содрежит подмодули git submodules для работы с внешними зависимостями.
Для клонирования используйте команду ```git clone --recurse-sumbodules -j4``` 

P.S. Параметр -j4 позволяет распараллелить процесс на 4 логических потока.

# Зависимости данного проекта
Данный проект зависит от следующих библиотек:

- Boost
- Catch2
- ffmpeg

Библиотеки Boost и Catch2 внедрены в проект как git-submodules и предварительное их наличие на устройстве не требуется
Библиотека ffmpeg должна быть установлена в директорию ${HOME}/libraries/ffmpeg