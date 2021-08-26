#include "MvgWidget.hpp"
#include "DepartureWidget.hpp"
#include "backend/mvg/Departure.hpp"
#include "backend/mvg/MvgHelper.hpp"
#include "backend/storage/Serializer.hpp"
#include "logger/Logger.hpp"
#include <backend/storage/Settings.hpp>
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
    Gtk::ScrolledWindow* scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
    departureslistBox.set_selection_mode(Gtk::SelectionMode::SELECTION_NONE);
    departureslistBox.set_margin_top(10);
    scroll->add(departureslistBox);
    scroll->set_vexpand(true);
    scroll->set_hexpand(true);
    add(*scroll);
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
        backend::storage::Settings* settings = backend::storage::get_settings_instance();
        departures = backend::mvg::request_departures(settings->data.mvgLocation, settings->data.mvgBusEnabled, settings->data.mvgUBahnEnabled, settings->data.mvgSBahnEnabled, settings->data.mvgTramEnabled);
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