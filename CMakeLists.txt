cmake_minimum_required(VERSION 3.13)
project (ovh-dynhost)

include_directories($TOOLCHAIN/include)
link_directories($TOOLCHAIN/lib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")

set(LINKING_LIBRARIES "-lcurl -lwolfssl -lz -lcares -lnghttp2")

add_executable (ovh-dynhost ovh-dynhost.cpp)
target_link_libraries (ovh-dynhost ${LINKING_LIBRARIES} -static)
