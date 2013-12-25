#ifndef STATEMAP_HXX
#define STATEMAP_HXX

#include "JvmObject.hxx"

class StateMap : public JvmObject<StateMap>
{
    friend class JvmObject<StateMap>;

public:
    QStringList selectedPlugins(JNIEnv *jniEnv) const;
    void setPluginsSelection(JNIEnv *jniEnv,const QStringList &selected, const QStringList& unselected);

private:
    StateMap(JNIEnv *jniEnv, jobject stateMap);
};

#endif // STATEMAP_HXX
