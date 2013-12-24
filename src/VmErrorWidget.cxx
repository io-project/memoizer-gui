#include "VmErrorWidget.hxx"

VmErrorWidget::VmErrorWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

void VmErrorWidget::changeEvent(QEvent *e)
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
