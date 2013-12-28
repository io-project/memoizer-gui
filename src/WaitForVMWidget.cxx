#include "WaitForVMWidget.hxx"

WaitForVMWidget::WaitForVMWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

void WaitForVMWidget::setVmAboutToStop()
{
    label->setText(QString::fromUtf8("Maszyna wirtualna jest wyłączana..."));
}

void WaitForVMWidget::setWaitingForModel()
{
    label->setText(QString::fromUtf8("Tworzenie modelu..."));
}

void WaitForVMWidget::setWaitingForImage()
{
    label->setText(QString::fromUtf8("Pobieranie grafiki..."));
}

void WaitForVMWidget::setWaitingForData()
{
    label->setText(QString::fromUtf8("Wydobywanie danych z serwisu..."));
}

void WaitForVMWidget::setProgress(qint64 current, qint64 total)
{
    if(total>0)
    {
        progressBar->setMaximum(total);
        progressBar->setValue(current);
    }
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
