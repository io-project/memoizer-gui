#ifndef PLUGINMANAGER_HXX
#define PLUGINMANAGER_HXX

#include "JvmObject.hxx"

#include <mutex>

#include <QtCore/QStringList>

class PluginManager : public JvmObject<PluginManager>
{
    friend class JvmObject<PluginManager>;

public:
    QStringList getAllPluginNames(JNIEnv *jniEnv);

private:
    PluginManager(jobject pluginManager);

    virtual void initialize(JNIEnv* jniEnv) override;
    virtual void deinitialize(JNIEnv* jniEnv) override;

private:
    jclass _pluginManagerClass;
    jmethodID _getAllPluginNamesId;

    jclass _listClass;
    jmethodID _iteratorId;

    // Iterator
    jmethodID _hasNextId;
    jmethodID _nextId;
};

#endif // PLUGINMANAGER_HXX
