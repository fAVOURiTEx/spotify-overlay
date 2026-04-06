# 1️⃣ Минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

# 2️⃣ Название проекта
project(SpotifyOverlay)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 3️⃣ Стандарт C++ (17 — современный, с optional, string_view и т.д.)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4️⃣ Поиск системных библиотек
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# 5️⃣ Путь к ImGui (локальная копия в проекте)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

# 6️⃣ Все исходные файлы
set(SOURCES
    src/main.cpp
    src/animation.cpp
    src/spotify_track.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# 7️⃣ Заголовочные файлы (где компилятор ищет #include)
include_directories(
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    src
    include
)

# 8️⃣ Создаём исполняемый файл
add_executable(overlay ${SOURCES})

# 9️⃣ Подключаем библиотеки к нашему executable
target_link_libraries(overlay
    OpenGL::GL
    glfw
)

# 🔟 Платформенные особенности (macOS требует фреймворки)
if(APPLE)
    target_link_libraries(overlay
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

# 1️⃣1️⃣ Предупреждения компилятора (помогают ловить ошибки)
if(MSVC)
    target_compile_options(overlay PRIVATE /W4)
else()
    target_compile_options(overlay PRIVATE -Wall -Wextra -pedantic)
endif()

