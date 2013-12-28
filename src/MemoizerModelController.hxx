#ifndef MEMOIZERMODELCONTROLLER_HXX
#define MEMOIZERMODELCONTROLLER_HXX

#include "JvmObject.hxx"

#include <jni.h>

class Meme;
class MemeBuilder;
class JvmClass;

class MemoizerModelController : public JvmObject<MemoizerModelController>
{
    friend class JvmObject<MemoizerModelController>;
    friend class MemoizerModelBuilder;

public:
    bool pull(JNIEnv *jniEnv, jint id);
    Meme get(JNIEnv *JNIEnv, const MemeBuilder& memeBuilder, jint id);

private:
    MemoizerModelController(JNIEnv *jniEnv, const jobject& object);

private:
    std::shared_ptr<JvmClass> _memoizerModelControllerClass;
    jmethodID _pullId;
    jmethodID _getId;
};

#endif // MEMOIZERMODELCONTROLLER_HXX
