project(rzip)

set(CMAKE_CXX_STANDARD 20)

add_executable(rzfile rzip/zfile.cc rzip/huffman.cc)
