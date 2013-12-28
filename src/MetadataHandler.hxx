#ifndef METADATAHANDLER_HXX
#define METADATAHANDLER_HXX

#include "JvmObject.hxx"

#include <memory>

#include <jni.h>

#include "StateMap.hxx"

class MetadataHandler : public JvmObject<MetadataHandler>
{
    friend class JvmObject<MetadataHandler>;

public:
    static std::shared_ptr<MetadataHandler> build(JNIEnv* jniEnv, std::shared_ptr<StateMap> stateMap);

private:
    MetadataHandler(JNIEnv *jniEnv, const jobject& object);

private:

};

#endif // METADATAHANDLER_HXX
