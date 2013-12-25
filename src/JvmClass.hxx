#ifndef JVMCLASS_HXX
#define JVMCLASS_HXX

#include "JvmObject.hxx"

class JvmClass : public JvmObject<JvmClass,jclass>
{
    friend class JvmObject<JvmClass,jclass>;
public:

private:
    JvmClass(JNIEnv *jniEnv, const jclass& clazz);

};

#endif // JVMCLASS_HXX
