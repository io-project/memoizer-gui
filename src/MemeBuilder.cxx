#include "MemeBuilder.hxx"

#include <QtCore/QUrl>

#include "Meme.hxx"
#include "LocalRefsGuard.hxx"
#include "JvmClass.hxx"
#include "Utilities.hxx"

MemeBuilder::MemeBuilder(JNIEnv *jniEnv)
{
    LocalRefsGuard localRefs(jniEnv);
    _memeClass=JvmClass::transferChecked(
                jniEnv,
                jniEnv->FindClass("pl/edu/uj/tcs/memoizer/plugins/Meme"));
    _getDescriptionId=jniEnv->GetMethodID(
                _memeClass->unwrap(),
                "getDescription",
                "()Ljava/lang/String;");
    localRefs.check(_getDescriptionId);
    _getTitleId=jniEnv->GetMethodID(
                _memeClass->unwrap(),
                "getTitle",
                "()Ljava/lang/String;");
    localRefs.check(_getTitleId);
    _getImageLinkId=jniEnv->GetMethodID(
                _memeClass->unwrap(),
                "getImageLink",
                "()Ljava/net/URL;");
    localRefs.check(_getImageLinkId);
    _getPageLinkId=jniEnv->GetMethodID(
                _memeClass->unwrap(),
                "getPageLink",
                "()Ljava/net/URL;");
    localRefs.check(_getPageLinkId);

    _urlClass=JvmClass::transferChecked(
                jniEnv,
                jniEnv->FindClass("java/net/URL"));
    _urlToExternalFormId=jniEnv->GetMethodID(
                _urlClass->unwrap(),
                "toExternalForm",
                "()Ljava/lang/String;");
    localRefs.check(_urlToExternalFormId);
}

Meme MemeBuilder::build(JNIEnv *jniEnv, jobject meme) const
{
    LocalRefsGuard localRefs(jniEnv);
    Meme result;
    result._meme=jniEnv->NewGlobalRef(meme);
    localRefs.check(result._meme);
    {
        jobject description=jniEnv->CallObjectMethod(
                    meme,
                    _getDescriptionId);
        localRefs.checkIn(description);
        result._desc=toQString(jniEnv,reinterpret_cast<jstring>(description));
        localRefs.remove(description);
    }
    {
        jobject title=jniEnv->CallObjectMethod(
                    meme,
                    _getTitleId);
        localRefs.checkIn(title);
        result._title=toQString(jniEnv,reinterpret_cast<jstring>(title));
        localRefs.remove(title);
    }
    {
        jobject imageLink=jniEnv->CallObjectMethod(
                    meme,
                    _getImageLinkId);
        localRefs.checkIn(imageLink);
        jobject imageLinkString=jniEnv->CallObjectMethod(
                    imageLink,
                    _urlToExternalFormId);
        localRefs.checkIn(imageLinkString);
        localRefs.remove(imageLink);
        result._imageLink=QUrl(toQString(jniEnv,reinterpret_cast<jstring>(imageLinkString)));
        localRefs.remove(imageLinkString);
    }
    {
        jobject pageLink=jniEnv->CallObjectMethod(
                    meme,
                    _getPageLinkId);
        localRefs.checkIn(pageLink);
        jobject pageLinkString=jniEnv->CallObjectMethod(
                    pageLink,
                    _urlToExternalFormId);
        localRefs.checkIn(pageLinkString);
        localRefs.remove(pageLink);
        result._pageLink=QUrl(toQString(jniEnv,reinterpret_cast<jstring>(pageLinkString)));
        localRefs.remove(pageLinkString);
    }
    return result;
}
