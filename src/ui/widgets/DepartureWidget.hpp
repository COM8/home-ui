#pragma once

#include "backend/mvg/Departure.hpp"
#include <memory>
#include <gtkmm/box.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/label.h>
#include <gtkmm/listboxrow.h>

namespace ui::widgets {
class DepartureWidget : public Gtk::ListBoxRow {
 private:
    std::shared_ptr<backend::mvg::Departure> departure;

    Gtk::Box mainBox{Gtk::Orientation::VERTICAL};
    Gtk::Box headerBox{Gtk::Orientation::HORIZONTAL};
    Gtk::Label label{};
    Glib::RefPtr<Gtk::CssProvider> labelProvider = Gtk::CssProvider::create();
    Gtk::Label canceled{"❌"};
    Gtk::Label destination{};

    Gtk::Box infoBox{Gtk::Orientation::HORIZONTAL};
    Gtk::Label depInfo{};

 public:
    DepartureWidget();
    DepartureWidget(DepartureWidget&&) = default;
    DepartureWidget(const DepartureWidget&) = delete;
    DepartureWidget& operator=(DepartureWidget&&) = default;
    DepartureWidget& operator=(const DepartureWidget&) = delete;
    ~DepartureWidget() override = default;

 private:
    void prep_widget();
    void update_departure_ui();

 public:
    void set_departure(std::shared_ptr<backend::mvg::Departure> departure);
};
}  // namespace ui::widgets