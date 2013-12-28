#include "Core.hxx"

#include <QtCore/QList>

#include <QtWidgets/QApplication>

#include "MainWindow.hxx"
#include "JvmException.hxx"
#include "ViewType.hxx"
#include "MemoizerModel.hxx"
#include "Meme.hxx"

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
    qRegisterMetaType<std::weak_ptr<const ViewType>>();
    qRegisterMetaType<QList<std::weak_ptr<const ViewType>>>();
    qRegisterMetaType<std::size_t>("std::size_t");
    qRegisterMetaType<MemoizerModel*>();
    qRegisterMetaType<jint>("jint");
    qRegisterMetaType<Meme>();
}
