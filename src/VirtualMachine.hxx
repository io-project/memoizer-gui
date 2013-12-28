#ifndef VIRTUALMACHINE_HXX
#define VIRTUALMACHINE_HXX

#include <memory>
#include <unordered_map>
#include <vector>

#include <QtCore/QObject>
#include <QtCore/QHash>

#include <jni.h>

class JvmException;
class StateMap;
class EventService;
class PluginManager;
class ViewType;
class MemoizerModel;
class MetadataHandler;
class MemoizerModelController;
class MemoizerModelBuilder;
class JvmClass;
class MemeBuilder;
class Meme;

class VirtualMachine : public QObject
{
    Q_OBJECT
    friend void JNICALL notifyUpdate(
            JNIEnv* jniEnv,
            jobject memoizerModelHandler,
            jobject meme,
            jint id);
    friend void JNICALL notifyStreamEnd(
            JNIEnv* jniEnv,
            jobject memoizerModelHandler);

public:
    explicit VirtualMachine(QObject *parent = 0);
    virtual ~VirtualMachine() override;

    void setVmProvider(jint (JNICALL *)(JavaVM **, void **, void *));

    void start();

    Q_INVOKABLE void prepareModel(QObject *receiver,std::size_t requestId, std::weak_ptr<const ViewType> viewType,const QStringList& pluginsNames);
    Q_INVOKABLE void prepareSearchModel(QObject *receiver,std::size_t requestId,const QStringList& pluginsNames, const QString& searchKey);

    Q_INVOKABLE void releaseModel(MemoizerModel* model);
    Q_INVOKABLE void pull(MemoizerModel* model,jint id);

    Q_INVOKABLE void storePluginsSelection(const QStringList& selected, const QStringList& unselected);
    Q_INVOKABLE void stop();

signals:
    void started();
    void readyToUse();
    void pluginsNames(const QStringList& pluginsNames,const QStringList& selectedPlugins);
    void viewsTypes(const QList<std::weak_ptr<const ViewType>>& viewsTypes);
    void pluginsNamesForView(const QStringList& pluginsNames, std::weak_ptr<const ViewType> viewType);
    void initializationComplete(bool havePlugins);
    void initializationFailed();
    void aboutToStop();
    void stopped();

    void exceptionOccured(const JvmException& exception);

private:
    void registerNatives();
    void prepareCommonObjects();
    void prepareDataForGui();

private:
    JavaVM *_javaVm;
    JNIEnv *_jniEnv;

    jint (JNICALL * _CreateJavaVM)(JavaVM **, void **, void *);

    std::shared_ptr<StateMap> _stateMap;
    std::shared_ptr<EventService> _eventService;
    std::shared_ptr<PluginManager> _pluginManager;
    std::shared_ptr<MetadataHandler> _metadataHandler;

    jlong _nextId;
    std::unordered_map<MemoizerModel*,jlong> _memoizerModelToHandlerId;
    std::unordered_map<MemoizerModel*,std::shared_ptr<MemoizerModelController>> _memoizerModelToController;
    std::unordered_map<MemoizerModel*,std::vector<Meme>> _memoizerModelToMemeList;
    std::unordered_map<jlong,MemoizerModel*> _handlerIdToMemoizerModel;
    MemoizerModelBuilder* _modelBuilder;
    MemeBuilder* _memeBuilder;
    std::shared_ptr<JvmClass> _memoizerModelHandlerClass;
    jmethodID _memoizerModelHandlerInitId;
    jfieldID _memoizerModelHandlerIdId;

    // "Żywy" odnośnik do typu wyliczeniowego (GUI otrzymuje "miękką" kopie)
    QList<std::shared_ptr<ViewType>> _allViewTypes;
};

#endif // VIRTUALMACHINE_HXX
