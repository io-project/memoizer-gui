#ifndef JVMOBJECT_HXX
#define JVMOBJECT_HXX

#include <memory>

#include <QtCore/QtGlobal>

#include <jni.h>

#include "LocalRefsGuard.hxx"

template<class T,class U=jobject>
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
    static std::shared_ptr<T> wrap(JNIEnv *jniEnv, const U& object);

    template<typename W>
    static std::shared_ptr<T> wrap(JNIEnv *jniEnv, const U& object, const W& arg1);

    template<typename W,typename X>
    static std::shared_ptr<T> wrap(JNIEnv *jniEnv, const U& object, const W& arg1, const X& arg2);

    /**
     * @brief Tworzy nowy obiekt przechowujący globalną referencję i zwraca
     * wskaźnik do niego. Podana referencja jest sprawdzana i zwalniana.
     * @param jniEnv
     * @param object
     * @return
     */
    static std::shared_ptr<T> transferChecked(JNIEnv *jniEnv, const U& object);

    U unwrap() const;

protected:
    JvmObject(JNIEnv *jniEnv, const U& object);
    virtual ~JvmObject();
    void release(JNIEnv *jniEnv);

    void ensureInitialized(JNIEnv *jniEnv);

private:
    virtual void initialize(JNIEnv *jniEnv);
    virtual void deinitialize(JNIEnv *jniEnv);

private:
    U _object;  // Globalna referencja
    bool _isCacheInitialized; // Klasy pochodne mogą mieć cache
};

#include "JvmException.hxx"

template<class T,class U>
std::shared_ptr<T> JvmObject<T,U>::wrap(JNIEnv *jniEnv, const U &object)
{
    Q_CHECK_PTR(jniEnv);
    Q_CHECK_PTR(object);
    U objectG=reinterpret_cast<U>(jniEnv->NewGlobalRef(object));
    if(!objectG)
        throw JvmException::fromEnv(jniEnv);
    return std::shared_ptr<T>(new T(jniEnv,objectG),[jniEnv](T *o) {
        o->release(jniEnv);
    });
}

template<class T,class U>
template<typename W>
std::shared_ptr<T> JvmObject<T,U>::wrap(JNIEnv *jniEnv, const U& object, const W& arg1)
{
    Q_CHECK_PTR(jniEnv);
    Q_CHECK_PTR(object);
    U objectG=reinterpret_cast<U>(jniEnv->NewGlobalRef(object));
    if(!objectG)
        throw JvmException::fromEnv(jniEnv);
    return std::shared_ptr<T>(new T(jniEnv,objectG,arg1),[jniEnv](T *o) {
        o->release(jniEnv);
    });
}

template<class T,class U>
template<typename W,typename X>
std::shared_ptr<T> JvmObject<T,U>::wrap(JNIEnv *jniEnv, const U& object, const W& arg1, const X& arg2)
{
    Q_CHECK_PTR(jniEnv);
    Q_CHECK_PTR(object);
    U objectG=reinterpret_cast<U>(jniEnv->NewGlobalRef(object));
    if(!objectG)
        throw JvmException::fromEnv(jniEnv);
    return std::shared_ptr<T>(new T(jniEnv,objectG,arg1,arg2),[jniEnv](T *o) {
        o->release(jniEnv);
    });
}

template<class T,class U>
std::shared_ptr<T> JvmObject<T,U>::transferChecked(JNIEnv *jniEnv, const U &object)
{
    Q_CHECK_PTR(jniEnv);
    LocalRefsGuard localRefs(jniEnv);
    localRefs.checkIn(object);
    return wrap(jniEnv,object);
}

template<class T,class U>
U JvmObject<T,U>::unwrap() const
{
    return _object;
}

template<class T,class U>
JvmObject<T,U>::~JvmObject()
{
}

template<class T,class U>
void JvmObject<T,U>::release(JNIEnv *jniEnv)
{
    Q_CHECK_PTR(jniEnv);
    Q_CHECK_PTR(_object);
    jniEnv->DeleteGlobalRef(_object);
    if(_isCacheInitialized)
        deinitialize(jniEnv);
}

template<class T,class U>
void JvmObject<T,U>::ensureInitialized(JNIEnv *jniEnv)
{
    if(_isCacheInitialized)
        return;
    _isCacheInitialized=true;
    initialize(jniEnv);
}

template<class T,class U>
void JvmObject<T,U>::initialize(JNIEnv *jniEnv)
{
    Q_UNUSED(jniEnv);
    // Domyślna implementacja nic nie robi
}

template<class T,class U>
void JvmObject<T,U>::deinitialize(JNIEnv *jniEnv)
{
    Q_UNUSED(jniEnv);
    // Domyślna implementacja nic nie robi
}

template<class T,class U>
JvmObject<T,U>::JvmObject(JNIEnv *jniEnv, const U &object) :
     _object(object), _isCacheInitialized(false)
{
    Q_UNUSED(jniEnv);
    Q_CHECK_PTR(object);
}


#endif // JVMOBJECT_HXX
