cmake_minimum_required(VERSION 2.8)
set(ProjectName EasyARTest)
project(${ProjectName})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(INSTALL_BIN_DIR "${PROJECT_SOURCE_DIR}/bin" CACHE PATH "Installation directory for executables")

file(GLOB sources "src/*.cpp")

include_directories(
include
include/easyar
)

add_executable(${ProjectName} ${sources})

#target_link_libraries(${ProjectName} ${OpenCV_LIBS})

set(
OPenCV_LIBS
${PROJECT_SOURCE_DIR}/libs/opencv_world320d.lib
)
set(
EASYAR_LIB
${PROJECT_SOURCE_DIR}/libs/EasyAR.lib
)

target_link_libraries(${ProjectName} ${OPenCV_LIBS}  ${EASYAR_LIB})
set(
DLL
${PROJECT_SOURCE_DIR}/libs/EasyAR.dll
${PROJECT_SOURCE_DIR}/libs/opencv_world320d.dll
)
add_custom_command(
  TARGET ${ProjectName}
 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${DLL}   "${PROJECT_SOURCE_DIR}/bin/Debug"
)

