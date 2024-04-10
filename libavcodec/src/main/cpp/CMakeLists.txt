# CMake最小版本
cmake_minimum_required(VERSION 3.22.1)

# 项目名称
project("avcodec")



include_directories(include)

# 导入预编译库
# opus
add_library(opus SHARED IMPORTED)

set_target_properties(opus PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libopus.so)

include_directories(include/opus/include)

# 导入预编译库
# crypto
add_library(crypto SHARED IMPORTED)
set_target_properties(crypto PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libcrypto.so)

## openssl
add_library(openssl SHARED IMPORTED)
set_target_properties(openssl PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libssl.so)

## srt
add_library(srt SHARED IMPORTED)
set_target_properties(srt PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libsrt.so)

# 生成目标库
add_library(${CMAKE_PROJECT_NAME} SHARED
        src/native-lib.cpp
        src/RecordBuffer.cpp
        src/audio.cpp)

# TODO 该函数只能在add_executable/add_library之后用
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC include/srt/${CMAKE_ANDROID_ARCH_ABI}/include)

# 链接库文件
target_link_libraries(${CMAKE_PROJECT_NAME}
        opus
        crypto
        openssl
        OpenSLES
        srt
        android
        log)