#include "MessageWidget.hxx"

#include <QtCore/QDir>

MessageWidget::MessageWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

void MessageWidget::setWaitingForViewMessage()
{
    label->setText(QString::fromUtf8("Wybierz jakiś widok z menu Widok"));
}

void MessageWidget::setInitializedEmptyMessage()
{
    label->setText(QString::fromUtf8(
                       "Maszyna wirtualna została zainicjalizowana, ale nie "
                       "znaleziono żadnych pluginów.\nUpewnij się że "
                       "zainstalowałeś pluginy w katalogu ")
                   +QDir::toNativeSeparators(
                       QDir(qApp->applicationDirPath()+"/../plugins/")
                       .canonicalPath()));
}

void MessageWidget::changeEvent(QEvent *e)
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
