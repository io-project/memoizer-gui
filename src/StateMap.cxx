#include "StateMap.hxx"

#include <QtCore/QStringList>

QStringList StateMap::selectedPlugins(JNIEnv *jniEnv) const
{
    return QStringList("Demotywatory");
}

#include <QDebug>

void StateMap::setPluginsSelection(JNIEnv *jniEnv, const QStringList &selected, const QStringList &unselected)
{
    qDebug()<<"Zapis wybranych pluginów:"<<selected<<"i nie wybranych"<<unselected;
}

StateMap::StateMap(JNIEnv *jniEnv, jobject stateMap)
    : JvmObject<StateMap>(jniEnv,stateMap)
{

}
