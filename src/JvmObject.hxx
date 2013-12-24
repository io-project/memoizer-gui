#ifndef JVMOBJECT_HXX
#define JVMOBJECT_HXX

#include <memory>

#include <QtCore/QtGlobal>

#include <jni.h>

template<class T>
class JvmObject
{
    Q_DISABLE_COPY(JvmObject)
public:

    /**
     * @brief Tworzy nowy obiekt przechowujący globalną referencję i zwraca
     * wskaźnik do niego.
     * @param javaVm
     * @param jniEnv
     * @param stateMap lokalna referencja
     * @return
     */
    static std::shared_ptr<T> wrap(JNIEnv *jniEnv, jobject object);

    jobject unwrap() const;

protected:
    JvmObject(jobject stateMap);
    virtual ~JvmObject();
    void release(JNIEnv *jniEnv);

    void ensureInitialized(JNIEnv *jniEnv);

private:
    virtual void initialize(JNIEnv *jniEnv);
    virtual void deinitialize(JNIEnv *jniEnv);

private:
    jobject _object;  // Globalna referencja
    bool _isCacheInitialized; // Klasy pochodne mogą mieć cache
};

#include "JvmException.hxx"

template<class T>
std::shared_ptr<T> JvmObject<T>::wrap(JNIEnv *jniEnv, jobject object)
{
    Q_CHECK_PTR(jniEnv);
    Q_CHECK_PTR(object);
    jobject objectG=jniEnv->NewGlobalRef(object);
    if(!objectG)
        throw JvmException::fromEnv(jniEnv);
    return std::shared_ptr<T>(new T(objectG),[jniEnv](T *o) {
        o->release(jniEnv);
    });
}

template<class T>
jobject JvmObject<T>::unwrap() const
{
    return _object;
}

template<class T>
JvmObject<T>::~JvmObject()
{
}

template<class T>
void JvmObject<T>::release(JNIEnv *jniEnv)
{
    Q_CHECK_PTR(jniEnv);
    Q_CHECK_PTR(_object);
    jniEnv->DeleteGlobalRef(_object);
    if(_isCacheInitialized)
        deinitialize(jniEnv);
}

template<class T>
void JvmObject<T>::ensureInitialized(JNIEnv *jniEnv)
{
    if(_isCacheInitialized)
        return;
    _isCacheInitialized=true;
    initialize(jniEnv);
}

template<class T>
void JvmObject<T>::initialize(JNIEnv *jniEnv)
{
    Q_UNUSED(jniEnv);
    // Domyślna implementacja nic nie robi
}

template<class T>
void JvmObject<T>::deinitialize(JNIEnv *jniEnv)
{
    Q_UNUSED(jniEnv);
    // Domyślna implementacja nic nie robi
}

template<class T>
JvmObject<T>::JvmObject(jobject object) :
     _object(object), _isCacheInitialized(false)
{
    Q_CHECK_PTR(object);
}


#endif // JVMOBJECT_HXX
