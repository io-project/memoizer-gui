#include "WaitForVMWidget.hxx"

WaitForVMWidget::WaitForVMWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

void WaitForVMWidget::handleVmAboutToStop()
{
    label->setText(QString::fromUtf8("Maszyna wirtualna jest wyłączana..."));
}

void WaitForVMWidget::changeEvent(QEvent *e)
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
