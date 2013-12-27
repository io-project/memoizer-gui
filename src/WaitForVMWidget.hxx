#ifndef WAITFORVMWIDGET_HXX
#define WAITFORVMWIDGET_HXX

#include "ui_WaitForVMWidget.h"

class WaitForVMWidget : public QWidget, private Ui::WaitForVMWidget
{
    Q_OBJECT

public:
    explicit WaitForVMWidget(QWidget *parent = 0);

    void setVmAboutToStop();

protected:
    void changeEvent(QEvent *e) override;
};

#endif // WAITFORVMWIDGET_HXX
