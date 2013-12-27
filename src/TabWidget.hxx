#ifndef TABWIDGET_HXX
#define TABWIDGET_HXX

#include <QtWidgets/QTabWidget>

#include <memory>

class ViewType;
class ViewWidget;
class VirtualMachine;

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = 0);

    void showView(VirtualMachine* vm, std::weak_ptr<const ViewType> viewType, const QStringList &pluginsNames);

    ViewWidget* widget(int index) const;

    void releaseVm();

signals:
    void lastTabClosed();

private:
    void handleTabCloseRequest(int index);

};

#endif // TABWIDGET_HXX
