#ifndef VIEWWIDGET_HXX
#define VIEWWIDGET_HXX

#include <QtWidgets/QWidget>
#include "ui_ViewWidget.h"

#include <memory>

class ViewType;
class VirtualMachine;
class MemoizerModel;

class ViewWidget : public QWidget, private Ui::ViewWidget
{
    Q_OBJECT
public:
    explicit ViewWidget(VirtualMachine* vm, std::shared_ptr<const ViewType> viewType, const QStringList &pluginsNames, QWidget *parent = 0);

    void handleFocus();
    void releaseVm();

    Q_INVOKABLE void receiveModel(MemoizerModel* model, std::size_t requestId);

protected:
    void changeEvent(QEvent *e) override;

private:
    void handleStandardView(VirtualMachine* vm,std::shared_ptr<const ViewType> viewType, const QStringList &pluginsNames);
    void handleSearchView(VirtualMachine *vm, const QStringList &pluginsNames);

private:
    QWidget* _focusWidget;
    QWidget* _interactionWidget;
    std::size_t _modelRequestId;

    VirtualMachine* _vm;
};

#endif // VIEWWIDGET_HXX
