#ifndef MEMEBUILDER_HXX
#define MEMEBUILDER_HXX

#include <memory>

#include <jni.h>

class Meme;
class JvmClass;

class MemeBuilder
{
public:
    MemeBuilder(JNIEnv* jniEnv);

    Meme build(JNIEnv* jniEnv,jobject meme) const;

private:
    std::shared_ptr<JvmClass> _memeClass;
    jmethodID _getDescriptionId;
    jmethodID _getTitleId;
    jmethodID _getImageLinkId;
    jmethodID _getPageLinkId;

    std::shared_ptr<JvmClass> _urlClass;
    jmethodID _urlToExternalFormId;
};

#endif // MEMEBUILDER_HXX
