#include "TabWidget.hxx"

#include "VirtualMachine.hxx"
#include "ViewType.hxx"
#include "ViewWidget.hxx"

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    setTabsClosable(true);
    connect(this,&QTabWidget::tabCloseRequested,this,&TabWidget::handleTabCloseRequest);
}

void TabWidget::showView(VirtualMachine* vm, std::weak_ptr<const ViewType> viewType, const QStringList &pluginsNames)
{
    auto viewTypeL=viewType.lock();
    if(!viewTypeL)
        return;

    ViewWidget *viewWidget=new ViewWidget(vm,viewType,pluginsNames,this);

    int i=addTab(viewWidget,viewTypeL->getName()+": "+pluginsNames.join(", "));
    setCurrentIndex(i);
}

ViewWidget *TabWidget::widget(int index) const
{
    return static_cast<ViewWidget*>(QTabWidget::widget(index));
}

void TabWidget::releaseVm()
{
    int c=count();
    while(c--)
    {
        widget(c)->releaseVm();
        removeTab(c);
    }
}

void TabWidget::handleTabCloseRequest(int index)
{
    ViewWidget *viewWidget=widget(index);
    Q_CHECK_PTR(viewWidget);
    viewWidget->releaseVm();
    removeTab(index);
    if(count()==0)
        emit lastTabClosed();
}
