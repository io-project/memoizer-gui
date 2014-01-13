#ifndef METASHARER_HXX
#define METASHARER_HXX

#include <memory>

#include <jni.h>

class JvmClass;
class Object;
class QUrl;

class MetaSharer
{
public:
    MetaSharer(JNIEnv *jniEnv);

    void shareOnFacebook(JNIEnv* jniEnv, const QUrl& url);
    void shareOnTwitter(JNIEnv* jniEnv, const QUrl& url);

private:
    /**
     * @brief share Udostępnia przy użyciu wskazanej metody.
     * @param jniEnv
     * @param url
     * @param method Metoda która zostanie wykorzystana w celu udostępnienia.
     */
    void share(JNIEnv* jniEnv, const QUrl& url, jmethodID method);

private:
    std::shared_ptr<Object> _metaSharer;
    jmethodID _shareOnFacebookId;
    jmethodID _shareOnTwitterId;

    std::shared_ptr<JvmClass> _urlClass;
    jmethodID _urlInitId;
};

#endif // METASHARER_HXX
