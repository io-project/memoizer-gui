#include "CentralWidget.hxx"

#include <QtCore/QTimer>

#include "WaitForVMWidget.hxx"
#include "VmErrorWidget.hxx"
#include "MainWindow.hxx"
#include "VirtualMachine.hxx"
#include "ViewType.hxx"
#include "MessageWidget.hxx"
#include "TabWidget.hxx"

CentralWidget::CentralWidget(MainWindow *parent) :
    QStackedWidget(parent), _vm(nullptr), _vmReadyToUse(false),
    _currentInteractionWidget(nullptr)
{
    _waitForVmWidget=new WaitForVMWidget(this);
    addWidget(_waitForVmWidget);
    _vmErrorWidget=new VmErrorWidget(this);
    addWidget(_vmErrorWidget);
    _messageWidget=new MessageWidget(this);
    addWidget(_messageWidget);
    _tabWidget=new TabWidget(this);
    addWidget(_tabWidget);
    connect(_tabWidget,&TabWidget::lastTabClosed,this,&CentralWidget::hideTabWidget);

    connect(parent,&MainWindow::virtualMachineInitialized,this,&CentralWidget::setVirtualMachine);
}

void CentralWidget::showView(std::weak_ptr<const ViewType> viewType, const QStringList &pluginsNames)
{
    if(!_vmReadyToUse)
        return;
    _tabWidget->showView(_vm,viewType,pluginsNames);
    if(!_currentInteractionWidget)
    {
        _currentInteractionWidget=_tabWidget;
        setCurrentWidget(_currentInteractionWidget);
    }
}

MainWindow *CentralWidget::parent() const
{
    return static_cast<MainWindow*>(QStackedWidget::parent());
}

void CentralWidget::setVirtualMachine(VirtualMachine *vm)
{
    _vm=vm;
    connect(vm,&VirtualMachine::readyToUse,this,[this](){_vmReadyToUse=true;});
    connect(vm,&VirtualMachine::initializationComplete,this,&CentralWidget::handleVmInitializationComplete);
    connect(vm,&VirtualMachine::initializationFailed,this,&CentralWidget::handleVmInitializationFailed);
    connect(vm,&VirtualMachine::aboutToStop,this,&CentralWidget::handleVmAboutToStop);
}

void CentralWidget::handleVmInitializationComplete(bool havePlugins)
{
    if(havePlugins)
    {
        _messageWidget->setWaitingForViewMessage();
    }
    else
    {
        _messageWidget->setInitializedEmptyMessage();
    }
    _currentInfoWidget=_messageWidget;
    if(!_currentInteractionWidget)
    {
        setCurrentWidget(_currentInfoWidget);
    }
}

void CentralWidget::handleVmInitializationFailed()
{
    _currentInfoWidget=_vmErrorWidget;
    setCurrentWidget(_currentInfoWidget);
    _vmReadyToUse=false;
}

void CentralWidget::handleVmAboutToStop()
{
    _vmReadyToUse=false;
    _waitForVmWidget->setVmAboutToStop();
    _currentInfoWidget=_waitForVmWidget;
    setCurrentWidget(_currentInfoWidget);
    _tabWidget->releaseVm();
    if(_currentInteractionWidget)
    {
        _currentInteractionWidget=nullptr;
    }
}

void CentralWidget::hideTabWidget()
{
    if(_currentInteractionWidget)
    {
        _currentInteractionWidget=nullptr;
        setCurrentWidget(_currentInfoWidget);
    }
}

