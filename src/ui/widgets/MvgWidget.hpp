#pragma once

#include <gtkmm/box.h>
#include <gtkmm/listbox.h>

namespace ui::widgets {
class MvgWidget : public Gtk::Box {
 private:
    Gtk::ListBox listBox{};

 public:
    MvgWidget();
    MvgWidget(MvgWidget&&) = default;
    MvgWidget(const MvgWidget&) = delete;
    MvgWidget& operator=(MvgWidget&&) = default;
    MvgWidget& operator=(const MvgWidget&) = delete;
    ~MvgWidget() override = default;

 private:
    void prep_widget();
    static void update();
};
}  // namespace ui::widgets