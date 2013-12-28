#ifndef MEMOIZERMODEL_HXX
#define MEMOIZERMODEL_HXX

#include <QtCore/QObject>

#include <memory>

#include <jni.h>

class Meme;
class VirtualMachine;

class MemoizerModel : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MemoizerModel)
public:
    explicit MemoizerModel(VirtualMachine* vm, QObject *parent = 0);
    ~MemoizerModel();

    void pull(jint id);

signals:
    void notifyUpdate(const Meme& meme, jint id);
    void notifyStreamEnd();

private:
    VirtualMachine* _vm;

};

#endif // MEMOIZERMODEL_HXX
