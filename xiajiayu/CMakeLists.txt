cmake_minimum_required(VERSION 3.5)
project(BallBalance)

set(CMAKE_CXX_STANDARD 11)

FIND_PACKAGE(OpenCV REQUIRED )

INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIR} )

ADD_EXECUTABLE(BallBalance BallCenter.cpp BallCenter.h Header.h Main.cpp SerialPort.h SerialPort.cpp )
TARGET_LINK_LIBRARIES(BallBalance ${OpenCV_LIBS})


