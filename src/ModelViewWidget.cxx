#include "ModelViewWidget.hxx"

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QScrollBar>

#include "MemoizerModel.hxx"
#include "MemeProcessor.hxx"
#include "MemeWidget.hxx"
#include "WaitForVMWidget.hxx"
#include "Meme.hxx"

ModelViewWidget::ModelViewWidget(MemoizerModel *model, QWidget *parent) :
    QScrollArea(parent),_visibleMemeCount(0),
    _collectedMemeCount(0) , _model(model), _modelIsDead(false),
    _havePlaceholderInView(false)
{
    setWidgetResizable(true);

    _compositeWidget=new QWidget();
    _layout=new QVBoxLayout();
    _compositeWidget->setLayout(_layout);
    setWidget(_compositeWidget);

    _placeholderWidget=new WaitForVMWidget();
    _placeholderWidget->setWaitingForData();

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    connect(verticalScrollBar(),&QAbstractSlider::valueChanged,this,&ModelViewWidget::testAndHandleAtDown);
    testAndHandleAtDown();

    connect(model,&MemoizerModel::notifyUpdate,this,&ModelViewWidget::handleNotifyUpdate);
    connect(model,&MemoizerModel::notifyStreamEnd,this,&ModelViewWidget::handleStreamEnd);

    for(std::size_t i=0;i<_cacheSize;++i)
        if(Q_LIKELY(!_modelIsDead))
            _model->pull(i);
}

ModelViewWidget::~ModelViewWidget()
{
    _placeholderWidget->setParent(nullptr);
    _placeholderWidget->deleteLater();
}

void ModelViewWidget::releaseVm()
{
    _model->deleteLater();
    _model=nullptr;
}

void ModelViewWidget::handleNotifyUpdate(const Meme &meme, jint id)
{
    MemeProcessor* memeProcessor=new MemeProcessor(meme,this);
    _memes[id]=memeProcessor;
    jint i=_collectedMemeCount;
    for(;_memes.find(i)!=_memes.end();++i);
    _collectedMemeCount=i;

    tryToRemovePlaceholder();
}

void ModelViewWidget::handleStreamEnd()
{
    _modelIsDead=true;
    if(Q_LIKELY(!_havePlaceholderInView))
        return;
    _layout->removeWidget(_placeholderWidget);
}

#include <QDebug>

void ModelViewWidget::testAndHandleAtDown()
{
    if(Q_LIKELY(verticalScrollBar()->value()<verticalScrollBar()->maximum()))
        return;
    if(Q_UNLIKELY(_havePlaceholderInView))
        return;
    if(Q_LIKELY(_visibleMemeCount<_collectedMemeCount))
    {
        _layout->addWidget(prepareMemeWidget());
    }
    else
    {
        _havePlaceholderInView=true;
        _layout->addWidget(_placeholderWidget);
        _placeholderWidget->show();
    }
    //_compositeWidget->resize(_compositeWidget->sizeHint());
}

void ModelViewWidget::tryToRemovePlaceholder()
{
    if(Q_LIKELY(!_havePlaceholderInView))
        return;
    if(Q_UNLIKELY(_visibleMemeCount==_collectedMemeCount))
        return;
    _layout->removeWidget(_placeholderWidget);
    _placeholderWidget->setParent(nullptr);
    _havePlaceholderInView=false;
    _placeholderWidget->hide();
    _layout->addWidget(prepareMemeWidget());
    //_compositeWidget->resize(_compositeWidget->sizeHint());

    //testAndHandleAtDown(); // Być może trzeba asynchronicznie, żeby widget zdążył zająć miejsce. (albo w ogóle, bo modyfikacja widgetu spowoduje pojawienie się sygnału...)
}

MemeWidget *ModelViewWidget::prepareMemeWidget()
{
    Q_ASSERT(_visibleMemeCount<_collectedMemeCount);
    MemeWidget* result=new MemeWidget(_memes[_visibleMemeCount++]);
    if(_visibleMemeCount+_cacheSize>_collectedMemeCount)
        if(Q_LIKELY(!_modelIsDead))
            _model->pull(_collectedMemeCount);
    return result;
}
