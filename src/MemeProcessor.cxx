#include "MemeProcessor.hxx"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

MemeProcessor::MemeProcessor(const Meme &meme, QObject *parent) :
    QObject(parent), _meme(meme), _imageDownloadReply(nullptr)
{
    QNetworkAccessManager *nam=new QNetworkAccessManager(this);
    _imageDownloadReply= nam->get(QNetworkRequest(meme.imageLink()));
}

QNetworkReply *MemeProcessor::downloadReply() const
{
    return _imageDownloadReply;
}
