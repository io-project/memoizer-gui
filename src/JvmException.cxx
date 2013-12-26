#include "JvmException.hxx"

#include <list>
#include <iostream>

JvmException::JvmException() : std::runtime_error(nullptr)
{

}

JvmException::JvmException(const QString &message) :
    std::runtime_error(nullptr), _message(message),
    _localMessage(message.toLocal8Bit())
{
}

const char *JvmException::what() const noexcept
{
    return _localMessage;
}

QString JvmException::whatUnicode() const noexcept
{
    return _message;
}

void JvmException::checkEnv(JNIEnv *jniEnv)
{
    if(jniEnv->ExceptionCheck())
        throw fromEnv(jniEnv);
}

JvmException JvmException::fromEnv(JNIEnv *jniEnv)
{
    Q_CHECK_PTR(jniEnv);
    return wrapThrowable(jniEnv,jniEnv->ExceptionOccurred());
}

JvmException JvmException::wrapThrowable(JNIEnv *jniEnv, jobject throwable)
{
    Q_CHECK_PTR(jniEnv);
    // Dodatkowe narzędzia
    std::list<jobject> localObjects;
    auto deleteLocalRefs=[&localObjects,jniEnv](){
        for(auto o : localObjects)
            jniEnv->DeleteLocalRef(o);
        localObjects.clear();
    };

    // Obsługa wyjątku uniemożliwiającego obsługe tego wyjątku
#define EXCEPTION_EXCEPTION                                                     \
    do {                                                                        \
    QString message=QString::fromUtf8("Wystąpił wyjątek w maszynie wirtualnej, "\
                                      "ale nie udało się go obsłużyć")          \
                    .toLocal8Bit().constData();                                 \
    jniEnv->ExceptionDescribe();                                                \
    deleteLocalRefs();                                                          \
    return JvmException(message);                                                                     \
    } while(false)

    // Wydobywanie wiadomości
    Q_CHECK_PTR(throwable);
    jniEnv->ExceptionClear();
    localObjects.push_back(throwable);

    jclass stringWriterClass=jniEnv->FindClass("java/io/StringWriter");
    if(!stringWriterClass)
        EXCEPTION_EXCEPTION;
    localObjects.push_back(stringWriterClass);
    jmethodID stringWriterInitId=jniEnv->GetMethodID(stringWriterClass,"<init>","()V");
    if(!stringWriterInitId)
        EXCEPTION_EXCEPTION;
    jobject stringWriter=jniEnv->NewObject(stringWriterClass,stringWriterInitId);
    if(!stringWriter)
        EXCEPTION_EXCEPTION;
    localObjects.push_back(stringWriter);
    jclass printWriterClass=jniEnv->FindClass("java/io/PrintWriter");
    if(!printWriterClass)
        EXCEPTION_EXCEPTION;
    localObjects.push_back(printWriterClass);
    jmethodID printWriterInit=jniEnv->GetMethodID(
                printWriterClass,"<init>","(Ljava/io/Writer;)V");
    if(!printWriterInit)
        EXCEPTION_EXCEPTION;
    jobject printWriter=jniEnv->NewObject(printWriterClass,printWriterInit,stringWriter);
    if(!printWriter)
        EXCEPTION_EXCEPTION;
    localObjects.push_back(printWriter);
    jniEnv->DeleteLocalRef(printWriterClass);
    localObjects.remove(printWriterClass);
    jclass throwableClass=jniEnv->GetObjectClass(throwable);
    localObjects.push_back(throwableClass);
    jmethodID printStackTraceId=jniEnv->GetMethodID(
                throwableClass,
                "printStackTrace",
                "(Ljava/io/PrintWriter;)V");
    if(!printStackTraceId)
        EXCEPTION_EXCEPTION;
    jniEnv->CallVoidMethod(throwable,printStackTraceId,printWriter);
    if(jniEnv->ExceptionCheck())
        EXCEPTION_EXCEPTION;
    jniEnv->DeleteLocalRef(throwable);
    localObjects.remove(throwable);
    jniEnv->DeleteLocalRef(printWriter);
    localObjects.remove(printWriter);
    jniEnv->DeleteLocalRef(throwableClass);
    localObjects.remove(throwableClass);
    jmethodID toStringId=jniEnv->GetMethodID(
                stringWriterClass,"toString","()Ljava/lang/String;");
    if(!toStringId)
        EXCEPTION_EXCEPTION;

    // Tłumaczenie wiadomości
    jstring message=reinterpret_cast<jstring>(
                jniEnv->CallObjectMethod(stringWriter,toStringId));
    if(jniEnv->ExceptionCheck())
        EXCEPTION_EXCEPTION;
    deleteLocalRefs();
    localObjects.push_back(message);
    const jsize stringLength=jniEnv->GetStringLength(message);
    const jchar* string=jniEnv->GetStringCritical(message,nullptr);
    const QString result=QString::fromUtf16(string,stringLength);
    jniEnv->ReleaseStringCritical(message,string);
    deleteLocalRefs();
    return JvmException(result);
#undef EXCEPTION_EXCEPTION
}
