#include "VirtualMachine.hxx"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include "LocalRefsGuard.hxx"
#include "JvmException.hxx"
#include "StateMap.hxx"
#include "EventService.hxx"
#include "PluginManager.hxx"

VirtualMachine::VirtualMachine(QObject *parent) :
    QObject(parent), _javaVm(nullptr), _jniEnv(nullptr)
{
}

VirtualMachine::~VirtualMachine()
{
}

void VirtualMachine::setVmProvider(jint (JNICALL* vmProvider)(JavaVM **, void **, void *))
{
    _CreateJavaVM=vmProvider;
}

void jvmAbort()
{
    qCritical("JVM zarządało abort()");
    abort();
}

void jvmExit(int r)
{
    qCritical("JVM zarządało exit(%d)",r);
    exit(r);
}

static QString buildClasspath()
{
#ifdef Q_OS_WIN
    const QString pathSeparator=";";
#else
    const QString pathSeparator=":";
#endif
    QDir appBaseDir(qApp->applicationDirPath());
    appBaseDir.cdUp();
    appBaseDir.cd("lib");
    QFileInfoList classpath=appBaseDir.entryInfoList(QStringList()<<"*.jar");
    QStringList result;
    for(const auto file : classpath)
    {
        result<<QDir::toNativeSeparators(file.canonicalFilePath());
    }
    return result.join(pathSeparator);
}

void VirtualMachine::start()
{
    if(!_CreateJavaVM)
    {
        emit initializationFailed();
        return;
    }
    QByteArray classpath=("-Djava.class.path="+buildClasspath()).toLocal8Bit();
    JavaVMOption options[]={
    #ifndef NDEBUG
        {const_cast<char*>("-verbose:class,gc,jni"),nullptr},
        {const_cast<char*>("-Xcheck:jni"),nullptr},
    #endif
        {classpath.data(),nullptr},
        {const_cast<char*>("abort"),reinterpret_cast<void*>(&jvmAbort)},
        {const_cast<char*>("exit"),reinterpret_cast<void*>(&jvmExit)},
    };

    JavaVMInitArgs initArgs;
    initArgs.ignoreUnrecognized=false;
    initArgs.version=JNI_VERSION_1_6;
    initArgs.options=options;
    initArgs.nOptions=std::extent<decltype(options)>::value;

    auto res=_CreateJavaVM(&_javaVm,reinterpret_cast<void**>(&_jniEnv),&initArgs);
    if(res)
    {
        emit initializationFailed();
        return;
    }
    emit started();
    prepareCommonObjects();
}

void VirtualMachine::storePluginsSelection(const QStringList& selected, const QStringList& unselected)
{
    try
    {
        _stateMap->setPluginsSelection(_jniEnv,selected,unselected);
    }
    catch(const JvmException& e)
    {
        emit exceptionOccured(e);
    }
}

void VirtualMachine::stop()
{
    if(_javaVm)
    {
        emit aboutToStop();
        try
        {
            _eventService->shutdown(_jniEnv);
        }
        catch(const JvmException& e)
        {
            emit exceptionOccured(e);
        }
        // Kasuje referencje do obiektów składowych
        _pluginManager.reset();
        _eventService.reset();
        _stateMap.reset();
        _allViewTypes.clear();

        _javaVm->DestroyJavaVM();
        emit stopped();
    }
}

static QString getConfigPath()
{
    QSettings settings;
    const QString defaultConfigPath=QUrl::fromLocalFile(QDir::toNativeSeparators(QDir::homePath() + "/.memoizer.json")).toString(QUrl::None);

    QString configPath=settings.value("configPath").toString();
    if(configPath.isEmpty())
    {
        configPath=defaultConfigPath;
        settings.setValue("configPath",QVariant::fromValue(configPath));
    }
    return configPath;
}

/**
 * @brief Przygotowuje menadżera pluginów i pobiera z niego dane na potrzeby GUI.
 */
void VirtualMachine::prepareCommonObjects()
{
    LocalRefsGuard localRefs(_jniEnv);
    auto failed=[this](){
        emit exceptionOccured(JvmException::fromEnv(_jniEnv));
        emit initializationFailed();
    };
#define check(x) do {if(!(x)){failed();return;}} while(false)

    jobject uri;
    {
        jclass uriClass=_jniEnv->FindClass("java/net/URI");
        check(uriClass);
        localRefs.insert(uriClass);
        jmethodID uriInit=_jniEnv->GetMethodID(uriClass,"<init>","(Ljava/lang/String;)V");
        check(uriInit);
        jstring configPath;
        {
            const QString configPathP=getConfigPath();
            configPath=_jniEnv->NewString(configPathP.utf16(),configPathP.length());
        }
        check(configPath);
        localRefs.insert(configPath);
        uri=_jniEnv->NewObject(uriClass,uriInit,configPath);
        check(uri);
        localRefs.removeAll();
        localRefs.insert(uri);
    }

    jclass mainClass=_jniEnv->FindClass("pl/edu/uj/tcs/memoizer/Main");
    check(mainClass);
    localRefs.insert(mainClass);
    {
        jmethodID getStateMapId=_jniEnv->GetStaticMethodID(
                    mainClass,
                    "getStateMap",
                    "(Ljava/net/URI;)Lpl/edu/uj/tcs/memoizer/serialization/StateMap;");
        check(getStateMapId);
        jobject stateMap=_jniEnv->CallStaticObjectMethod(mainClass,getStateMapId,uri);
        check(stateMap);
        localRefs.insert(stateMap);
        _stateMap=StateMap::wrap(_jniEnv,stateMap);
        localRefs.remove(stateMap);
    }
    localRefs.remove(uri);
    // Stworzono obiekt stateMap
    {
        jmethodID getEventServiceId=_jniEnv->GetStaticMethodID(
                    mainClass,
                    "getEventService",
                    "(Lpl/edu/uj/tcs/memoizer/serialization/StateMap;)Lpl/edu/uj/tcs/memoizer/events/IEventService;");
        check(getEventServiceId);
        jobject eventService=_jniEnv->CallStaticObjectMethod(
                    mainClass,getEventServiceId,_stateMap->unwrap());
        check(eventService);
        localRefs.insert(eventService);
        _eventService=EventService::wrap(_jniEnv,eventService);
        localRefs.remove(eventService);
    }
    // Stworzono obiekt eventService
    {
        QDir appBaseDir=qApp->applicationDirPath();
        appBaseDir.cdUp();
        const QString appBaseDirS=QDir::toNativeSeparators(appBaseDir.absolutePath());
        jstring appBaseDirJ=_jniEnv->NewString(appBaseDirS.utf16(),appBaseDirS.length());
        check(appBaseDirJ);
        localRefs.insert(appBaseDirJ);
        jmethodID getPluginManagerId=_jniEnv->GetStaticMethodID(
                    mainClass,
                    "getPluginManager",
                    "(Ljava/lang/String;Lpl/edu/uj/tcs/memoizer/events/IEventService;)"
                    "Lpl/edu/uj/tcs/memoizer/plugins/communication/IPluginManagerClient;");
        check(getPluginManagerId);
        jobject pluginManager=_jniEnv->CallStaticObjectMethod(
                    mainClass,getPluginManagerId,appBaseDirJ,_eventService->unwrap());
        check(pluginManager);
        localRefs.insert(pluginManager);
        _pluginManager=PluginManager::wrap(_jniEnv,pluginManager);
        localRefs.remove(pluginManager);
        localRefs.remove(appBaseDirJ);
    }
    // Stworzono obiekt pluginManager
    localRefs.removeAll();
    emit readyToUse();
    prepareDataForGui();
#undef check
}

void VirtualMachine::prepareDataForGui()
{
    try
    {
        QStringList pluginsNamesL=_pluginManager->getAllPluginNames(_jniEnv);
        QStringList selectedPluginsL=_stateMap->selectedPlugins(_jniEnv);
        emit pluginsNames(pluginsNamesL,selectedPluginsL);
        _allViewTypes=_pluginManager->getAvailableViews(_jniEnv);
        QList<std::weak_ptr<const ViewType>> softViewTypes;
        softViewTypes.reserve(_allViewTypes.size());
        for(const auto v : _allViewTypes)
            softViewTypes.append(v);
        emit viewsTypes(softViewTypes);
        for(const auto viewType: _allViewTypes)
        {
            auto results=_pluginManager->getPluginsNamesForView(_jniEnv,viewType);
            emit pluginsNamesForView(results,viewType);
        }
        emit initializationComplete(!pluginsNamesL.isEmpty());
    }
    catch(const JvmException& e)
    {
        emit exceptionOccured(e);
        emit initializationFailed();
    }
}
