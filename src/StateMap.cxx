#include "StateMap.hxx"

#include <QtCore/QStringList>

#include "LocalRefsGuard.hxx"
#include "Utilities.hxx"

QStringList StateMap::selectedPlugins(JNIEnv *jniEnv) const
{
    LocalRefsGuard localRefs(jniEnv);
    jobjectArray selectedJ;
    {
        LocalRefsGuard localRefsL(jniEnv);
        jclass stateMapsClass=jniEnv->FindClass("StateMaps");
        localRefsL.checkIn(stateMapsClass);
        jmethodID getSelectedPluginsId=jniEnv->GetStaticMethodID(
                    stateMapsClass,
                    "getSelectedPlugins",
                    "(Lpl/edu/uj/tcs/memoizer/serialization/StateMap;)"
                    "[Ljava/lang/String;");
        localRefsL.check(getSelectedPluginsId);
        selectedJ=reinterpret_cast<jobjectArray>(jniEnv->CallStaticObjectMethod(
                    stateMapsClass,
                    getSelectedPluginsId,
                    unwrap()));
        localRefs.checkIn(selectedJ);
    }
    QStringList result;
    Q_CHECK_PTR(selectedJ);
    jsize size=jniEnv->GetArrayLength(selectedJ);
    for(jsize i=0;i<size;++i)
    {
        jstring plugin=reinterpret_cast<jstring>(jniEnv->GetObjectArrayElement(selectedJ,i));
        JvmException::checkEnv(jniEnv); // null jest nie porządany, ale możliwy
        localRefs.insert(plugin);
        result<<toQString(jniEnv,plugin);
        localRefs.remove(plugin);
    }
    return result;
}

#include <QDebug>

void StateMap::setPluginsSelection(JNIEnv *jniEnv, const QStringList &selected, const QStringList &unselected)
{
    LocalRefsGuard localRefs(jniEnv);
    jobjectArray selectedJ,unselectedJ;
    {
        LocalRefsGuard localRefsL(jniEnv);
        jclass stringClass=jniEnv->FindClass("java/lang/String");
        localRefsL.checkIn(stringClass);
        selectedJ=jniEnv->NewObjectArray(selected.size(),stringClass,nullptr);
        localRefs.checkIn(selectedJ);
        for(jsize i=0;i<selected.size();++i)
        {
            jstring plugin=toJString(jniEnv,selected[i]);
            localRefsL.checkIn(plugin);
            jniEnv->SetObjectArrayElement(selectedJ,i,plugin);
            JvmException::checkEnv(jniEnv);
            localRefsL.remove(plugin);
        }
        unselectedJ=jniEnv->NewObjectArray(unselected.size(),stringClass,nullptr);
        localRefs.checkIn(unselectedJ);
        for(jsize i=0;i<unselected.size();++i)
        {
            jstring plugin=toJString(jniEnv,unselected[i]);
            localRefsL.checkIn(plugin);
            jniEnv->SetObjectArrayElement(unselectedJ,i,plugin);
            JvmException::checkEnv(jniEnv);
            localRefsL.remove(plugin);
        }
    }
    jclass stateMapsClass=jniEnv->FindClass("StateMaps");
    localRefs.checkIn(stateMapsClass);
    jmethodID setPluginsSelectionId=jniEnv->GetStaticMethodID(
                stateMapsClass,
                "setPluginsSelection",
                "(Lpl/edu/uj/tcs/memoizer/serialization/StateMap;"
                "[Ljava/lang/String;"
                "[Ljava/lang/String;)V");
    localRefs.check(setPluginsSelectionId);
    jniEnv->CallStaticVoidMethod(
                stateMapsClass,
                setPluginsSelectionId,
                unwrap(),
                selectedJ,
                unselectedJ);
    JvmException::checkEnv(jniEnv);
}

StateMap::StateMap(JNIEnv *jniEnv, jobject stateMap)
    : JvmObject<StateMap>(jniEnv,stateMap)
{

}
