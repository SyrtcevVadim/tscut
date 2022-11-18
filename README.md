# Клонирование данного репозитория
Данный проект содрежит подмодули git submodules для работы с внешними зависимостями.
Для клонирования используйте команду ```git clone --recurse-submodules -j4``` 

P.S. Параметр -j4 позволяет распараллелить процесс на 4 логических потока.

# Зависимости данного проекта
Данный проект зависит от следующих библиотек:

- Boost
- Catch2
- ffmpeg

Библиотеки Boost и Catch2 внедрены в проект как git-submodules и предварительное их наличие на устройстве не требуется
Библиотека ffmpeg должна быть установлена в директорию ${HOME}/libraries/ffmpeg

# Выбор нужной версии библиотек Boost и Catch2
*Находясь в корне проекта, откройте терминал и наберите следующие команды*
```
>> сd dependencies/boost
>> git checkout boost-1.80.0
>> cd ../dependencies/catch2
// Во второй версии Catch2 присутствует заголовочный файл <catch2/catch.hpp> 
>> git checkout v2.x
```


# Сборка проекта
Сборка проекта осуществляется с помощью утилиты CMake

## Этапы сборки
Находясь в директории проекта выполнить команды:
```
>> cd ../
>> mkdir tscut_build
>> cd tscut_build
// Конфигурирование сборки. Это похоже на вызов ./configure.sh
>> cmake ../tscut
// Для сборки неоптимизированной версии
>> cmake --build .
// Для сборки проекта в релизе
>> cmake --build . -j4 --config Release
```
# Тестирование
После сборки тесты должны находиться в директории *build/tests/tscut_tests*.

# Соглашения по форматированию кода
Для форматирования кода используется инструмент *clang-format*.
Перед тем как зафиксировать изменения, находясь в корне проекта, запустите команду:
```
>> clang-format -i sources/*.cpp && clang-format -i sources/*.hpp
```