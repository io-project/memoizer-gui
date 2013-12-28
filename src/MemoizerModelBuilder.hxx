#ifndef MEMOIZERMODELBUILDER_HXX
#define MEMOIZERMODELBUILDER_HXX

#include <memory>

#include <jni.h>

#include "JvmClass.hxx"

class MemoizerModelController;
class ViewType;
class PluginManager;
class MetadataHandler;

class MemoizerModelBuilder
{
public:
    MemoizerModelBuilder(JNIEnv* jniEnv);

    std::shared_ptr<MemoizerModelController> newMemoizerModel(
            JNIEnv *jniEnv,
            std::shared_ptr<const ViewType> viewType,
            const QStringList& pluginsNames,
            std::shared_ptr<const PluginManager> pluginManager,
            std::shared_ptr<const MetadataHandler> metadataHandler,
            jobject memoizerModelHandler);

    std::shared_ptr<MemoizerModelController> newMemoizerSearchModel(
            JNIEnv* jniEnv,
            const QString& searchKey,
            std::shared_ptr<const PluginManager> pluginManager,
            const QStringList& pluginsNames,
            jobject memoizerModelHandler);

private:
    void setupModelController(std::shared_ptr<MemoizerModelController> modelController);

private:
    std::shared_ptr<JvmClass> _memoizerModelsClass;
    jmethodID _newMemoizerModelId;
    jmethodID _newMemoizerSearchModelId;

    std::shared_ptr<JvmClass> _arrayListClass;
    jmethodID _arrayListInitId;
    jmethodID _arrayListAddId;

    std::shared_ptr<JvmClass> _memoizerModelControllerClass;
    jmethodID _memoizerModelControllerPullId;
    jmethodID _memoizerModelControllerGetId;
};

#endif // MEMOIZERMODELBUILDER_HXX
