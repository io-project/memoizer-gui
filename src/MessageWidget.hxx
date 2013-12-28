#ifndef MESSAGEWIDGET_HXX
#define MESSAGEWIDGET_HXX

#include "ui_MessageWidget.h"

class MessageWidget : public QWidget, private Ui::MessageWidget
{
    Q_OBJECT

public:
    explicit MessageWidget(QWidget *parent = 0);

    void setWaitingForViewMessage();
    void setInitializedEmptyMessage();
    void setWaitingForSearchKey();

protected:
    void changeEvent(QEvent *e);
};

#endif // MESSAGEWIDGET_HXX
