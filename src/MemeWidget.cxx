#include "MemeWidget.hxx"

#include "MemeProcessor.hxx"
#include "Meme.hxx"
#include "VirtualMachine.hxx"

MemeWidget::MemeWidget(MemeProcessor *memeProcessor, VirtualMachine *vm, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    titleLabel->setText(memeProcessor->meme().title());
    descriptionLabel->setText(memeProcessor->meme().description());
    imageWidget->setMemeProcessor(memeProcessor);

    QUrl imageUrl=memeProcessor->meme().imageLink();
    connect(sharePushButton,&QPushButton::clicked,this,[imageUrl,vm]{
        QMetaObject::invokeMethod(
                    vm,
                    "shareOnFacebook",
                    Qt::QueuedConnection,
                    Q_ARG(QUrl,imageUrl));
    });
    connect(shareOnTwitterPushButton,&QPushButton::clicked,this,[imageUrl,vm]{
        QMetaObject::invokeMethod(
                    vm,
                    "shareOnTwitter",
                    Qt::QueuedConnection,
                    Q_ARG(QUrl,imageUrl));
    });
}

void MemeWidget::changeEvent(QEvent *e)
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
