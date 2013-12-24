#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QtWidgets/QMainWindow>

#include <jni.h>

class VirtualMachine;
class JvmException;

typedef jint (JNICALL * CreateJavaVM_t)(JavaVM **, void **, void *);

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow() override;

    void closeEvent(QCloseEvent* event) override;

signals:

public slots:

private slots:
    /* Inicjalizacja GUI */
    void initializeMenuBar();
    void initializeCentralWidget();

    /* Inicjalizacja pomocników */
    void initializeVirtualMachine();

private:
    /* Stan */
    void saveWindowSettings();
    bool isReadyClose() const;
    void handleVmAboutToStop();
    void handleVmThreadFinished();

    /* Inicjalizacja GUI */
    void initializeSourcesMenu();
    void initializeMoreMenu(QMenu *moreMenu);

    /* Zdobywanie dodatkowych informacji od użytkownika */
    CreateJavaVM_t getCreateJavaVM();

    /* Komunikacja */
    // przenieść wyżej
    void receivePluginsNames(const QStringList& pluginsNames);

    /* Obsługa akcji */
    void selectAllSources();
    void deselectAllSources();
    void releaseAllPlugins();
    void showHelp();
    void showAbout();
    void showAccountsSettings();
    void showException(const JvmException &exception);

private:
    QThread* _vmThread;
    VirtualMachine* _vm;
    bool _wantClose;
    bool _vmIsAlive;

    QMenu* _sourcesMenu;
    QList<QAction*> _sourcesActions;

    QList<QString> _selectedSources;
};

#endif // MAINWINDOW_HXX
