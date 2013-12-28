#ifndef MEMEIMAGEWIDGET_HXX
#define MEMEIMAGEWIDGET_HXX

#include <QtWidgets/QWidget>

#include <QtWidgets/QVBoxLayout>

class MemeProcessor;

class MemeImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MemeImageWidget(QWidget *parent = 0);

    void setMemeProcessor(MemeProcessor *memeProcessor);

private:
    QVBoxLayout* _layout;
};

#endif // MEMEIMAGEWIDGET_HXX
