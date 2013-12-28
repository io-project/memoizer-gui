#include "MemoizerModelController.hxx"

#include "LocalRefsGuard.hxx"
#include "JvmException.hxx"
#include "MemeBuilder.hxx"
#include "Meme.hxx"

bool MemoizerModelController::pull(JNIEnv* jniEnv, jint id)
{
    LocalRefsGuard localRefs(jniEnv);
    bool result=jniEnv->CallBooleanMethod(
                unwrap(),
                _pullId,
                id);
    JvmException::checkEnv(jniEnv);
    return result;
}

Meme MemoizerModelController::get(JNIEnv* jniEnv, const MemeBuilder &memeBuilder, jint id)
{
    LocalRefsGuard localRefs(jniEnv);
    jobject meme=jniEnv->CallObjectMethod(
                unwrap(),
                _getId,
                id);
    JvmException::checkEnv(jniEnv);
    localRefs.insert(meme);
    return memeBuilder.build(jniEnv,meme);
}

MemoizerModelController::MemoizerModelController(JNIEnv *jniEnv, const jobject &object)
    : JvmObject<MemoizerModelController>(jniEnv,object)
{
}
