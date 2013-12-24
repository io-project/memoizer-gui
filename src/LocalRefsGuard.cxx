#include "LocalRefsGuard.hxx"

#include "JvmException.hxx"

LocalRefsGuard::LocalRefsGuard(JNIEnv *jniEnv) : _jniEnv(jniEnv)
{
}

LocalRefsGuard::~LocalRefsGuard()
{
    removeAll();
}

void LocalRefsGuard::insert(jobject ref)
{
    _refs.push_back(ref);
}

void LocalRefsGuard::remove(jobject ref)
{
    _jniEnv->DeleteLocalRef(ref);
    _refs.remove(ref);
}

void LocalRefsGuard::removeAll()
{
    for(const auto ref : _refs)
        _jniEnv->DeleteLocalRef(ref);
    _refs.clear();
}

void LocalRefsGuard::checkIn(jobject ref)
{
    if(!ref)
        throw JvmException::fromEnv(_jniEnv);
    insert(ref);
}

jobject LocalRefsGuard::toGlobalChecked(jobject ref)
{
    if(!ref)
        throw JvmException::fromEnv(_jniEnv);
    jobject r=_jniEnv->NewGlobalRef(ref);
    if(!r)
    {
        _jniEnv->DeleteLocalRef(ref);
        throw JvmException::fromEnv(_jniEnv);
    }
    return r;
}


