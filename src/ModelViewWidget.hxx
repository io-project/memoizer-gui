#ifndef MODELVIEWWIDGET_HXX
#define MODELVIEWWIDGET_HXX

#include <QtWidgets/QScrollArea>

#include <memory>

#include <QtCore/QMap>
#include <QtWidgets/QVBoxLayout>

#include <jni.h>

class MemoizerModel;
class Meme;
class MemeProcessor;
class MemeWidget;
class WaitForVMWidget;

class ModelViewWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit ModelViewWidget(MemoizerModel *model, QWidget *parent = 0);
    virtual ~ModelViewWidget() override;

    void releaseVm();

private:
    void handleNotifyUpdate(const Meme& meme, jint id);
    void handleStreamEnd();

    void testAndHandleAtDown(); // Po przygotowaniu do wyświetlenia sprawdzić ponownie
    void tryToRemovePlaceholder();

    MemeWidget* prepareMemeWidget();

private:
    QWidget* _compositeWidget;
    QVBoxLayout* _layout;
    WaitForVMWidget* _placeholderWidget;

    static const std::size_t _cacheSize=10;
    std::size_t _visibleMemeCount;  // Wyświetlone
    std::size_t _collectedMemeCount;// Otrzymane z modelu (cache,...)
    bool _havePlaceholderInView;


    QMap<jint,MemeProcessor*> _memes;   // Memy otrzymane z modelu
    MemoizerModel* _model;
    bool _modelIsDead;
};

#endif // MODELVIEWWIDGET_HXX
