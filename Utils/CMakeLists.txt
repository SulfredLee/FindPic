file(GLOB Utils_src
  "${PROJECT_SOURCE_DIR}/Utils/*.cpp"
  "${PROJECT_SOURCE_DIR}/Utils/*.h")

message(STATUS "Utils - include path: ${Boost_INCLUDE_DIR}")

include_directories(${PROJECT_SOURCE_DIR}/Utils
  ${Boost_INCLUDE_DIR})

# state that this project is a library
add_library(Utils
  ${Utils_src}) # static library
# add_library(Utils SHARED ${Utils_src}) # dynamic library

# Adds logic to INSTALL.vcproj to copy *.a to destination directory
install (TARGETS Utils
  RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin
  LIBRARY DESTINATION ${PROJECT_BINARY_DIR}/bin
  ARCHIVE DESTINATION ${PROJECT_BINARY_DIR}/bin)
