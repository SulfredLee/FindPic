# Handle boost libraries
find_package(Boost REQUIRED
  log
  log_setup
  filesystem)

message(STATUS "SearchEng - Boost library status:")
message(STATUS "SearchEng -     version: ${BoosT_VERSION}")
message(STATUS "SearchEng -     libraries: ${Boost_LIBRARIES}")
message(STATUS "SearchEng -     include path: ${Boost_INCLUDE_DIR}")

# Handle opencv libraries
find_package(OpenCV REQUIRED)

message(STATUS "SearchEng - OpenCV library status:")
message(STATUS "SearchEng -     version: ${OpenCV_VERSION}")
message(STATUS "SearchEng -     libraries: ${OpenCV_LIBS}")
message(STATUS "SearchEng -     include path: ${OpenCV_INCLUDE_DIRS}")

file(GLOB FindPic_src
  "${PROJECT_SOURCE_DIR}/SearchEng/*.cpp"
  "${PROJECT_SOURCE_DIR}/SearchEng/*.h")

include_directories(${PROJECT_SOURCE_DIR}
  ${OpenCV_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIR})

add_executable(FindPic ${FindPic_src})
add_dependencies(FindPic Utils)

if (UNIX)
  target_link_libraries(FindPic
    debug "${PROJECT_BINARY_DIR}/Utils/libUtils.a"
    optimized "${PROJECT_BINARY_DIR}/Utils/libUtils.a")
else ()
  target_link_libraries(GLOB FindPic_lib
    debug ${PROJECT_BINARY_DIR}/Utils/Debug/Utils.lib
    optimized ${PROJECT_BINARY_DIR}/Utils/Release/Utils.lib)
endif ()

target_link_libraries(FindPic
  ${Boost_LIBRARIES}
  ${OpenCV_LIBS})

set_property(TARGET FindPic PROPERTY FOLDER "executables")
