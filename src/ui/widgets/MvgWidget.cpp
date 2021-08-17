#include "MvgWidget.hpp"
#include "DepartureWidget.hpp"
#include "backend/mvg/Departure.hpp"
#include "backend/mvg/MvgHelper.hpp"
#include "logger/Logger.hpp"
#include <cassert>
#include <memory>
#include <thread>
#include <vector>
#include <gtkmm/checkbutton.h>
#include <gtkmm/enums.h>
#include <gtkmm/flowbox.h>
#include <gtkmm/grid.h>
#include <gtkmm/scrolledwindow.h>
#include <spdlog/spdlog.h>

namespace ui::widgets {
MvgWidget::MvgWidget() {
    prep_widget();
    disp.connect(sigc::mem_fun(*this, &MvgWidget::on_notification_from_update_thread));
    start_thread();
}

MvgWidget::~MvgWidget() {
    stop_thread();
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

void MvgWidget::update_departures_ui() {
    // Clear existing items:
    std::vector<Gtk::Widget*> neg_children = departureslistBox.get_children();
    for (Gtk::Widget* child : neg_children) {
        departureslistBox.remove(*child);
    }

    // Add new items:
    departuresMutex.lock();
    for (const std::shared_ptr<backend::mvg::Departure>& departure : departures) {
        DepartureWidget* depW = Gtk::make_managed<DepartureWidget>(departure);
        departureslistBox.add(*depW);
    }
    departuresMutex.unlock();
    departureslistBox.show_all();
}

void MvgWidget::update_departures() {
    SPDLOG_INFO("Updating departures...");
    std::vector<std::shared_ptr<backend::mvg::Departure>> departures;
    try {
        departures = backend::mvg::request_departures("de:09162:530", busCBtn.get_active(), ubanCBtn.get_active(), sbahnCBtn.get_active(), tramCBtn.get_active());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to update departures with: {}", e.what());
    }
    departuresMutex.lock();
    this->departures = departures;
    departuresMutex.unlock();
    disp.emit();
    SPDLOG_INFO("Departures updated.");
}

void MvgWidget::thread_run() {
    SPDLOG_INFO("Departure thread started.");
    while (shouldRun) {
        update_departures();
        std::this_thread::sleep_for(std::chrono::seconds(20));
    }
    SPDLOG_INFO("Departure thread stoped.");
}

void MvgWidget::start_thread() {
    assert(!updateThread);
    assert(!shouldRun);
    shouldRun = true;
    updateThread = std::make_unique<std::thread>(&MvgWidget::thread_run, this);
}

void MvgWidget::stop_thread() {
    assert(updateThread);
    assert(shouldRun);
    shouldRun = false;
    updateThread->join();
    updateThread = nullptr;
}

//-----------------------------Events:-----------------------------
void MvgWidget::on_notification_from_update_thread() { update_departures_ui(); }

}  // namespace ui::widgets