#ifndef PLUGINMANAGER_HXX
#define PLUGINMANAGER_HXX

#include "JvmObject.hxx"

#include <memory>

#include <QtCore/QList>
#include <QtCore/QStringList>

class ViewType;

class PluginManager : public JvmObject<PluginManager>
{
    friend class JvmObject<PluginManager>;

public:
    QStringList getAllPluginNames(JNIEnv *jniEnv);
    QList<std::shared_ptr<ViewType> > getAvailableViews(JNIEnv *jniEnv);
    QStringList getPluginsNamesForView(JNIEnv *jniEnv, std::shared_ptr<const ViewType> viewType);

private:
    PluginManager(JNIEnv *jniEnv, jobject pluginManager);

    virtual void initialize(JNIEnv* jniEnv) override;
    virtual void deinitialize(JNIEnv* jniEnv) override;

private:
    jclass _pluginManagerClass;
    jmethodID _getAllPluginNamesId;
    jmethodID _getAvailableViews;
    jmethodID _getPluginsNamesForView;

    jclass _listClass;
    jmethodID _iteratorId;

    // Iterator
    jmethodID _hasNextId;
    jmethodID _nextId;
};

#endif // PLUGINMANAGER_HXX
