#include "MemoizerModelBuilder.hxx"

#include <QtCore/QString>
#include <QtCore/QStringList>

#include "LocalRefsGuard.hxx"
#include "ViewType.hxx"
#include "PluginManager.hxx"
#include "MetadataHandler.hxx"
#include "Utilities.hxx"
#include "JvmException.hxx"
#include "MemoizerModelController.hxx"

MemoizerModelBuilder::MemoizerModelBuilder(JNIEnv *jniEnv)
{
    Q_CHECK_PTR(jniEnv);
    LocalRefsGuard localRefs(jniEnv);
    _memoizerModelsClass=JvmClass::transferChecked(
                jniEnv,
                jniEnv->FindClass("pl/edu/uj/tcs/memoizer/gui/models/MemoizerModels"));
    _newMemoizerModelId=jniEnv->GetStaticMethodID(
                _memoizerModelsClass->unwrap(),
                "newMemoizerModel",
                "(Lpl/edu/uj/tcs/memoizer/plugins/EViewType;"
                "Ljava/util/List;"
                "Lpl/edu/uj/tcs/memoizer/plugins/communication/IPluginManagerClient;"
                "Lpl/edu/uj/tcs/memoizer/gui/MetadataHandler;"
                "Lpl/edu/uj/tcs/memoizer/gui/models/IMemoizerModelHandler;"
                ")Lpl/edu/uj/tcs/memoizer/gui/models/IMemoizerModelController;");
    localRefs.check(_newMemoizerModelId);
    _newMemoizerSearchModelId=jniEnv->GetStaticMethodID(
                _memoizerModelsClass->unwrap(),
                "newMemoizerSearchModel",
                "(Ljava/lang/String;"
                "Lpl/edu/uj/tcs/memoizer/plugins/communication/IPluginManagerClient;"
                "Ljava/util/List;"
                "Lpl/edu/uj/tcs/memoizer/gui/models/IMemoizerModelHandler;"
                ")Lpl/edu/uj/tcs/memoizer/gui/models/IMemoizerModelController;");
    localRefs.check(_newMemoizerModelId);

    _arrayListClass=JvmClass::transferChecked(
                jniEnv,
                jniEnv->FindClass("java/util/ArrayList"));
    _arrayListInitId=jniEnv->GetMethodID(
                _arrayListClass->unwrap(),
                "<init>",
                "()V");
    localRefs.check(_arrayListInitId);
    _arrayListAddId=jniEnv->GetMethodID(
                _arrayListClass->unwrap(),
                "add",
                "(Ljava/lang/Object;)Z");
    localRefs.check(_arrayListAddId);

    _memoizerModelControllerClass=JvmClass::transferChecked(
                jniEnv,
                jniEnv->FindClass("pl/edu/uj/tcs/memoizer/gui/models/IMemoizerModelController"));
    _memoizerModelControllerPullId=jniEnv->GetMethodID(
                _memoizerModelControllerClass->unwrap(),
                "pull",
                "(I)Z");
    localRefs.check(_memoizerModelControllerPullId);
    _memoizerModelControllerGetId=jniEnv->GetMethodID(
                _memoizerModelControllerClass->unwrap(),
                "get",
                "(I)Lpl/edu/uj/tcs/memoizer/plugins/Meme;");
    localRefs.check(_memoizerModelControllerGetId);
}

std::shared_ptr<MemoizerModelController> MemoizerModelBuilder::newMemoizerModel(
        JNIEnv* jniEnv,
        std::shared_ptr<const ViewType> viewType,
        const QStringList &pluginsNames,
        std::shared_ptr<const PluginManager> pluginManager,
        std::shared_ptr<const MetadataHandler> metadataHandler,
        jobject memoizerModelHandler)
{
    LocalRefsGuard localRefs(jniEnv);
    jobject pluginsNamesJ=jniEnv->NewObject(
                _arrayListClass->unwrap(),
                _arrayListInitId);
    localRefs.checkIn(pluginsNamesJ);
    for(const auto plugin : pluginsNames)
    {
        jstring pluginJ=toJString(jniEnv,plugin);
        localRefs.checkIn(pluginJ);
        jniEnv->CallBooleanMethod(
                    pluginsNamesJ,
                    _arrayListAddId,
                    pluginJ);
        JvmException::checkEnv(jniEnv);
        localRefs.remove(pluginJ);
    }
    auto result= MemoizerModelController::transferChecked(
                jniEnv,
                jniEnv->CallStaticObjectMethod(
                    _memoizerModelsClass->unwrap(),
                    _newMemoizerModelId,
                    viewType->unwrap(),
                    pluginsNamesJ,
                    pluginManager->unwrap(),
                    metadataHandler->unwrap(),
                    memoizerModelHandler));
    setupModelController(result);
    return result;
}

std::shared_ptr<MemoizerModelController> MemoizerModelBuilder::newMemoizerSearchModel(
        JNIEnv *jniEnv,
        const QString &searchKey,
        std::shared_ptr<const PluginManager> pluginManager,
        const QStringList &pluginsNames,
        jobject memoizerModelHandler)
{
    LocalRefsGuard localRefs(jniEnv);
    jobject pluginsNamesJ=jniEnv->NewObject(
                _arrayListClass->unwrap(),
                _arrayListInitId);
    localRefs.checkIn(pluginsNamesJ);
    for(const auto plugin : pluginsNames)
    {
        jstring pluginJ=toJString(jniEnv,plugin);
        localRefs.checkIn(pluginJ);
        jniEnv->CallBooleanMethod(
                    pluginsNamesJ,
                    _arrayListAddId,
                    pluginJ);
        JvmException::checkEnv(jniEnv);
        localRefs.remove(pluginJ);
    }
    jstring searchKeyJ=toJString(jniEnv,searchKey);
    localRefs.checkIn(searchKeyJ);
    auto result= MemoizerModelController::transferChecked(
                jniEnv,
                jniEnv->CallStaticObjectMethod(
                    _memoizerModelsClass->unwrap(),
                    _newMemoizerSearchModelId,
                    searchKeyJ,
                    pluginManager->unwrap(),
                    pluginsNamesJ,
                    memoizerModelHandler));
    setupModelController(result);
    return result;
}

void MemoizerModelBuilder::setupModelController(std::shared_ptr<MemoizerModelController> modelController)
{
    modelController->_memoizerModelControllerClass=_memoizerModelControllerClass;
    modelController->_pullId=_memoizerModelControllerPullId;
    modelController->_getId=_memoizerModelControllerGetId;
}
