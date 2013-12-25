#include "ViewTypeBuilder.hxx"

#include "LocalRefsGuard.hxx"
#include "JvmClass.hxx"
#include "Utilities.hxx"
#include "ViewType.hxx"
#include "Object.hxx"

ViewTypeBuilder::ViewTypeBuilder(JNIEnv* jniEnv) : _jniEnv(jniEnv)
{
    LocalRefsGuard localRefs(jniEnv);
    _viewTypeClass=JvmClass::transferChecked(
                jniEnv,
                jniEnv->FindClass("pl/edu/uj/tcs/memoizer/plugins/EViewType"));
    _getNameId=jniEnv->GetMethodID(_viewTypeClass->unwrap(),"getName","()Ljava/lang/String;");
    localRefs.check(_getNameId);
    auto searchField=jniEnv->GetStaticFieldID(
                _viewTypeClass->unwrap(),
                "SEARCH",
                "Lpl/edu/uj/tcs/memoizer/plugins/EViewType;");
    localRefs.check(searchField);
    _search=Object::transferChecked(
                jniEnv,
                jniEnv->GetStaticObjectField(_viewTypeClass->unwrap(),searchField));
}

std::shared_ptr<ViewType> ViewTypeBuilder::transferChecked(jobject viewType)
{
    LocalRefsGuard localRefs(_jniEnv);
    localRefs.checkIn(viewType);
    jstring n=reinterpret_cast<jstring>(_jniEnv->CallObjectMethod(viewType,_getNameId));
    localRefs.checkIn(n);
    auto object=Object::wrap(_jniEnv,viewType);
    return ViewType::wrap(_jniEnv,
                          viewType,
                          toQString(_jniEnv,n),
                          Object::isSame(_jniEnv,object,_search));
}
