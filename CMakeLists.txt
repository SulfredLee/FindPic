cmake_minimum_required(VERSION 2.8)

set(project_name "FindPic")
project("${project_name}")

set(CMAKE_CXX_FLAGS "-Wall -std=c++14")

# Handle Opencv libaries
if (UNIX)
  set(OpenCV_INCLUDE_DIRS "/usr/local/opencv/2.4.13.2/include")
  set(OpenCV_LIBS "/usr/local/opencv/2.4.13.2/lib")
  set(CMAKE_PREFIX_PATH "/usr/local/opencv/2.4.13.2")
else ()
  set(OpenCV_INCLUDE_DIRS "C:\\boost\\boost_1_59_0")
  set(OpenCV_LIBS "C:\\boost\\boost_1_59_0\\lib64-msvc-12.0")
endif ()

# Handle Boost libaries
if (UNIX)
  set(Boost_INCLUDE_DIR "/usr/local/boost/1.59.0/include")
  set(Boost_LIBRARY_DIR "/usr/local/boost/1.59.0/lib")
else ()
  set(Boost_INCLUDE_DIR "C:\\boost\\boost_1_59_0")
  set(Boost_LIBRARY_DIR "C:\\boost\\boost_1_59_0\\lib64-msvc-12.0")
endif ()
set(Boost_USE_STATIC_LIBS   ON)

add_subdirectory(SearchEng/SimpleSearch)
add_subdirectory(SearchEng/EigenSearch)
add_subdirectory(SearchEng/FaceDetection)
add_subdirectory(SearchEng/FisherFaces)
add_subdirectory(SearchEng/LPBHistograms)
add_subdirectory(Utils)
