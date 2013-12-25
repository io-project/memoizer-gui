#ifndef LOCALREFSGUARD_HXX
#define LOCALREFSGUARD_HXX

#include <list>

#include <QtCore/QtGlobal>

#include <jni.h>

#include "JvmException.hxx"

class LocalRefsGuard
{
    Q_DISABLE_COPY(LocalRefsGuard);
public:
    LocalRefsGuard(JNIEnv* jniEnv);
    ~LocalRefsGuard();

    void insert(jobject ref);
    void remove(jobject ref);
    void removeAll();

    /**
     * Sprawdza czy @p o jest zainicjalizowany. Rzuca wyjątkiem jeśli nie jest.
     */
    template<typename T>
    void check(const T& o);

    /**
     * @brief Sprawdza czy ref nie jest null-em i umieszcza w obiekcie.
     * W przeciwnym wypadku rzuca wyjątkiem JVM.
     * @param ref
     */
    void checkIn(jobject ref);

    /**
     * @brief Sprawdza czy ref nie jest null-em, tworzy
     * referencje globalną, sprawdza czy nie jest null-em, zwraca referencje
     * globalną. Jeżeli coś jest null-em - rzuca wyjątkiem JVM. Referencja
     * lokalna jest zwalniana.
     * @param ref
     * @return
     */
    template<typename T>
    T toGlobalChecked(const T& ref);

private:
    std::list<jobject> _refs;
    JNIEnv *_jniEnv;
};

template<typename T>
void LocalRefsGuard::check(const T &o)
{
    if(!o)
        throw JvmException::fromEnv(_jniEnv);
}

template<typename T>
T LocalRefsGuard::toGlobalChecked(const T& ref)
{
    check(ref);
    T r=reinterpret_cast<T>(_jniEnv->NewGlobalRef(ref));
    _jniEnv->DeleteLocalRef(ref);
    check(r);
    return r;
}


#endif // LOCALREFSGUARD_HXX
