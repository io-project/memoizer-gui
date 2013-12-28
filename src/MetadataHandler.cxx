#include "MetadataHandler.hxx"

#include "LocalRefsGuard.hxx"

std::shared_ptr<MetadataHandler> MetadataHandler::build(JNIEnv *jniEnv, std::shared_ptr<StateMap> stateMap)
{
    LocalRefsGuard localRefs(jniEnv);
    jclass metadataHandlerClass=jniEnv->FindClass("pl/edu/uj/tcs/memoizer/gui/MetadataHandler");
    localRefs.checkIn(metadataHandlerClass);
    jmethodID metadataHandlerInitId=jniEnv->GetMethodID(
                metadataHandlerClass,
                "<init>",
                "(Lpl/edu/uj/tcs/memoizer/serialization/StateMap;)V");
    localRefs.check(metadataHandlerInitId);
    jobject metadataHandler=jniEnv->NewObject(
                metadataHandlerClass,
                metadataHandlerInitId,
                stateMap->unwrap());
    localRefs.checkIn(metadataHandler);
    return wrap(jniEnv,metadataHandler);
}

MetadataHandler::MetadataHandler(JNIEnv *jniEnv, const jobject &object)
    : JvmObject<MetadataHandler>(jniEnv,object)
{
}
