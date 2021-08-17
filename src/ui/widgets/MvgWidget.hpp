#pragma once

#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/listbox.h>

namespace ui::widgets {
class MvgWidget : public Gtk::Box {
 private:
    Gtk::ListBox departureslistBox{};
    Gtk::Box departuresBox{Gtk::Orientation::ORIENTATION_VERTICAL};
    Gtk::Box loadingBox{Gtk::Orientation::ORIENTATION_VERTICAL};

    Gtk::CheckButton busCBtn{"Bus"};
    Gtk::CheckButton ubanCBtn{"U-Bahn"};
    Gtk::CheckButton sbahnCBtn{"S-Bahn"};
    Gtk::CheckButton tramCBtn{"Tram"};

 public:
    MvgWidget();
    MvgWidget(MvgWidget&&) = default;
    MvgWidget(const MvgWidget&) = delete;
    MvgWidget& operator=(MvgWidget&&) = default;
    MvgWidget& operator=(const MvgWidget&) = delete;
    ~MvgWidget() override = default;

 private:
    void prep_widget();
    void update();
};
}  // namespace ui::widgets