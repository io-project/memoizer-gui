#include "MemeImageWidget.hxx"

#include <QtNetwork/QNetworkReply>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QImage>
#include <QtGui/QImageReader>

#include "MemeProcessor.hxx"
#include "WaitForVMWidget.hxx"

MemeImageWidget::MemeImageWidget(QWidget *parent) :
    QWidget(parent)
{
    _layout=new QVBoxLayout();
    setLayout(_layout);
}

void MemeImageWidget::setMemeProcessor(MemeProcessor *memeProcessor)
{
    Q_CHECK_PTR(memeProcessor);
    QNetworkReply *reply=memeProcessor->downloadReply();
    if(reply->isFinished())
    {
        QLabel *imageLabel=new QLabel();
        QImageReader reader(reply);
        imageLabel->setPixmap(QPixmap::fromImageReader(&reader));
        _layout->addWidget(imageLabel,1);
    }
    else
    {
        WaitForVMWidget* widget=new WaitForVMWidget();
        widget->setWaitingForImage();
        _layout->addWidget(widget,1);
        connect(reply,&QNetworkReply::downloadProgress,widget,&WaitForVMWidget::setProgress);
        connect(reply,&QNetworkReply::finished,this,[this,reply,widget](){
            _layout->removeWidget(widget);
            widget->deleteLater();
            QLabel *imageLabel=new QLabel();
            QImageReader reader(reply);
            imageLabel->setPixmap(QPixmap::fromImageReader(&reader));
            _layout->addWidget(imageLabel,1);
        });
    }
}
