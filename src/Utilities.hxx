#ifndef UTILITIES_HXX
#define UTILITIES_HXX

#include <QtCore/QString>

#include <jni.h>

QString toQString(JNIEnv* jniEnv,jstring string);

/**
 * @brief Zwraca lokalną referencje
 * @param jniEnv
 * @param string
 * @return
 */
jstring toJString(JNIEnv* jniEnv,const QString& string);

#endif // UTILITIES_HXX
