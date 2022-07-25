#include "MvgWidget.hpp"
#include "DepartureWidget.hpp"
#include "backend/mvg/Departure.hpp"
#include "backend/mvg/MvgHelper.hpp"
#include "backend/storage/Serializer.hpp"
#include "logger/Logger.hpp"
#include <backend/storage/Settings.hpp>
#include <cassert>
#include <cstddef>
#include <memory>
#include <optional>
#include <thread>
#include <vector>
#include <gtkmm/checkbutton.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/enums.h>
#include <gtkmm/flowbox.h>
#include <gtkmm/grid.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/widget.h>
#include <re2/re2.h>
#include <spdlog/spdlog.h>

namespace ui::widgets {
MvgWidget::MvgWidget() {
    prep_widget();
    disp.connect(sigc::mem_fun(*this, &MvgWidget::on_notification_from_update_thread));
    start_thread();
}

MvgWidget::~MvgWidget() {
    if (shouldRun) {
        stop_thread();
    }
}

void MvgWidget::prep_widget() {
    Gtk::ScrolledWindow* scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
    departureslistBox.set_selection_mode(Gtk::SelectionMode::NONE);

    // Transparent background:
    Glib::RefPtr<Gtk::CssProvider> provider = Gtk::CssProvider::create();
    departureslistBox.get_style_context()->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    departureslistBox.add_css_class("transparent-background");

    scroll->set_child(departureslistBox);
    scroll->set_vexpand(true);
    scroll->set_hexpand(true);
    append(*scroll);
}

void MvgWidget::update_departures_ui() {
    // Clear existing items:
    for (DepartureWidget& widget : departureWidgets) {
        departureslistBox.remove(*static_cast<Gtk::Widget*>(&widget));
    }
    departureWidgets.clear();

    std::optional<re2::RE2> reg = std::nullopt;
    backend::storage::Settings* settings = backend::storage::get_settings_instance();
    if (settings->data.mvgDestRegexEnabled) {
        reg.emplace(settings->data.mvgDestRegex);
    }

    // Add new items:
    bool first = true;
    departuresMutex.lock();
    for (const std::shared_ptr<backend::mvg::Departure>& departure : departures) {
        if (settings->data.mvgDestRegexEnabled && !re2::RE2::FullMatch(departure->destination, *reg)) {
            continue;
        }
        departureWidgets.emplace_back(departure);
        DepartureWidget* depW = &departureWidgets.back();
        departureslistBox.append(*depW);
        if (first) {
            depW->set_margin_top(5);
            first = false;
        }
    }
    departuresMutex.unlock();
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