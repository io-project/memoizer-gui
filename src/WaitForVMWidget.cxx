#include "WaitForVMWidget.hxx"

WaitForVMWidget::WaitForVMWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
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
