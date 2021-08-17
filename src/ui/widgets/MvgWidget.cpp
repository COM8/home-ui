#include "MvgWidget.hpp"
#include "DepartureWidget.hpp"
#include "backend/mvg/Departure.hpp"
#include "backend/mvg/MvgHelper.hpp"
#include <memory>
#include <vector>
#include <gtkmm/checkbutton.h>
#include <gtkmm/enums.h>
#include <gtkmm/flowbox.h>
#include <gtkmm/grid.h>
#include <gtkmm/scrolledwindow.h>

namespace ui::widgets {
MvgWidget::MvgWidget() {
    prep_widget();
    update();
}

void MvgWidget::prep_widget() {
    ubanCBtn.set_active(true);
    sbahnCBtn.set_active(true);
    busCBtn.set_active(true);
    tramCBtn.set_active(true);

    Gtk::FlowBox* productGrid = Gtk::make_managed<Gtk::FlowBox>();
    productGrid->set_selection_mode(Gtk::SelectionMode::SELECTION_NONE);
    productGrid->set_homogeneous(false);
    productGrid->set_hexpand(true);
    productGrid->add(ubanCBtn);
    productGrid->add(sbahnCBtn);
    productGrid->add(busCBtn);
    productGrid->add(tramCBtn);

    departuresBox.add(*productGrid);
    Gtk::ScrolledWindow* scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
    departureslistBox.set_selection_mode(Gtk::SelectionMode::SELECTION_NONE);
    scroll->add(departureslistBox);
    scroll->set_vexpand(true);
    departuresBox.add(*scroll);
    add(departuresBox);
}

void MvgWidget::update() {
    std::vector<std::shared_ptr<backend::mvg::Departure>> departures = backend::mvg::request_departures("de:09162:530", busCBtn.get_active(), ubanCBtn.get_active(), sbahnCBtn.get_active(), tramCBtn.get_active());

    // Clear existing items:
    std::vector<Gtk::Widget*> neg_children = departureslistBox.get_children();
    for (Gtk::Widget* child : neg_children) {
        departureslistBox.remove(*child);
    }

    // Add new items:
    for (const std::shared_ptr<backend::mvg::Departure>& departure : departures) {
        DepartureWidget* depW = Gtk::make_managed<DepartureWidget>(departure);
        departureslistBox.add(*depW);
    }
}
}  // namespace ui::widgets