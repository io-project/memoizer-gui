#ifndef CENTRALWIDGET_HXX
#define CENTRALWIDGET_HXX

#include <memory>

#include <QtWidgets/QStackedWidget>

class WaitForVMWidget;
class VmErrorWidget;
class VirtualMachine;
class MainWindow;
class ViewType;
class TabWidget;
class MessageWidget;

class CentralWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit CentralWidget(MainWindow *parent = 0);

    void showView(std::weak_ptr<const ViewType> viewType, const QStringList &pluginsNames);

    MainWindow* parent() const;

    void setVirtualMachine(VirtualMachine* vm);

signals:

private:
    // handle vm state
    void handleVmInitializationComplete(bool havePlugins);
    void handleVmInitializationFailed();
    void handleVmAboutToStop();
    // handle user requests
    void hideTabWidget();

private:
    QWidget *_currentInfoWidget;
    QWidget *_currentInteractionWidget;

    WaitForVMWidget* _waitForVmWidget;
    VmErrorWidget* _vmErrorWidget;
    MessageWidget* _messageWidget;
    TabWidget* _tabWidget;

    VirtualMachine *_vm;
    bool _vmReadyToUse;

};

#endif // CENTRALWIDGET_HXX
