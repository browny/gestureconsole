cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(../include/)
ADD_EXECUTABLE(main main.cpp capture.cpp recognizer.cpp gesture.cpp reactor.cpp segmenter.cpp skin.cpp skinAlgo.cpp spotter.cpp svm-predict.cpp svm.cpp Timer.cpp tracker.cpp utility.cpp) 

FIND_PACKAGE(OpenCV REQUIRED)

TARGET_LINK_LIBRARIES(main ${OpenCV_LIBS})

FILE(COPY ${PROJECT_SOURCE_DIR}/scale_para	DESTINATION ${PROJECT_BINARY_DIR}/bin)
FILE(COPY ${PROJECT_SOURCE_DIR}/training_data.txt.scale.model	DESTINATION ${PROJECT_BINARY_DIR}/bin)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
