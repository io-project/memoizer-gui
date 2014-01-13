#include "MetaSharer.hxx"

#include <QtCore/QUrl>

#include "LocalRefsGuard.hxx"
#include "JvmClass.hxx"
#include "Object.hxx"
#include "Utilities.hxx"
#include "JvmException.hxx"

MetaSharer::MetaSharer(JNIEnv* jniEnv)
{
    LocalRefsGuard localRefs(jniEnv);
    std::shared_ptr<JvmClass> metaSharerClass=JvmClass::transferChecked(
                jniEnv,
                jniEnv->FindClass("MetaSharer"));
    _shareOnFacebookId=jniEnv->GetMethodID(
                metaSharerClass->unwrap(),
                "shareOnFacebook",
                "(Ljava/net/URL;)V");
    localRefs.check(_shareOnFacebookId);
    _shareOnTwitterId=jniEnv->GetMethodID(
                metaSharerClass->unwrap(),
                "shareOnTwitter",
                "(Ljava/net/URL;)V");
    localRefs.check(_shareOnTwitterId);
    jmethodID defaultMetaSharerId=jniEnv->GetStaticMethodID(
                metaSharerClass->unwrap(),
                "defaultMetaSharer",
                "()LMetaSharer;");
    localRefs.check(defaultMetaSharerId);
    _metaSharer=Object::transferChecked(
                jniEnv,
                jniEnv->CallStaticObjectMethod(
                    metaSharerClass->unwrap(),
                    defaultMetaSharerId));
    metaSharerClass.reset();

    _urlClass=JvmClass::transferChecked(
                jniEnv,
                jniEnv->FindClass("java/net/URL"));
    _urlInitId=jniEnv->GetMethodID(
                _urlClass->unwrap(),
                "<init>",
                "(Ljava/lang/String;)V");
    localRefs.check(_urlInitId);
}

void MetaSharer::shareOnFacebook(JNIEnv *jniEnv, const QUrl &url)
{
    share(jniEnv,url,_shareOnFacebookId);
}

void MetaSharer::shareOnTwitter(JNIEnv *jniEnv, const QUrl &url)
{
  share(jniEnv,url,_shareOnTwitterId);
}

void MetaSharer::share(JNIEnv *jniEnv, const QUrl &url, jmethodID method)
{
    LocalRefsGuard localRefs(jniEnv);
    jstring urlString=toJString(jniEnv,url.url());
    localRefs.checkIn(urlString);
    jobject jUrl=jniEnv->NewObject(
                _urlClass->unwrap(),
                _urlInitId,
                urlString);
    localRefs.checkIn(jUrl);
    localRefs.remove(urlString);
    jniEnv->CallVoidMethod(
                _metaSharer->unwrap(),
                method,
                jUrl);
    JvmException::checkEnv(jniEnv);
}
