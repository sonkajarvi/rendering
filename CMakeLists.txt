cmake_minimum_required(VERSION 3.20)

project(rendering LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(rendering
    "main.c"

    "gl_context.c"
    "gl_renderer.c"
    "gl_texture.c"
    "glx_context.c"
    "image.c"
    "renderer.c"
    "util.c"
    "x11_window.c"

    # glad sources
    "third_party/glad/src/gl.c"
    "third_party/glad/src/glx.c"
)

target_include_directories(rendering PRIVATE
    "third_party/cglm/include"
    "third_party/glad/include"
    "third_party/stb/include"
)

target_link_libraries(rendering PRIVATE
    "X11"
    "m"
)

target_compile_options(rendering PRIVATE
    "-Wall"
    "-Werror"
    "-Wextra"
    "-Wstrict-prototypes"
)

target_compile_definitions(rendering PRIVATE
    "RESOURCES_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/resources\""
)
