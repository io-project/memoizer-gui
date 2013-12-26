project(memoizer-gui CXX)
cmake_minimum_required(VERSION 2.8.11)
cmake_policy(SET CMP0020 NEW)

add_subdirectory(src)

add_maven(java package memoizer-gui-1.0-SNAPSHOT.jar DEPENDS memoizer)
install_maven(java lib)

add_executable(${PROJECT_NAME} WIN32 main.cxx)
target_link_libraries(${PROJECT_NAME} MemoizerGui)
if(NOT CMAKE_HOST_WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH_USE_LINK_PATH ON)
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
