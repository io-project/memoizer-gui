#include "JvmClass.hxx"

JvmClass::JvmClass(JNIEnv *jniEnv, const jclass &clazz)
    : JvmObject<JvmClass,jclass>(jniEnv,clazz)
{

}
