#include "MemeWidget.hxx"

#include "MemeProcessor.hxx"
#include "Meme.hxx"

MemeWidget::MemeWidget(MemeProcessor *memeProcessor, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    titleLabel->setText(memeProcessor->meme().title());
    descriptionLabel->setText(memeProcessor->meme().description());
    imageWidget->setMemeProcessor(memeProcessor);
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
