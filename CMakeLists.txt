project(memoizer-gui CXX)
cmake_minimum_required(VERSION 2.8.11)
cmake_policy(SET CMP0020 NEW)

add_subdirectory(src)

#add_maven(java package memoizer-gui-0.0.1-SNAPSHOT.jar DEPENDS memoizer)
#install_maven(java bin)

add_executable(${PROJECT_NAME} WIN32 main.cxx)
target_link_libraries(${PROJECT_NAME} MemoizerGui)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
