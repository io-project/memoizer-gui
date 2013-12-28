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
#include "MemoizerModel.hxx"
#include "MetadataHandler.hxx"
#include "MemoizerModelBuilder.hxx"
#include "JvmClass.hxx"
#include "MemoizerModelController.hxx"
#include "MemeBuilder.hxx"
#include "Meme.hxx"

static VirtualMachine* vmInstance=nullptr;

VirtualMachine::VirtualMachine(QObject *parent) :
    QObject(parent), _javaVm(nullptr), _jniEnv(nullptr), _nextId(0),
    _modelBuilder(nullptr), _memeBuilder(nullptr)
{
}

VirtualMachine::~VirtualMachine()
{
    delete _modelBuilder;
    delete _memeBuilder;
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
        {const_cast<char*>("-agentlib:jdwp=transport=dt_socket,server=y,suspend=n,address=5005"),nullptr},
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
    vmInstance=this;
    emit started();
    prepareCommonObjects();
}

void VirtualMachine::prepareModel(QObject *receiver,std::size_t requestId, std::weak_ptr<const ViewType> viewType, const QStringList &pluginsNames)
{
    std::shared_ptr<const ViewType> viewTypeL=viewType.lock();
    if(Q_UNLIKELY(!viewTypeL))
        return;
    jlong handlerId=_nextId++;
    LocalRefsGuard localRefs(_jniEnv);
    jobject memoizerModelHandler=_jniEnv->NewObject(
                _memoizerModelHandlerClass->unwrap(),
                _memoizerModelHandlerInitId,
                handlerId);
    localRefs.checkIn(memoizerModelHandler);
    std::shared_ptr<MemoizerModelController> controller=_modelBuilder->newMemoizerModel(
                _jniEnv,
                viewTypeL,
                pluginsNames,
                _pluginManager,
                _metadataHandler,
                memoizerModelHandler);

    MemoizerModel* model=new MemoizerModel(this);
    model->moveToThread(receiver->thread());
    _memoizerModelToHandlerId[model]=handlerId;
    _memoizerModelToController[model]=controller;
    _handlerIdToMemoizerModel[handlerId]=model;

    QMetaObject::invokeMethod(receiver,
                              "receiveModel",
                              Qt::QueuedConnection,
                              Q_ARG(MemoizerModel*,model),
                              Q_ARG(std::size_t,requestId));
}

void VirtualMachine::prepareSearchModel(QObject *receiver,std::size_t requestId, const QStringList &pluginsNames, const QString &searchKey)
{
    jlong handlerId=_nextId++;
    LocalRefsGuard localRefs(_jniEnv);
    jobject memoizerModelHandler=_jniEnv->NewObject(
                _memoizerModelHandlerClass->unwrap(),
                _memoizerModelHandlerInitId,
                handlerId);
    localRefs.checkIn(memoizerModelHandler);
    std::shared_ptr<MemoizerModelController> controller=_modelBuilder->newMemoizerSearchModel(
                _jniEnv,
                searchKey,
                _pluginManager,
                pluginsNames,
                memoizerModelHandler);

    MemoizerModel* model=new MemoizerModel(this);
    model->moveToThread(receiver->thread());
    _memoizerModelToHandlerId[model]=handlerId;
    _memoizerModelToController[model]=controller;
    _handlerIdToMemoizerModel[handlerId]=model;

    QMetaObject::invokeMethod(receiver,
                              "receiveModel",
                              Qt::QueuedConnection,
                              Q_ARG(MemoizerModel*,model),
                              Q_ARG(std::size_t,requestId));
}

void VirtualMachine::releaseModel(MemoizerModel *model)
{
    if(_memoizerModelToController.find(model)==_memoizerModelToController.end())
        return;
    _memoizerModelToController.erase(model);
    jlong id=_memoizerModelToHandlerId[model];
    _memoizerModelToHandlerId.erase(model);
    _handlerIdToMemoizerModel.erase(id);
    for(Meme meme: _memoizerModelToMemeList[model])
        meme.release(_jniEnv);
    _memoizerModelToMemeList.erase(model);
}

void VirtualMachine::pull(MemoizerModel *model, jint id)
{
    if(Q_UNLIKELY(_memoizerModelToController.find(model)==_memoizerModelToController.end()))
    {
        qWarning("Zarządano pull(%d) na modelu który został już zwolniony",id);
        return;
    }
    std::shared_ptr<MemoizerModelController> controller=_memoizerModelToController[model];
    if(Q_LIKELY(controller->pull(_jniEnv,id)))
        return;
    Meme meme=controller->get(_jniEnv,*_memeBuilder,id);
    QMetaObject::invokeMethod(
                model,
                "notifyUpdate",
                Qt::QueuedConnection,
                Q_ARG(Meme,meme),
                Q_ARG(jint,id));
}

void VirtualMachine::storePluginsSelection(const QStringList& selected, const QStringList& unselected)
{
    if(Q_UNLIKELY(!_jniEnv))
        return;
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
        delete _modelBuilder;
        _modelBuilder=nullptr;
        delete _memeBuilder;
        _memeBuilder=nullptr;
        for(jlong i=0;i<_nextId;++i)
        {
            if(_handlerIdToMemoizerModel.find(i)==_handlerIdToMemoizerModel.end())
                continue;
            releaseModel(_handlerIdToMemoizerModel[i]);
        }

        //_javaVm->DestroyJavaVM(); // FIXME: Zamknąć czysto JVM
    }
    emit stopped();
}

void JNICALL notifyUpdate(
        JNIEnv* jniEnv,
        jobject memoizerModelHandler,
        jobject meme,
        jint id)
{
    if(Q_UNLIKELY(!vmInstance))
    {
        qWarning("Notyfikacja przed rozruchem VM");
        return;
    }
    jlong handlerId=jniEnv->GetLongField(
                memoizerModelHandler,
                vmInstance->_memoizerModelHandlerIdId);
    if(Q_UNLIKELY(vmInstance->_handlerIdToMemoizerModel.find(handlerId)==vmInstance->_handlerIdToMemoizerModel.end()))
    {
        qWarning("Notyfikacja do zwolnionego modelu(%l)",handlerId);
        return;
    }
    MemoizerModel* model=vmInstance->_handlerIdToMemoizerModel[handlerId];
    try
    {
        Meme memeN=vmInstance->_memeBuilder->build(jniEnv,meme);
        vmInstance->_memoizerModelToMemeList[model].push_back(memeN);
        QMetaObject::invokeMethod(
                    model,
                    "notifyUpdate",
                    Qt::QueuedConnection,
                    Q_ARG(Meme,memeN),
                    Q_ARG(jint,id));
    }
    catch(const JvmException& e)
    {
        QMetaObject::invokeMethod(
                    vmInstance,
                    "exceptionOccured",
                    Qt::QueuedConnection,
                    Q_ARG(JvmException,e));
    }
}

void JNICALL notifyStreamEnd(
        JNIEnv* jniEnv,
        jobject memoizerModelHandler)
{
    if(Q_UNLIKELY(!vmInstance))
    {
        qWarning("Notyfikacja przed rozruchem VM");
        return;
    }
    jlong handlerId=jniEnv->GetLongField(
                memoizerModelHandler,
                vmInstance->_memoizerModelHandlerIdId);
    if(Q_UNLIKELY(vmInstance->_handlerIdToMemoizerModel.find(handlerId)==vmInstance->_handlerIdToMemoizerModel.end()))
    {
        qWarning("Notyfikacja do zwolnionego modelu(%l)",handlerId);
        return;
    }
    MemoizerModel* model=vmInstance->_handlerIdToMemoizerModel[handlerId];
    QMetaObject::invokeMethod(
                model,
                "notifyStreamEnd",
                Qt::QueuedConnection);
}

void VirtualMachine::registerNatives()
{
    LocalRefsGuard localRefs(_jniEnv);
    JNINativeMethod nativeMethods[]={
        {
            const_cast<char*>("notifyUpdate"),
            const_cast<char*>("(Lpl/edu/uj/tcs/memoizer/plugins/Meme;I)V"),
            reinterpret_cast<void*>(&notifyUpdate),
        },
        {
            const_cast<char*>("notifyStreamEnd"),
            const_cast<char*>("()V"),
            reinterpret_cast<void*>(&notifyStreamEnd),
        }
    };
    const jint size=std::extent<decltype(nativeMethods)>::value;
    jclass memoizerModelHandlerClass=_jniEnv->FindClass("MemoizerModelHandler");
    localRefs.checkIn(memoizerModelHandlerClass);
    auto res=_jniEnv->RegisterNatives(
                memoizerModelHandlerClass,
                nativeMethods,
                size);
    if(Q_UNLIKELY(res))
        JvmException::fromEnv(_jniEnv);
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

    try
    {
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
        _metadataHandler=MetadataHandler::build(_jniEnv,_stateMap);
        _modelBuilder=new MemoizerModelBuilder(_jniEnv);
        _memoizerModelHandlerClass=JvmClass::transferChecked(_jniEnv,_jniEnv->FindClass("MemoizerModelHandler"));
        _memoizerModelHandlerInitId=_jniEnv->GetMethodID(
                    _memoizerModelHandlerClass->unwrap(),
                    "<init>",
                    "(J)V");
        check(_memoizerModelHandlerInitId);
        _memoizerModelHandlerIdId=_jniEnv->GetFieldID(
                    _memoizerModelHandlerClass->unwrap(),
                    "id",
                    "J");
        check(_memoizerModelHandlerIdId);
        _memeBuilder=new MemeBuilder(_jniEnv);

        registerNatives();

        emit readyToUse();
        prepareDataForGui();
    }
    catch(const JvmException& e)
    {
        emit exceptionOccured(e);
        emit initializationFailed();
    }

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
