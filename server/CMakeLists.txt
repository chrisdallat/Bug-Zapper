cmake_minimum_required (VERSION 3.10)
project (server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QT6 QT5 specifics
#find_package(   QT NAMES Qt6 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
#find_package(   Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)

#find_package(   OpenCV REQUIRED )
#find_package(   SDL2 REQUIRED   )

#include all relevant directories
#include_directories(    ${PROJECT_SOURCE_DIR} ) 

#include_directories(    ${OpenCV_INCLUDE_DIRS} 
#                        ${SDL2_INCLUDE_DIRS}    )


#file(GLOB ALL_SRCS      "${PROJECT_SOURCE_DIR}*.hpp"
#                        "${PROJECT_SOURCE_DIR}*.cpp")
 
add_executable(         ${PROJECT_NAME} main.cpp server.cpp sockets.cpp)

#target_link_libraries(  ${PROJECT_NAME} PRIVATE
                        #Qt${QT_VERSION_MAJOR}::Widgets
                        #Qt${QT_VERSION_MAJOR}::Multimedia 
                        #Qt${QT_VERSION_MAJOR}::MultimediaWidgets
                        # AV
                        #${SDL2_LIBRARIES}
                        #${OpenCV_LIBS})

#if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
#    qt_finalize_executable(${PROJECT_NAME})
#endif()

