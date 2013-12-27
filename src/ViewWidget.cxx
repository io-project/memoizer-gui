#include "ViewWidget.hxx"

ViewWidget::ViewWidget(VirtualMachine* vm,
                       std::weak_ptr<const ViewType> viewType,
                       const QStringList &pluginsNames,
                       QWidget *parent)
    : QWidget(parent), _vm(vm)
{

}

void ViewWidget::releaseVm()
{

}
