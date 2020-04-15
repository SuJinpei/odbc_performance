cmake_minimum_required (VERSION 3.10)
project (odb)
set(CMAKE_CXX_STANDARD 17)
include_directories(include)
file(GLOB src
  "src/*.cpp"
  )
add_link_options(-lpthread -lodbc -static-libgcc -static-libstdc++)
add_executable(odb ${src})
