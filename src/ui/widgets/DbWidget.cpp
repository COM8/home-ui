#include "DbWidget.hpp"
#include "DepartureWidget.hpp"
#include "backend/db/DbHelper.hpp"
#include "backend/db/Departure.hpp"
#include "backend/storage/Serializer.hpp"
#include "logger/Logger.hpp"
#include <backend/storage/Settings.hpp>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <thread>
#include <vector>
#include <date/date.h>
#include <date/tz.h>
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

    backend::storage::Settings* settings = backend::storage::get_settings_instance();

    // Load optional destination regex:
    std::optional<re2::RE2> destReg = std::nullopt;
    if (settings->data.dbDestRegexEnabled) {
        destReg.emplace(settings->data.dbDestRegex);
    }

    // Load optional stops at regex:
    std::optional<re2::RE2> stopsAtReg = std::nullopt;
    if (settings->data.dbStopsAtRegexEnabled) {
        stopsAtReg.emplace(settings->data.dbStopsAtRegex);
    }

    const date::time_zone* tz = date::current_zone();
    auto now = date::make_zoned(tz, std::chrono::system_clock::now() - std::chrono::minutes(1)).get_local_time();

    departuresMutex.lock();

    std::vector<std::shared_ptr<backend::db::Departure>> validDepartures;
    validDepartures.reserve(departures.size());

    // Filter departures and make sure we do not show more than allowed:
    for (size_t i = 0; i < departures.size() && newMaxWidgetCount > validDepartures.size(); i++) {
        auto depTime = date::make_zoned(tz, departures[i]->depTime).get_local_time();

        // Skip departures that are past:
        if (settings->data.dbFilterDepartedTrains && depTime < now) {
            continue;
        }

        // Matches the destination:
        if (destReg && !re2::RE2::FullMatch(departures[i]->destination, *destReg)) {
            continue;
        }

        // Matches stops at:
        if (stopsAtReg) {
            bool found = false;
            for (const backend::db::Stop& stop : departures[i]->nextStops) {
                if (re2::RE2::FullMatch(stop.name, *stopsAtReg)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                continue;
            }
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
        departureWidgets[i].set_departure(validDepartures[i]);
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