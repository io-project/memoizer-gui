#ifndef VIEWWIDGET_HXX
#define VIEWWIDGET_HXX

#include <QtWidgets/QWidget>

#include <memory>

class ViewType;
class VirtualMachine;

class ViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewWidget(VirtualMachine* vm, std::weak_ptr<const ViewType> viewType, const QStringList &pluginsNames, QWidget *parent = 0);

    void releaseVm();

signals:

private:
    VirtualMachine* _vm;
};

#endif // VIEWWIDGET_HXX
