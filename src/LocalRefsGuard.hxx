#ifndef LOCALREFSGUARD_HXX
#define LOCALREFSGUARD_HXX

#include <list>

#include <QtCore/QtGlobal>

#include <jni.h>

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
     * @brief Sprawdza czy ref nie jest null-em i umieszcza w obiekcie.
     * W przeciwnym wypadku rzuca wyjątkiem JVM.
     * @param ref
     */
    void checkIn(jobject ref);

    /**
     * @brief Sprawdza czy ref nie jest null-em, tworzy
     * referencje globalną, sprawdza czy nie jest null-em, zwraca referencje
     * globalną. Jeżeli coś jest null-em - rzuca wyjątkiem JVM.
     * @param ref
     * @return
     */
    jobject toGlobalChecked(jobject ref);

private:
    std::list<jobject> _refs;
    JNIEnv *_jniEnv;
};

#endif // LOCALREFSGUARD_HXX
