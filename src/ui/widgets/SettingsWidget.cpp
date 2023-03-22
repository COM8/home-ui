#include "SettingsWidget.hpp"
#include "backend/storage/Serializer.hpp"
#include "backend/storage/Settings.hpp"
#include "logger/Logger.hpp"
#include <cstdlib>
#include <string>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/enums.h>
#include <gtkmm/label.h>
#include <gtkmm/object.h>
#include <gtkmm/scrolledwindow.h>
#include <spdlog/spdlog.h>

namespace ui::widgets {
SettingsWidget::SettingsWidget() : Gtk::Box(Gtk::Orientation::VERTICAL) {
    prep_widget();
    load_settings();
    set_margin_start(10);
    set_margin_end(10);
}

void SettingsWidget::prep_widget() {
    // Save:
    Gtk::Button* saveBtn = Gtk::make_managed<Gtk::Button>("Save");
    saveBtn->signal_clicked().connect(sigc::mem_fun(*this, &SettingsWidget::on_save_clicked));
    Glib::RefPtr<Gtk::StyleContext> styleCtx = saveBtn->get_style_context();
    styleCtx->add_class("suggested-action");
    saveBtn->set_halign(Gtk::Align::END);
    saveBtn->set_margin_top(10);
    saveBtn->set_tooltip_text("Save settings");
    append(*saveBtn);

    // Content:
    Gtk::ScrolledWindow* scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
    scroll->set_margin_top(10);
    scroll->set_vexpand(true);
    append(*scroll);
    Gtk::Box* contentBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    scroll->set_child(*contentBox);

    // DB:
    Gtk::Label* dbSectionLabel = Gtk::make_managed<Gtk::Label>();
    dbSectionLabel->set_markup("<span font_weight='bold'>DB</span>");
    dbSectionLabel->set_halign(Gtk::Align::START);
    contentBox->append(*dbSectionLabel);
    Gtk::Label* dbLocationLabel = Gtk::make_managed<Gtk::Label>();
    dbLocationLabel->set_text("Location for departures:");
    dbLocationLabel->set_margin_top(10);
    dbLocationLabel->set_halign(Gtk::Align::START);
    contentBox->append(*dbLocationLabel);
    dbStationId.set_placeholder_text("8003022");
    contentBox->append(dbStationId);
    Gtk::Label* dbDestRegexLabel = Gtk::make_managed<Gtk::Label>();
    dbDestRegexLabel->set_text("Destination regex:");
    dbDestRegexLabel->set_margin_top(10);
    dbDestRegexLabel->set_halign(Gtk::Align::START);
    contentBox->append(*dbDestRegexLabel);
    dbDestRegex.set_placeholder_text("Hulb");
    contentBox->append(dbDestRegex);
    contentBox->append(dbDestRegexCBtn);
    Gtk::Label* dbStopsAtRegexLabel = Gtk::make_managed<Gtk::Label>();
    dbStopsAtRegexLabel->set_text("Stops at regex:");
    dbStopsAtRegexLabel->set_margin_top(10);
    dbStopsAtRegexLabel->set_halign(Gtk::Align::START);
    contentBox->append(*dbStopsAtRegexLabel);
    dbStopsAtRegex.set_placeholder_text("Goldberg");
    contentBox->append(dbStopsAtRegex);
    contentBox->append(dbStopsAtRegexCBtn);
    contentBox->append(dbFilterDepartedTrainsCBtn);
    Gtk::Label* dbLookAheadCountLabel = Gtk::make_managed<Gtk::Label>();
    dbLookAheadCountLabel->set_text("Look ahead count:");
    dbLookAheadCountLabel->set_margin_top(10);
    dbLookAheadCountLabel->set_halign(Gtk::Align::START);
    contentBox->append(*dbLookAheadCountLabel);
    dbLookAheadCount.set_placeholder_text("150");
    contentBox->append(dbLookAheadCount);
    Gtk::Label* dbLookBehindCountLabel = Gtk::make_managed<Gtk::Label>();
    dbLookBehindCountLabel->set_text("Look behind count:");
    dbLookBehindCountLabel->set_margin_top(10);
    dbLookBehindCountLabel->set_halign(Gtk::Align::START);
    contentBox->append(*dbLookBehindCountLabel);
    dbLookBehindCount.set_placeholder_text("150");
    contentBox->append(dbLookBehindCount);

    // Weather:
    Gtk::Label* weatherSectionLabel = Gtk::make_managed<Gtk::Label>();
    weatherSectionLabel->set_markup("<span font_weight='bold'>Weather</span>");
    weatherSectionLabel->set_halign(Gtk::Align::START);
    weatherSectionLabel->set_margin_top(10);
    contentBox->append(*weatherSectionLabel);

    Gtk::Label* weatherLatLabel = Gtk::make_managed<Gtk::Label>();
    weatherLatLabel->set_text("Latitude:");
    weatherLatLabel->set_margin_top(10);
    weatherLatLabel->set_halign(Gtk::Align::START);
    contentBox->append(*weatherLatLabel);
    weatherLat.set_placeholder_text("48.137154");
    contentBox->append(weatherLat);

    Gtk::Label* weatherLongLabel = Gtk::make_managed<Gtk::Label>();
    weatherLongLabel->set_text("Longitude:");
    weatherLongLabel->set_margin_top(10);
    weatherLongLabel->set_halign(Gtk::Align::START);
    contentBox->append(*weatherLongLabel);
    weatherLong.set_placeholder_text("11.576124");
    contentBox->append(weatherLong);

    Gtk::Label* openWeatherApiKeyLabel = Gtk::make_managed<Gtk::Label>();
    openWeatherApiKeyLabel->set_text("API Key:");
    openWeatherApiKeyLabel->set_margin_top(10);
    openWeatherApiKeyLabel->set_halign(Gtk::Align::START);
    contentBox->append(*openWeatherApiKeyLabel);
    openWeatherApiKey.set_placeholder_text("API Key");
    contentBox->append(openWeatherApiKey);

    // Actions:
    Gtk::Label* actionsSectionLabel = Gtk::make_managed<Gtk::Label>();
    actionsSectionLabel->set_markup("<span font_weight='bold'>Actions</span>");
    actionsSectionLabel->set_halign(Gtk::Align::START);
    actionsSectionLabel->set_margin_top(10);
    contentBox->append(*actionsSectionLabel);
}

void SettingsWidget::load_settings() {
    SPDLOG_INFO("Loading settings...");
    const backend::storage::SettingsData* settings = &(backend::storage::get_settings_instance()->data);
    dbStationId.set_text(settings->dbStationId);
    dbDestRegex.set_text(settings->dbDestRegex);
    dbDestRegexCBtn.set_active(settings->dbDestRegexEnabled);
    dbStopsAtRegex.set_text(settings->dbStopsAtRegex);
    dbStopsAtRegexCBtn.set_active(settings->dbStopsAtRegexEnabled);
    dbFilterDepartedTrainsCBtn.set_active(settings->dbFilterDepartedTrains);
    dbLookAheadCount.set_text(std::to_string(settings->dbLookAheadCount));
    dbLookBehindCount.set_text(std::to_string(settings->dbLookBehindCount));

    weatherLat.set_text(settings->weatherLat);
    weatherLong.set_text(settings->weatherLong);
    openWeatherApiKey.set_text(settings->openWeatherApiKey);
    SPDLOG_INFO("Settings loaded.");
}

void SettingsWidget::save_settings() {
    SPDLOG_INFO("Saving settings...");
    backend::storage::SettingsData* settings = &(backend::storage::get_settings_instance()->data);
    settings->dbStationId = dbStationId.get_text();
    settings->dbDestRegex = dbDestRegex.get_text();
    settings->dbDestRegexEnabled = dbDestRegexCBtn.get_active();
    settings->dbStopsAtRegex = dbStopsAtRegex.get_text();
    settings->dbStopsAtRegexEnabled = dbStopsAtRegexCBtn.get_active();
    settings->dbFilterDepartedTrains = dbFilterDepartedTrainsCBtn.get_active();
    settings->dbLookAheadCount = static_cast<int>(std::strtol(dbLookAheadCount.get_text().c_str(), nullptr, 10));
    settings->dbLookBehindCount = static_cast<int>(std::strtol(dbLookBehindCount.get_text().c_str(), nullptr, 10));

    settings->weatherLat = weatherLat.get_text();
    settings->weatherLong = weatherLong.get_text();
    settings->openWeatherApiKey = openWeatherApiKey.get_text();
    backend::storage::get_settings_instance()->write_settings();
    SPDLOG_INFO("Settings saved.");
}

//-----------------------------Events:-----------------------------
void SettingsWidget::on_save_clicked() {
    save_settings();
}
}  // namespace ui::widgets