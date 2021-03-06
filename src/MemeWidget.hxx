#ifndef MEMEWIDGET_HXX
#define MEMEWIDGET_HXX

#include "ui_MemeWidget.h"

class MemeProcessor;
class VirtualMachine;

class MemeWidget : public QWidget, private Ui::MemeWidget
{
    Q_OBJECT

public:
    explicit MemeWidget(MemeProcessor* memeProcessor,VirtualMachine* vm, QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
};

#endif // MEMEWIDGET_HXX
