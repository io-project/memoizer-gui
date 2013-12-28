#ifndef MEME_HXX
#define MEME_HXX

#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QUrl>

#include <jni.h>

class Meme
{
    friend class MemeBuilder;

public:
    QString title() const;
    QString description() const;
    QUrl imageLink() const;
    QUrl pageLink() const;

    void release(JNIEnv* jniEnv);

private:
    jobject _meme;

    QString _title;
    QString _desc;
    QUrl _imageLink;
    QUrl _pageLink;

};

Q_DECLARE_METATYPE(Meme);

#endif // MEME_HXX
