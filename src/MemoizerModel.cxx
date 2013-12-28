#include "MemoizerModel.hxx"

#include "VirtualMachine.hxx"
#include "Meme.hxx"

MemoizerModel::MemoizerModel(VirtualMachine *vm, QObject *parent) :
    QObject(parent), _vm(vm)
{
}

MemoizerModel::~MemoizerModel()
{
    QMetaObject::invokeMethod(
                _vm,
                "releaseModel",
                Qt::QueuedConnection,
                Q_ARG(MemoizerModel*,this));
}

void MemoizerModel::pull(jint id)
{
    QMetaObject::invokeMethod(
                _vm,
                "pull",
                Qt::QueuedConnection,
                Q_ARG(MemoizerModel*,this),
                Q_ARG(jint,id));
}
