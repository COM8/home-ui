#pragma once

#include "backend/mvg/Departure.hpp"
#include <memory>
#include <gtkmm/box.h>
#include <gtkmm/listboxrow.h>

namespace ui::widgets {
class DepartureWidget : public Gtk::ListBoxRow {
 private:
    std::shared_ptr<backend::mvg::Departure> departure;

 public:
    explicit DepartureWidget(std::shared_ptr<backend::mvg::Departure> departure);
    DepartureWidget(DepartureWidget&&) = default;
    DepartureWidget(const DepartureWidget&) = delete;
    DepartureWidget& operator=(DepartureWidget&&) = default;
    DepartureWidget& operator=(const DepartureWidget&) = delete;
    ~DepartureWidget() override = default;

 private:
    void prep_widget();
};
}  // namespace ui::widgets