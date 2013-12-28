#include "ViewWidget.hxx"

#include <QtWidgets/QLineEdit>

#include "ViewType.hxx"
#include "WaitForVMWidget.hxx"
#include "VirtualMachine.hxx"
#include "ModelViewWidget.hxx"
#include "MessageWidget.hxx"

ViewWidget::ViewWidget(VirtualMachine* vm,
                       std::shared_ptr<const ViewType> viewType,
                       const QStringList &pluginsNames,
                       QWidget *parent)
    : QWidget(parent), _modelRequestId(0), _focusWidget(nullptr)
{
    setupUi(this);

    if(Q_LIKELY(!viewType->isSearch()))
    {
        handleStandardView(vm,viewType,pluginsNames);
    }
    else
    {
        handleSearchView(vm,pluginsNames);
    }
}

void ViewWidget::handleFocus()
{
    if(_focusWidget)
        _focusWidget->setFocus();
}

void ViewWidget::releaseVm()
{
    ModelViewWidget *mvWidget=dynamic_cast<ModelViewWidget*>(_interactionWidget);
    if(mvWidget)
    {
        mvWidget->releaseVm();
    }
}

void ViewWidget::receiveModel(MemoizerModel *model,std::size_t requestId)
{
    if(Q_UNLIKELY(requestId!=_modelRequestId))
        return;
    verticalLayout->removeWidget(_interactionWidget);
    _interactionWidget->deleteLater();
    _interactionWidget=new ModelViewWidget(model,this);
    verticalLayout->addWidget(_interactionWidget,1);
}

void ViewWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void ViewWidget::handleStandardView(VirtualMachine* vm, std::shared_ptr<const ViewType> viewType, const QStringList &pluginsNames)
{
    // na razie tylko obszar przewijalny
    WaitForVMWidget *widget=new WaitForVMWidget();
    widget->setWaitingForModel();
    horizontalLayout->insertStretch(0,1);
    verticalLayout->addWidget(widget,1);
    _interactionWidget=widget;

    QMetaObject::invokeMethod(vm,
                              "prepareModel",
                              Q_ARG(QObject*,this),
                              Q_ARG(std::size_t,++_modelRequestId),
                              Q_ARG(std::weak_ptr<const ViewType>,viewType),
                              Q_ARG(const QStringList&, pluginsNames));
}

void ViewWidget::handleSearchView(VirtualMachine* vm, const QStringList &pluginsNames)
{
    MessageWidget *widget=new MessageWidget();
    widget->setWaitingForSearchKey();
    QLineEdit *searchLineEdit=new QLineEdit();
    searchLineEdit->setPlaceholderText(QString::fromUtf8("Szukaj"));
    _focusWidget=searchLineEdit;
    horizontalLayout->insertWidget(0,searchLineEdit,1);

    verticalLayout->addWidget(widget,1);
    _interactionWidget=widget;
    connect(searchLineEdit,&QLineEdit::returnPressed,this,[this,vm,pluginsNames,searchLineEdit](){
        const QString key=searchLineEdit->text();
        if(Q_UNLIKELY(key.isEmpty()))
            return;
        if(Q_LIKELY(!dynamic_cast<WaitForVMWidget*>(_interactionWidget)))
        {
            verticalLayout->removeWidget(_interactionWidget);
            releaseVm();
            _interactionWidget->deleteLater();
            WaitForVMWidget *widget=new WaitForVMWidget();
            widget->setWaitingForModel();
            verticalLayout->addWidget(widget,1);
            _interactionWidget=widget;
        }

        QMetaObject::invokeMethod(vm,
                                  "prepareSearchModel",
                                  Q_ARG(QObject*,this),
                                  Q_ARG(std::size_t,++_modelRequestId),
                                  Q_ARG(const QStringList&, pluginsNames),
                                  Q_ARG(const QString&,key));
    });
}
