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
    void handleInitialized();
    void handleInitializedEmpty();

signals:

private slots:
    void initialize();

private:
    WaitForVMWidget* _waitForVmWidget;
    VmErrorWidget* _vmErrorWidget;

    enum class State
    {
        uninitialized,  // Stan początkowy
        initialization, // Trwa inicjalizacja (GUI odbiera dane z JVM)
        initializationFailed, // Inicjalizacja nie powiodła się (wyjątek JVM w trakcie inicjalizacji)
        vmConnected,    // Wyświetla zachęte do wyboru widoku lub wybrane widoki (w kartach)
        teardown,       // Trwa zamykanie aplikacji
        vmDisconnected, // JVM zostało odłączone (potrzebne to?)
    } _state;
};

#endif // CENTRALWIDGET_HXX
