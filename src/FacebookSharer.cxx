#include "FacebookSharer.hxx"

#include <QtCore/QUrl>

#include "LocalRefsGuard.hxx"
#include "JvmClass.hxx"
#include "Object.hxx"
#include "Utilities.hxx"
#include "JvmException.hxx"

FacebookSharer::FacebookSharer(JNIEnv* jniEnv)
{
    LocalRefsGuard localRefs(jniEnv);
    _facebookSharerClass=JvmClass::transferChecked(
                jniEnv,
                jniEnv->FindClass("pl/edu/uj/tcs/memoizer/sharing/FacebookSharer"));
    _shareId=jniEnv->GetMethodID(
                _facebookSharerClass->unwrap(),
                "share",
                "(Ljava/net/URL;)V");
    localRefs.check(_shareId);
    jmethodID facebookSharerInitId=jniEnv->GetMethodID(
                _facebookSharerClass->unwrap(),
                "<init>",
                "()V");
    localRefs.check(facebookSharerInitId);
    _facebookSharer=Object::transferChecked(
                jniEnv,
                jniEnv->NewObject(
                    _facebookSharerClass->unwrap(),
                    facebookSharerInitId));

    _urlClass=JvmClass::transferChecked(
                jniEnv,
                jniEnv->FindClass("java/net/URL"));
    _urlInitId=jniEnv->GetMethodID(
                _urlClass->unwrap(),
                "<init>",
                "(Ljava/lang/String;)V");
    localRefs.check(_urlInitId);
}

void FacebookSharer::share(JNIEnv *jniEnv, const QUrl &url)
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
                _facebookSharer->unwrap(),
                _shareId,
                jUrl);
    JvmException::checkEnv(jniEnv);
}
