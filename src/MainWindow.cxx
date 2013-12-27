#include "MainWindow.hxx"

#include <functional>

#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtCore/QSemaphore>
#include <QtCore/QLibrary>
#include <QtCore/QDir>

#include <QtGui/QCloseEvent>

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStyle>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>

#include "CentralWidget.hxx"
#include "VirtualMachine.hxx"
#include "JvmException.hxx"
#include "ViewType.hxx"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), _sourcesMenu(nullptr), _wantClose(false),
    _vmIsAlive(false), _viewsMenu(nullptr)
{
    QSettings settings;
    resize(settings.value("MainWindow/size",QSize(600,400)).value<QSize>());
    move(settings.value("MainWindow/position",pos()).value<QPoint>());

    QTimer::singleShot(0,this,SLOT(initializeMenuBar()));
    QTimer::singleShot(0,this,SLOT(initializeCentralWidget()));
    QTimer::singleShot(0,this,SLOT(initializeVirtualMachine()));
}

void MainWindow::saveWindowSettings()
{
    QSettings settings;
    settings.setValue("MainWindow/size",size());
    settings.setValue("MainWindow/position",pos());
}

MainWindow::~MainWindow()
{
}

CentralWidget *MainWindow::centralWidget() const
{
    return static_cast<CentralWidget*>(QMainWindow::centralWidget());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    _wantClose=true;
    if(isReadyClose())
    {
        saveWindowSettings();
        event->accept();
    }
    else
    {
        _vmIsAlive=false;
        {
            QStringList plugins[2];
            for(const auto plugin : _allSources)
                plugins[_sourcesSelection[plugin]]<<plugin;
            QMetaObject::invokeMethod(
                        _vm,"storePluginsSelection",
                        Q_ARG(QStringList,plugins[1]),
                        Q_ARG(QStringList,plugins[0]));
        }
        QMetaObject::invokeMethod(_vm,"stop");
        event->ignore();
    }
}

void MainWindow::initializeMenuBar()
{
    _viewsMenu=menuBar()->addMenu(QString::fromUtf8("&Widok"));
    _sourcesMenu=menuBar()->addMenu(QString::fromUtf8("Źró&dła"));
    initializeSourcesMenu();
    initializeMoreMenu(menuBar()->addMenu(QString::fromUtf8("Więc&ej")));
}

void MainWindow::initializeCentralWidget()
{
    setCentralWidget(new CentralWidget(this));
}

void MainWindow::initializeVirtualMachine()
{
    _vmThread=new QThread(this);
    _vmThread->setObjectName("Virtual Machine Controller");
    _vm=new VirtualMachine();
    _vm->setVmProvider(getCreateJavaVM());
    _vm->moveToThread(_vmThread);
    connect(_vmThread,&QThread::started,_vm,&VirtualMachine::start);
    connect(_vm,&VirtualMachine::pluginsNames,this,&MainWindow::receivePluginsNames);
    connect(_vm,&VirtualMachine::viewsTypes,this,&MainWindow::receiveViewTypes);
    connect(_vm,&VirtualMachine::pluginsNamesForView,this,&MainWindow::receivePluginsNamesForView);
    connect(_vm,&VirtualMachine::aboutToStop,this,&MainWindow::handleVmAboutToStop);
    connect(_vm,&VirtualMachine::stopped,_vmThread,&QThread::quit);
    connect(_vm,&VirtualMachine::exceptionOccured,this,&MainWindow::showException);
    connect(_vmThread,&QThread::finished,this,&MainWindow::handleVmThreadFinished);
    _vmThread->start();

    emit virtualMachineInitialized(_vm);
}

bool MainWindow::isReadyClose() const
{
    return _vmThread->isFinished();
}

void MainWindow::handleVmAboutToStop()
{
    releaseAllPlugins();
}

void MainWindow::handleVmThreadFinished()
{
    if(_wantClose)
        close();
}

void MainWindow::initializeSourcesMenu()
{
    _sourcesMenu->addSeparator();

    QAction* selectAllAction=_sourcesMenu->addAction(
                QString::fromUtf8("Z&aznacz wszystkie"));
    connect(selectAllAction,&QAction::triggered,this,&MainWindow::selectAllSources);

    QAction* unselectAllAction=_sourcesMenu->addAction(
                QString::fromUtf8("O&dznacz wszystkie"));
    connect(unselectAllAction,&QAction::triggered,this,&MainWindow::deselectAllSources);
}

void MainWindow::initializeMoreMenu(QMenu *moreMenu)
{
    QAction* settingsAction=moreMenu->addAction(
                QString::fromUtf8("U&stawienia"));
    settingsAction->setMenuRole(QAction::PreferencesRole);
    settingsAction->setEnabled(false);

    QAction* pluginsAction=moreMenu->addAction(
                QString::fromUtf8("Plugi&ny"));
    pluginsAction->setEnabled(false);

    QAction* helpAction=moreMenu->addAction(
                QIcon::fromTheme(QString::fromUtf8("help-contents")),
                QString::fromUtf8("&Pomoc"));
    connect(helpAction,&QAction::triggered,this,&MainWindow::showHelp);

    QAction* aboutAction=moreMenu->addAction(
                QIcon::fromTheme(QString::fromUtf8("help-about")),
                QString::fromUtf8("O progr&amie"));
    aboutAction->setMenuRole(QAction::AboutRole);
    connect(aboutAction,&QAction::triggered,this,&MainWindow::showAbout);

    QAction* accountAction=moreMenu->addAction(
                QString::fromUtf8("&Konto"));
    connect(accountAction,&QAction::triggered,this,&MainWindow::showAccountsSettings);
}

#ifdef Q_OS_WIN
static QString getJvmLocationHint()
{
    QString base="HKEY_LOCAL_MACHINE\\SOFTWARE\\JavaSoft\\Java Runtime Environment";
    QSettings jre(base,QSettings::NativeFormat);
    QSettings jvm(base+"\\"+jre.value("CurrentVersion").toString(),
                  QSettings::NativeFormat);
    return jvm.value("JavaHome").toString();
}
#endif

CreateJavaVM_t MainWindow::getCreateJavaVM()
{
#ifdef Q_OS_WIN
    QSettings settings;
#ifdef Q_OS_WIN64
    const QString platform=QString::fromUtf8("x86_64");
#elif Q_OS_WIN32
    const QString platform=QString::fromUtf8("x86");
#endif
    QString jvmLibraryLocation=settings.value("jvmLibraryLocation"+platform).toString();
    if(jvmLibraryLocation.isEmpty())
        jvmLibraryLocation=getJvmLocationHint();
    QFunctionPointer result;
    do
    {
        const char* symbol="JNI_CreateJavaVM";
        result=QLibrary::resolve(jvmLibraryLocation,symbol);
        if(result)
            break;
        QString additionalPropositions[]={
            QString::fromUtf8("bin/client/jvm.dll"),
            QString::fromUtf8("bin/server/jvm.dll"),
        };
        for(const auto proposition : additionalPropositions)
        {
            result=QLibrary::resolve(jvmLibraryLocation+"/"+proposition,symbol);
            if(result)
            {
                jvmLibraryLocation+="/"+proposition;
                break;
            }
        }
        if(result)
            break;
        bool ok;
        jvmLibraryLocation=QInputDialog::getText(
                    this,
                    QString::fromUtf8("Podaj lokalizacje maszyny wirtualnej Javy"),
                    QString::fromUtf8("Podaj lokalizacje maszyny wirtualnej z"
                                      " której chcesz korzystać. Musi to być"
                                      " wersja na platformę ")+platform,
                    QLineEdit::Normal,
                    jvmLibraryLocation,
                    &ok);
        if(!ok)
            break;
    } while(true);
    if(result)
    {
        settings.setValue("jvmLibraryLocation"+platform,QDir::toNativeSeparators(jvmLibraryLocation));
    }
    return reinterpret_cast<CreateJavaVM_t>(result);
#else
    return &JNI_CreateJavaVM;
#endif
}

void MainWindow::receivePluginsNames(const QStringList &pluginsNames, const QStringList &selectedPlugins)
{
    const QSet<QString> selectedPluginsS=QSet<QString>::fromList(selectedPlugins);
    QAction* separator=_sourcesMenu->actions().first();
    for(const auto plugin : pluginsNames)
    {
       QAction* sourceAction=new QAction(plugin,_sourcesMenu);
       sourceAction->setCheckable(true);
       connect(sourceAction,&QAction::triggered,[this,plugin](bool checked){
           if(checked)
               _sourcesSelection[plugin]=true;
           else
               _sourcesSelection[plugin]=false;
       });
       if(selectedPluginsS.contains(plugin))
           sourceAction->trigger();
       _sourcesActions.append(sourceAction);
    }
    _sourcesMenu->insertActions(separator,_sourcesActions);
    _allSources=pluginsNames;
}
void MainWindow::receiveViewTypes(const QList<std::weak_ptr<const ViewType> > &viewTypes)
{
    if(viewTypes.isEmpty())
        emit initializedEmpty();
    for(const auto v : viewTypes)
        _waitingViewTypes.insert(v.lock());
}

void MainWindow::receivePluginsNamesForView(const QStringList &pluginsNames, std::weak_ptr<const ViewType> viewType)
{
    auto viewTypeL=viewType.lock();
    Q_ASSERT(viewTypeL);
    QMenu *viewMenu=_viewsMenu->addMenu(viewTypeL->getName());
    QAction *action=viewMenu->addAction(QString::fromUtf8("Zaznaczone źródła"));
    connect(action,&QAction::triggered,this,[viewType,this](){centralWidget()->showView(viewType,selectedSources());});
    viewMenu->addSeparator();
    for(auto plugin:pluginsNames)
    {
        action=viewMenu->addAction(plugin);
        connect(action,&QAction::triggered,this,[this,plugin,viewType](){centralWidget()->showView(viewType,QStringList(plugin));});
    }
    viewMenu->addSeparator();
    action=viewMenu->addAction(QString::fromUtf8("Wszystko"));
    connect(action,&QAction::triggered,this,[this,viewType,pluginsNames](){centralWidget()->showView(viewType,pluginsNames);});

    _waitingViewTypes.erase(viewTypeL);
    if(_waitingViewTypes.empty())
        emit initialized();
}

void MainWindow::selectAllSources()
{
    for(QAction* action: _sourcesActions)
    {
        action->setChecked(true);
    }
    for(const auto source : _allSources)
        _sourcesSelection[source]=true;
}

void MainWindow::deselectAllSources()
{
    for(QAction* action: _sourcesActions)
    {
        action->setChecked(false);
    }
    for(const auto source : _allSources)
        _sourcesSelection[source]=false;
}

void MainWindow::releaseAllPlugins()
{
    deselectAllSources();
    menuBar()->setEnabled(false);
    for(QAction *action : _sourcesActions)
        _sourcesMenu->removeAction(action);
}

void MainWindow::showHelp()
{

}

void MainWindow::showAbout()
{

}

void MainWindow::showAccountsSettings()
{

}

void MainWindow::showException(const JvmException& exception)
{
    QMessageBox::critical(this,
                          QString::fromUtf8("Wystąpił wyjątek"),
                          exception.whatUnicode());
}

QStringList MainWindow::selectedSources() const
{
    QStringList result;
    for(auto source : _allSources)
        if(_sourcesSelection[source])
            result<<source;
    return result;
}
