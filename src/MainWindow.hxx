#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <memory>
#include <unordered_set>

#include <QtCore/QList>
#include <QtCore/QStringList>

#include <QtWidgets/QMainWindow>

#include <jni.h>

class VirtualMachine;
class JvmException;
class ViewType;
class CentralWidget;

typedef jint (JNICALL * CreateJavaVM_t)(JavaVM **, void **, void *);

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow() override;

    CentralWidget* centralWidget() const;
    void closeEvent(QCloseEvent* event) override;

signals:
    void virtualMachineInitialized(VirtualMachine* vm);
    void initialized();
    void initializedEmpty();  // Zainicjalizowano, ale nie ma plugin-ów

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
    void receivePluginsNames(const QStringList& pluginsNames,const QStringList& selectedPlugins);
    void receiveViewTypes(const QList<std::weak_ptr<const ViewType>>& viewTypes);
    void receivePluginsNamesForView(const QStringList& pluginsNames, std::weak_ptr<const ViewType> viewType);

    /* Obsługa akcji */
    void selectAllSources();
    void deselectAllSources();
    void releaseAllPlugins();
    void showHelp();
    void showAbout();
    void showAccountsSettings();
    void showException(const JvmException &exception);

    /* Funkcje pomocnicze */
    QStringList selectedSources() const;

private:
    QThread* _vmThread;
    VirtualMachine* _vm;
    bool _wantClose;
    bool _vmIsAlive;

    QMenu* _viewsMenu;
    std::unordered_set<std::shared_ptr<const ViewType>> _waitingViewTypes;

    QMenu* _sourcesMenu;
    QList<QAction*> _sourcesActions;
    QStringList _allSources;

    QHash<QString,bool> _sourcesSelection;
};

#endif // MAINWINDOW_HXX
