#include "ViewType.hxx"

#include "LocalRefsGuard.hxx"
#include "JvmException.hxx"
#include "Utilities.hxx"

QString ViewType::getName() const
{
    return _name;
}

bool ViewType::isSearch() const
{
    return _isSearch;
}

ViewType::ViewType(JNIEnv *jniEnv, jobject viewType, const QString &name,bool isSearch)
    : JvmObject<ViewType>(jniEnv,viewType), _name(name),_isSearch(isSearch)
{
}

ViewType::ViewType(JNIEnv *jniEnv, jobject viewType, QString &&name,bool isSearch)
    : JvmObject<ViewType>(jniEnv,viewType), _name(name),_isSearch(isSearch)
{

}
