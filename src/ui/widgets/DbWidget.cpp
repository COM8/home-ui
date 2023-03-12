#include "DbWidget.hpp"
#include "DepartureWidget.hpp"
#include "backend/db/DbHelper.hpp"
#include "backend/db/Departure.hpp"
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
DbWidget::DbWidget() {
    prep_widget();
    disp.connect(sigc::mem_fun(*this, &DbWidget::on_notification_from_update_thread));
    start_thread();
}

DbWidget::~DbWidget() {
    if (shouldRun) {
        stop_thread();
    }
}

void DbWidget::prep_widget() {
    Gtk::ScrolledWindow* scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
    departureslistBox.set_selection_mode(Gtk::SelectionMode::NONE);

    // Transparent background:
    Glib::RefPtr<Gtk::CssProvider> provider = Gtk::CssProvider::create();
    departureslistBox.add_css_class("boxed-list");
    departureslistBox.set_margin_start(10);
    departureslistBox.set_margin_end(10);

    scroll->set_child(departureslistBox);
    scroll->set_vexpand(true);
    scroll->set_hexpand(true);
    append(*scroll);
}

void DbWidget::update_departures_ui() {
    static const size_t MAX_COUNT = 10;
    size_t newMaxWidgetCount = departures.size() >= MAX_COUNT ? MAX_COUNT : departures.size();

    // Load optional regex:
    std::optional<re2::RE2> reg = std::nullopt;
    backend::storage::Settings* settings = backend::storage::get_settings_instance();
    if (settings->data.dbDestRegexEnabled) {
        reg.emplace(settings->data.dbDestRegex);
    }

    departuresMutex.lock();

    std::vector<std::shared_ptr<backend::db::Departure>> validDepartures;
    validDepartures.reserve(departures.size());

    // Filter departures and make sure we do not show more than allowed:
    for (size_t i = 0; i < departures.size() && newMaxWidgetCount > validDepartures.size(); i++) {
        if (reg && !re2::RE2::FullMatch(departures[i]->destination, *reg)) {
            continue;
        }
        validDepartures.push_back(departures[i]);
    }

    // Remove widgets that are too many:
    while (departureWidgets.size() > validDepartures.size()) {
        DepartureWidget& widget = departureWidgets.back();
        departureslistBox.remove(*static_cast<Gtk::Widget*>(&widget));
        departureWidgets.pop_back();
    }

    // Add widgets so there are enough:
    while (departureWidgets.size() < validDepartures.size()) {
        departureslistBox.append(departureWidgets.emplace_back());
        if (departureWidgets.size() == 1) {
            departureWidgets.back().set_margin_top(5);
        }
    }

    // Update items:
    for (size_t i = 0; i < validDepartures.size(); i++) {
        departureWidgets[i].set_departure(departures[i]);
    }
    departuresMutex.unlock();
}

void DbWidget::update_departures() {
    SPDLOG_INFO("Updating departures...");
    std::vector<std::shared_ptr<backend::db::Departure>> departures;
    try {
        backend::storage::Settings* settings = backend::storage::get_settings_instance();
        departures = backend::db::request_departures(settings->data.dbStationId, settings->data.dbLookAhead, settings->data.dbLookBehind);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to update departures with: {}", e.what());
    }
    departuresMutex.lock();
    this->departures = departures;
    departuresMutex.unlock();
    disp.emit();
    SPDLOG_INFO("Departures updated.");
}

void DbWidget::thread_run() {
    SPDLOG_INFO("Departure thread started.");
    while (shouldRun) {
        update_departures();
        std::this_thread::sleep_for(std::chrono::seconds(20));
    }
    SPDLOG_INFO("Departure thread stoped.");
}

void DbWidget::start_thread() {
    assert(!updateThread);
    assert(!shouldRun);
    shouldRun = true;
    updateThread = std::make_unique<std::thread>(&DbWidget::thread_run, this);
}

void DbWidget::stop_thread() {
    assert(updateThread);
    assert(shouldRun);
    shouldRun = false;
    updateThread->join();
    updateThread = nullptr;
}

//-----------------------------Events:-----------------------------
void DbWidget::on_notification_from_update_thread() { update_departures_ui(); }

}  // namespace ui::widgets