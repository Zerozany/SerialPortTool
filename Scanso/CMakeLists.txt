cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(CMAKE_TOOLCHAIN_FILE "D:\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(socsn VERSION 0.1 LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Network Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Network Concurrent)
find_package(OpenSSL REQUIRED)

# 将外部 .exe 文件复制到输出目录
configure_file("./pem/Rar.exe" "${CMAKE_BINARY_DIR}/pem/Rar.exe" COPYONLY)

qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCE_DIR}/main.cpp
)

file(GLOB RESOURCES_LIST
    RELATIVE ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/pem/*.pem
)

file(GLOB SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/include/*.h
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

qt_add_resources(${PROJECT_NAME}
    PREFIX "/"
    FILES
    ${RESOURCES_LIST}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    AUTOUIC ON
    AUTOMOC ON
    AUTORCC ON
)

target_compile_features(${PROJECT_NAME}
    PRIVATE
    cxx_std_23
)

target_sources(${PROJECT_NAME}
    PRIVATE
    ${SOURCE_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Concurrent
    OpenSSL::SSL
    OpenSSL::Crypto
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
