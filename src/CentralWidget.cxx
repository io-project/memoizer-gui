#include "CentralWidget.hxx"

#include <QtCore/QTimer>

#include "WaitForVMWidget.hxx"
#include "VmErrorWidget.hxx"

CentralWidget::CentralWidget(QWidget *parent) :
    QStackedWidget(parent), _state(State::uninitialized)
{
    initialize();
}

void CentralWidget::vmInitializationFailed()
{
    _state=State::initializationFailed;
    setCurrentWidget(_vmErrorWidget);
}

void CentralWidget::handleVmAboutToStop()
{
    _state=State::initialization;
    _waitForVmWidget->handleVmAboutToStop();
    setCurrentWidget(_waitForVmWidget);
}

void CentralWidget::initialize()
{
    _waitForVmWidget=new WaitForVMWidget(this);
    addWidget(_waitForVmWidget);
    _state=State::initialization;
    _vmErrorWidget=new VmErrorWidget(this);
    addWidget(_vmErrorWidget);
}
