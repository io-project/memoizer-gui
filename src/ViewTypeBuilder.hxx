#ifndef VIEWTYPEBUILDER_HXX
#define VIEWTYPEBUILDER_HXX

#include <memory>

#include <jni.h>

class ViewType;
class JvmClass;
class Object;

class ViewTypeBuilder
{
public:
    ViewTypeBuilder(JNIEnv *jniEnv);

    std::shared_ptr<ViewType> transferChecked(jobject viewType);

private:
    std::shared_ptr<JvmClass> _viewTypeClass;
    jmethodID _getNameId;
    std::shared_ptr<Object> _search;

    JNIEnv* _jniEnv;
};

#endif // VIEWTYPEBUILDER_HXX
