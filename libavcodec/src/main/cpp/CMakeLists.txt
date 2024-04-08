
cmake_minimum_required(VERSION 3.22.1)


project(Srt)


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


add_library(${CMAKE_PROJECT_NAME} SHARED
        Srt.c)

target_include_directories(Srt PUBLIC include/${CMAKE_ANDROID_ARCH_ABI}/include)
include_directories(.)

target_link_libraries(${CMAKE_PROJECT_NAME}
        crypto
        openssl
        srt
        android
        log)