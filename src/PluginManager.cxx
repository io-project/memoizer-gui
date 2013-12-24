#include "PluginManager.hxx"

#include "LocalRefsGuard.hxx"
#include "JvmException.hxx"

QStringList PluginManager::getAllPluginNames(JNIEnv *jniEnv)
{
    ensureInitialized(jniEnv);
    LocalRefsGuard localRefs(jniEnv);
    QStringList result;
    jobject namesList=jniEnv->CallObjectMethod(unwrap(),_getAllPluginNamesId);
    localRefs.checkIn(namesList);
    jobject iterator=jniEnv->CallObjectMethod(namesList,_iteratorId);
    localRefs.checkIn(iterator);
    localRefs.remove(namesList);
    for(;;)
    {
        jboolean hn=jniEnv->CallBooleanMethod(iterator,_hasNextId);
        JvmException::checkEnv(jniEnv);
        if(!hn)
            break;
        jobject n=jniEnv->CallObjectMethod(iterator,_nextId);
        localRefs.checkIn(n);
        jsize nlen=jniEnv->GetStringLength(reinterpret_cast<jstring>(n));
        const jchar *nchars=jniEnv->GetStringCritical(reinterpret_cast<jstring>(n),nullptr);
        result<<QString::fromUtf16(nchars,nlen);
        jniEnv->ReleaseStringCritical(reinterpret_cast<jstring>(n),nchars);
        // Tutaj można pobrać dodatkowo ikone
        localRefs.remove(n);
    }
    return result;
}

PluginManager::PluginManager(jobject pluginManager)
    : JvmObject<PluginManager>(pluginManager), _pluginManagerClass(nullptr),
      _listClass(nullptr)
{
}

void PluginManager::initialize(JNIEnv *jniEnv)
{
    LocalRefsGuard localRefs(jniEnv);
    auto check=[jniEnv](void* o) {
        if(!o)
            throw JvmException::fromEnv(jniEnv);
    };

    // IPluginManagerClient
    jclass clazz=jniEnv->GetObjectClass(unwrap());
    if(!clazz)
        throw JvmException::fromEnv(jniEnv);
    localRefs.insert(clazz);
    _pluginManagerClass=reinterpret_cast<jclass>(jniEnv->NewGlobalRef(clazz));
    if(!_pluginManagerClass)
        throw JvmException::fromEnv(jniEnv);
    localRefs.remove(clazz);
    _getAllPluginNamesId=jniEnv->GetMethodID(
                _pluginManagerClass,
                "getAllPluginNames",
                "()Ljava/util/List;");
    if(!_getAllPluginNamesId)
        throw JvmException::fromEnv(jniEnv);

    // java.util.List
    clazz=jniEnv->FindClass("java/util/List");
    if(!clazz)
        throw JvmException::fromEnv(jniEnv);
    localRefs.insert(clazz);
    _listClass=reinterpret_cast<jclass>(jniEnv->NewGlobalRef(clazz));
    if(!_listClass)
        throw JvmException::fromEnv(jniEnv);
    localRefs.remove(clazz);
    _iteratorId=jniEnv->GetMethodID(
                _listClass,
                "iterator",
                "()Ljava/util/Iterator;");
    if(!_iteratorId)
        throw JvmException::fromEnv(jniEnv);

    // java.util.Iterator
    clazz=jniEnv->FindClass("java/util/Iterator");
    localRefs.checkIn(clazz);
    _hasNextId=jniEnv->GetMethodID(clazz,"hasNext","()Z");
    check(_hasNextId);
    _nextId=jniEnv->GetMethodID(clazz,"next","()Ljava/lang/Object;");
    check(_nextId);
}

void PluginManager::deinitialize(JNIEnv *jniEnv)
{
    jniEnv->DeleteGlobalRef(_pluginManagerClass);
    jniEnv->DeleteGlobalRef(_listClass);
}

