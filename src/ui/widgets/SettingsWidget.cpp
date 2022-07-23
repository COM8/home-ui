#include "SettingsWidget.hpp"
#include "backend/storage/Serializer.hpp"
#include "backend/storage/Settings.hpp"
#include "logger/Logger.hpp"
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

    // MVG:
    Gtk::Label* mvgSectionLabel = Gtk::make_managed<Gtk::Label>();
    mvgSectionLabel->set_markup("<span font_weight='bold'>MVG</span>");
    mvgSectionLabel->set_halign(Gtk::Align::START);
    contentBox->append(*mvgSectionLabel);
    Gtk::FlowBox* productGrid = Gtk::make_managed<Gtk::FlowBox>();
    productGrid->set_selection_mode(Gtk::SelectionMode::NONE);
    productGrid->set_homogeneous(false);
    productGrid->set_hexpand(true);
    mvgUbanCBtn.set_active(true);
    productGrid->append(mvgUbanCBtn);
    mvgSbahnCBtn.set_active(true);
    productGrid->append(mvgSbahnCBtn);
    mvgBusCBtn.set_active(true);
    productGrid->append(mvgBusCBtn);
    mvgTramCBtn.set_active(true);
    productGrid->append(mvgTramCBtn);
    contentBox->append(*productGrid);
    Gtk::Label* mvgLocationLabel = Gtk::make_managed<Gtk::Label>();
    mvgLocationLabel->set_text("Location for departures:");
    mvgLocationLabel->set_margin_top(10);
    mvgLocationLabel->set_halign(Gtk::Align::START);
    contentBox->append(*mvgLocationLabel);
    mvgLocation.set_placeholder_text("de:09162:530");
    contentBox->append(mvgLocation);
    Gtk::Label* mvgDestRegexLabel = Gtk::make_managed<Gtk::Label>();
    mvgDestRegexLabel->set_text("Destination regex:");
    mvgDestRegexLabel->set_margin_top(10);
    mvgDestRegexLabel->set_halign(Gtk::Align::START);
    contentBox->append(*mvgDestRegexLabel);
    mvgDestRegex.set_placeholder_text("Garching, Forschungszentrum");
    contentBox->append(mvgDestRegex);
    contentBox->append(mvgDestRegexCBtn);

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
    mvgBusCBtn.set_active(settings->mvgBusEnabled);
    mvgUbanCBtn.set_active(settings->mvgUBahnEnabled);
    mvgSbahnCBtn.set_active(settings->mvgSBahnEnabled);
    mvgTramCBtn.set_active(settings->mvgTramEnabled);
    mvgLocation.set_text(settings->mvgLocation);
    mvgDestRegex.set_text(settings->mvgDestRegex);
    mvgDestRegexCBtn.set_active(settings->mvgDestRegexEnabled);

    weatherLat.set_text(settings->weatherLat);
    weatherLong.set_text(settings->weatherLong);
    openWeatherApiKey.set_text(settings->openWeatherApiKey);
    SPDLOG_INFO("Settings loaded.");
}

void SettingsWidget::save_settings() {
    SPDLOG_INFO("Saving settings...");
    backend::storage::SettingsData* settings = &(backend::storage::get_settings_instance()->data);
    settings->mvgBusEnabled = mvgBusCBtn.get_active();
    settings->mvgUBahnEnabled = mvgUbanCBtn.get_active();
    settings->mvgSBahnEnabled = mvgSbahnCBtn.get_active();
    settings->mvgTramEnabled = mvgTramCBtn.get_active();
    settings->mvgLocation = mvgLocation.get_text();
    settings->mvgDestRegex = mvgDestRegex.get_text();
    settings->mvgDestRegexEnabled = mvgDestRegexCBtn.get_active();

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