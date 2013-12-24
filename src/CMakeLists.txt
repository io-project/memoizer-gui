find_package(JNI 1.6 REQUIRED)
find_package(Java 1.7 REQUIRED Development)
include(UseJava)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core 5.2 REQUIRED)
find_package(Qt5Widgets 5.2 REQUIRED)

set(SRC_LIST
    Core.cxx
    MainWindow.cxx
    CentralWidget.cxx
    WaitForVMWidget.cxx
    VirtualMachine.cxx
    ViewType.cxx
    VmErrorWidget.cxx
    LocalRefsGuard.cxx
    JvmException.cxx
    JvmObject.cxx
    StateMap.cxx
    EventService.cxx
    PluginManager.cxx
)

qt5_wrap_ui(UI_HDRS_LIST
    WaitForVMWidget.ui
    VmErrorWidget.ui
)


add_library(MemoizerGui ${SRC_LIST} ${UI_HDRS_LIST})
target_include_directories(MemoizerGui PRIVATE
    ${JAVA_INCLUDE_PATH}
    ${JAVA_INCLUDE_PATH2})

target_link_libraries(MemoizerGui Qt5::Core Qt5::Widgets)
