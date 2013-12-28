find_package(JNI 1.6 REQUIRED)
find_package(Java 1.7 REQUIRED Development)
include(UseJava)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

if(CMAKE_HOST_UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

find_package(Qt5Core 5.2 REQUIRED)
find_package(Qt5Widgets 5.2 REQUIRED)
find_package(Qt5Network 5.2 REQUIRED)

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
    Utilities.cxx
    ViewTypeBuilder.cxx
    JvmClass.cxx
    Object.cxx
    MessageWidget.cxx
    TabWidget.cxx
    ViewWidget.cxx
    ModelViewWidget.cxx
    Meme.cxx
    MemoizerModel.cxx
    MemeProcessor.cxx
    MemeImageWidget.cxx
    MemeWidget.cxx
    MetadataHandler.cxx
    MemoizerModelController.cxx
    MemeBuilder.cxx
    MemoizerModelBuilder.cxx
)

qt5_wrap_ui(UI_HDRS_LIST
    WaitForVMWidget.ui
    VmErrorWidget.ui
    MessageWidget.ui
    ViewWidget.ui
    MemeWidget.ui
)


add_library(MemoizerGui ${SRC_LIST} ${UI_HDRS_LIST})
target_include_directories(MemoizerGui PRIVATE
    ${JAVA_INCLUDE_PATH}
    ${JAVA_INCLUDE_PATH2})

target_link_libraries(MemoizerGui Qt5::Core Qt5::Widgets Qt5::Network)
if(NOT CMAKE_HOST_WIN32)
    target_link_libraries(MemoizerGui ${JAVA_JVM_LIBRARY})
endif()
