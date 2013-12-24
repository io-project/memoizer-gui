#ifndef CENTRALWIDGET_HXX
#define CENTRALWIDGET_HXX

#include <QtWidgets/QStackedWidget>

class WaitForVMWidget;
class VmErrorWidget;

class CentralWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit CentralWidget(QWidget *parent = 0);

    void vmInitializationFailed();
    void handleVmAboutToStop();
    // Być może już gotowy, ponieważ otrzymano wcześniej żądanie wyświetlenia karty z treścią
    void changeStateToReady();

signals:

private slots:
    void initialize();

private:
    WaitForVMWidget* _waitForVmWidget;
    VmErrorWidget* _vmErrorWidget;

    enum class State
    {
        uninitialized,
        initialization,
        initializationFailed,
        vmConnected,
        vmDisconnected,
    } _state;
};

#endif // CENTRALWIDGET_HXX
