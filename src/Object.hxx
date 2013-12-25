#ifndef OBJECT_HXX
#define OBJECT_HXX

#include <memory>

#include "JvmObject.hxx"

class Object : public JvmObject<Object>
{
    friend class JvmObject<Object>;

public:
    template<class T,class U>
    static bool isSame(JNIEnv* jniEnv, T lhs,U rhs)
    {
        return jniEnv->IsSameObject(lhs->unwrap(),rhs->unwrap());
    }

private:
    Object(JNIEnv *jniEnv, const jobject& object);
};

#endif // OBJECT_HXX
