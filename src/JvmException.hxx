#ifndef JVMEXCEPTION_HXX
#define JVMEXCEPTION_HXX

#include <stdexcept>

#include <QtCore/QMetaType>
#include <QtCore/QString>

#include <jni.h>

class JvmException : public std::runtime_error
{
public:
    explicit JvmException();
    explicit JvmException(const QString& message);

    virtual const char* what() const noexcept override;
    QString whatUnicode() const noexcept;

    static void checkEnv(JNIEnv *jniEnv);
    static JvmException fromEnv(JNIEnv *jniEnv);
    static JvmException wrapThrowable(JNIEnv *jniEnv, jobject throwable);

private:

private:
    const QString _message;
    const QByteArray _localMessage;
};

Q_DECLARE_METATYPE(JvmException);

#endif // JVMEXCEPTION_HXX
