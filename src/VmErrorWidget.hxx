#ifndef VMERRORWIDGET_HXX
#define VMERRORWIDGET_HXX

#include "ui_VmErrorWidget.h"

class VmErrorWidget : public QWidget, private Ui::VmErrorWidget
{
    Q_OBJECT

public:
    explicit VmErrorWidget(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
};

#endif // VMERRORWIDGET_HXX
