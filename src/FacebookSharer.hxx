#ifndef FACEBOOKSHARER_HXX
#define FACEBOOKSHARER_HXX

#include <memory>

#include <jni.h>

class JvmClass;
class Object;
class QUrl;

class FacebookSharer
{
public:
    FacebookSharer(JNIEnv *jniEnv);

    void share(JNIEnv* jniEnv, const QUrl& url);

private:
    std::shared_ptr<JvmClass> _facebookSharerClass;
    std::shared_ptr<Object> _facebookSharer;
    jmethodID _shareId;

    std::shared_ptr<JvmClass> _urlClass;
    jmethodID _urlInitId;
};

#endif // FACEBOOKSHARER_HXX
