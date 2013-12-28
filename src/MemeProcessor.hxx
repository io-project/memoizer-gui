#ifndef MEMEPROCESSOR_HXX
#define MEMEPROCESSOR_HXX

#include <QtCore/QObject>

#include <memory>

#include "Meme.hxx"

class QNetworkReply;

class MemeProcessor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Meme meme READ meme USER true CONSTANT);
public:
    explicit MemeProcessor(const Meme& meme, QObject *parent = 0);

    QNetworkReply* downloadReply() const;
    const Meme& meme() const {
        return _meme;
    }

private:
    Meme _meme;
    QNetworkReply* _imageDownloadReply;
};

#endif // MEMEPROCESSOR_HXX
