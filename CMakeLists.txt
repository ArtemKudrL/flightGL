cmake_minimum_required(VERSION 3.10)

project(flight)

add_executable(flight src/main.cxx src/application.cxx src/renderer.cxx src/window.cxx src/game.cxx src/body.cxx)
target_include_directories(flight PRIVATE include)
target_compile_options(flight PRIVATE -O2 -g)
target_link_libraries(flight PRIVATE -lglfw -lGL -lGLEW)
