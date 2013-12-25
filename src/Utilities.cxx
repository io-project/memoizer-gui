#include "Utilities.hxx"

#include "LocalRefsGuard.hxx"

QString toQString(JNIEnv *jniEnv, jstring string)
{
    Q_CHECK_PTR(jniEnv);
    Q_CHECK_PTR(string);
    jsize len=jniEnv->GetStringLength(string);
    const jchar* chars=jniEnv->GetStringCritical(string,nullptr);
    QString result=QString::fromUtf16(chars,len);
    jniEnv->ReleaseStringCritical(string,chars);
    return result;
}


jstring toJString(JNIEnv *jniEnv, const QString &string)
{
    Q_CHECK_PTR(jniEnv);
    return jniEnv->NewString(string.utf16(),string.length());
}
