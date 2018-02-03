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

file(GLOB EigenSearch_src
  "${PROJECT_SOURCE_DIR}/SearchEng/LPBHistograms/*.cpp"
  "${PROJECT_SOURCE_DIR}/SearchEng/LPBHistograms/*.h")

include_directories(${PROJECT_SOURCE_DIR}
  ${OpenCV_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIR})

add_executable(LPBHistograms ${EigenSearch_src})
add_dependencies(LPBHistograms Utils)

if (UNIX)
  target_link_libraries(LPBHistograms
    debug "${PROJECT_BINARY_DIR}/Utils/libUtils.a"
    optimized "${PROJECT_BINARY_DIR}/Utils/libUtils.a")
else ()
  target_link_libraries(GLOB LPBHistograms_lib
    debug ${PROJECT_BINARY_DIR}/Utils/Debug/Utils.lib
    optimized ${PROJECT_BINARY_DIR}/Utils/Release/Utils.lib)
endif ()

target_link_libraries(LPBHistograms
  ${Boost_LIBRARIES}
  ${OpenCV_LIBS})

set_property(TARGET LPBHistograms PROPERTY FOLDER "executables")
