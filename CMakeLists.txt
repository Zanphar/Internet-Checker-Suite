cmake_minimum_required(VERSION 3.16)
project(InternetConnectionUtility LANGUAGES CXX)

if(WIN32)
    enable_language(RC)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(InternetConnectionUtility WIN32
    src/main.cpp
    src/ui.cpp
    src/doctor.cpp
    src/net.cpp
    res/app.rc
)

# C++ only: windres word-splits an -I value that contains spaces, which breaks the RC
# step whenever the repository lives under a path like "Internet Checker". app.rc finds
# resource.h through its quoted include, relative to its own folder, so it needs no -I.
target_include_directories(InternetConnectionUtility PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/res>)
target_compile_definitions(InternetConnectionUtility PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)

if(MSVC)
    target_compile_options(InternetConnectionUtility PRIVATE /W3 /utf-8)
    target_link_libraries(InternetConnectionUtility PRIVATE gdiplus wlanapi iphlpapi winhttp dwmapi ws2_32 ole32 uuid shell32)
else()
    target_compile_options(InternetConnectionUtility PRIVATE -Wall -Wextra -Wno-unused-parameter -O2)
    target_link_options(InternetConnectionUtility PRIVATE -static-libgcc -static-libstdc++)
    target_link_libraries(InternetConnectionUtility PRIVATE
        gdiplus wlanapi iphlpapi winhttp dwmapi ws2_32 ole32 uuid shell32)
endif()
