#pragma once

#include <gtkmm.h>
#include <gtkmm/flowbox.h>

namespace ui::widgets {
class ActionsWidget : public Gtk::FlowBox {
 public:
    ActionsWidget();
    ActionsWidget(ActionsWidget&&) = delete;
    ActionsWidget(const ActionsWidget&) = delete;
    ActionsWidget& operator=(ActionsWidget&&) = delete;
    ActionsWidget& operator=(const ActionsWidget&) = delete;
    ~ActionsWidget() override = default;

 private:
    void prep_widget();
};
}  // namespace ui::widgets