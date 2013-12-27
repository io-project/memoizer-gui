#include "VmErrorWidget.hxx"

VmErrorWidget::VmErrorWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    icon->setPixmap(QIcon::fromTheme(
                        "dialog-error",
                        style()->standardIcon(QStyle::SP_MessageBoxCritical))
                    .pixmap(70));
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
