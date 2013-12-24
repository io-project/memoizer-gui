#include "Core.hxx"

#include <QtWidgets/QApplication>

#include "MainWindow.hxx"
#include "JvmException.hxx"

Core::Core()
{
}

int Core::run(int argc, char *argv[])
{
    QApplication app(argc,argv);
    app.setOrganizationName(QString::fromUtf8("io-project"));
    app.setApplicationName(QString::fromUtf8("memoizer"));
    registerMetaTypes();
    MainWindow window;
    window.show();
    return app.exec();
}

void Core::registerMetaTypes()
{
    qRegisterMetaType<JvmException>();
}
