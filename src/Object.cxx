#include "Object.hxx"

Object::Object(JNIEnv *jniEnv, const jobject &object)
    : JvmObject<Object>(jniEnv,object)
{
}
