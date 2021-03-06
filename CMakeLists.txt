CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

SET(TARGET "GUI")

IF(NOT DEFINED GUI_VERSION_MAJOR)
  SET(GUI_VERSION_MAJOR 0)
ENDIF()
IF(NOT DEFINED GUI_VERSION_MINOR)
  SET(GUI_VERSION_MINOR 1)
ENDIF()
IF(NOT DEFINED PATCH)
    string(TIMESTAMP PATCH "%Y%m%d")
ENDIF()

SET(VERSION_STRING ${GUI_VERSION_MAJOR}.${GUI_VERSION_MINOR}.${PATCH})

PROJECT(${TARGET} LANGUAGES CXX VERSION ${VERSION_STRING})
SET(CMAKE_CXX_STANDARD 17)

SET(CMAKE_VERBOSE_MAKEFILE ON)

# link opencv.
find_package(OpenCV 4 REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

# link sdl2
find_package(SDL2 REQUIRED)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})

# link glfw3
find_package(glfw3 REQUIRED)

IF((IOS AND CMAKE_OSX_ARCHITECTURES MATCHES "arm")
  OR (APPLE AND CMAKE_OSX_ARCHITECTURES MATCHES "arm64")
  OR (CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|aarch64)"))
  SET(TARGET_ARCH aarch64)
ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(mips)")
  SET(TARGET_ARCH mips)
ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(riscv)")
  SET(TARGET_ARCH riscv)
ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc)")
  SET(TARGET_ARCH powerpc)
ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64)|(X86_64)|(x64)|(X64)|(amd64)|(AMD64)|(i686)")
  SET(TARGET_ARCH x64)
ENDIF()

#options
IF(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
ENDIF()

SET(BUILD_MODULE_TYPE SHARED)
AUX_SOURCE_DIRECTORY(. SRCS)
#SET(SRCS main.cpp)
ADD_EXECUTABLE(${PROJECT_NAME}
  ${SRCS}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenCV_LIBRARIES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} -lGL -ldl)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} glfw -lGL)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin)

MESSAGE(STATUS ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
MESSAGE(STATUS "BUILD INFO: ")
MESSAGE(STATUS "\t TARGETS = ${TARGET}")
MESSAGE(STATUS "\t VERSION = ${GUI_VERSION_MAJOR}.${GUI_VERSION_MINOR}")
MESSAGE(STATUS "\t CONFIG = ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "\t SYSTEM = ${CMAKE_SYSTEM_NAME}")
MESSAGE(STATUS "\t TARGET ARCH = ${TARGET_ARCH}")
MESSAGE(STATUS "\t SDL2_LIBRARIES = ${TARGET_ARCH}")
#MESSAGE(STATUS "\t SRCS = ${SRCS}")
