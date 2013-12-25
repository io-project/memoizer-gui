#ifndef UTILITIES_HXX
#define UTILITIES_HXX

#include <QtCore/QString>

#include <jni.h>

QString toQString(JNIEnv* jniEnv,jstring string);

#endif // UTILITIES_HXX
