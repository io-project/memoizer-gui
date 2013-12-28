#include "Meme.hxx"

QString Meme::title() const
{
    return _title;
}

QString Meme::description() const
{
    return _desc;
}

QUrl Meme::imageLink() const
{
    return _imageLink;
}

void Meme::release(JNIEnv *jniEnv)
{
    jniEnv->DeleteGlobalRef(_meme);
}
