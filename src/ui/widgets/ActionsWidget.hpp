#pragma once

#include <gtkmm.h>

namespace ui::widgets {
class ActionsWidget : public Gtk::Box {
 public:
    ActionsWidget();
    ActionsWidget(ActionsWidget&&) = default;
    ActionsWidget(const ActionsWidget&) = delete;
    ActionsWidget& operator=(ActionsWidget&&) = default;
    ActionsWidget& operator=(const ActionsWidget&) = delete;
    ~ActionsWidget() override = default;

 private:
    void prep_widget();
};
}  // namespace ui::widgets