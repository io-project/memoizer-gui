#ifndef VIRTUALMACHINE_HXX
#define VIRTUALMACHINE_HXX

#include <memory>

#include <QtCore/QObject>

#include <jni.h>

#include "ViewType.hxx"

class JvmException;
class StateMap;
class EventService;
class PluginManager;

class VirtualMachine : public QObject
{
    Q_OBJECT
public:
    explicit VirtualMachine(QObject *parent = 0);
    virtual ~VirtualMachine() override;

    void setVmProvider(jint (JNICALL *)(JavaVM **, void **, void *));

    void start();

    /**
     * @brief Odpowie emisją sygnału pluginsNamesForView()
     * @param viewType
     */
    void preparePluginsNamesForView(const ViewType& viewType);

    Q_INVOKABLE void stop();

signals:
    void started();
    void objectsCreated();
    void pluginsNames(const QStringList& pluginsNames);
    void viewsTypes(const QList<ViewType>& viewsTypes);
    void pluginsNamesForView(const QList<QString>& pluginsNames, const ViewType& viewType);
    void initializationFailed();
    void aboutToStop();
    void stopped();

    void exceptionOccured(const JvmException& exception);

private:
    void prepareCommonObjects();
    void prepareDataForGui();

private:
    JavaVM *_javaVm;
    JNIEnv *_jniEnv;

    jint (JNICALL * _CreateJavaVM)(JavaVM **, void **, void *);

    std::shared_ptr<StateMap> _stateMap;
    std::shared_ptr<EventService> _eventService;
    std::shared_ptr<PluginManager> _pluginManager;
};

#endif // VIRTUALMACHINE_HXX
