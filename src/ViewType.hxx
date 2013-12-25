#ifndef VIEWTYPE_HXX
#define VIEWTYPE_HXX

#include "JvmObject.hxx"

#include <QtCore/QList>
#include <QtCore/QMetaType>

class ViewType : public JvmObject<ViewType>
{
    friend class JvmObject<ViewType>;

public:
    QString getName() const;
    bool isSearch() const;

private:
    ViewType(JNIEnv *jniEnv, jobject viewType, const QString& name,bool isSearch);
    ViewType(JNIEnv *jniEnv, jobject viewType, QString&& name, bool isSearch);

private:
    QString _name;
    bool _isSearch;
};

Q_DECLARE_METATYPE(std::weak_ptr<const ViewType>);
Q_DECLARE_METATYPE(QList<std::weak_ptr<const ViewType>>);

#endif // VIEWTYPE_HXX
