#include "LightWidget.hpp"
#include "backend/hass/HassHelper.hpp"
#include "backend/lightning/LightningHelper.hpp"
#include "backend/storage/Settings.hpp"
#include "spdlog/spdlog.h"
#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <gdkmm/pixbuf.h>
#include <gdkmm/rgba.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/enums.h>
#include <gtkmm/orientable.h>

namespace ui::widgets {
LightWidget::LightWidget(std::string&& entity) : Gtk::Box(Gtk::Orientation::VERTICAL), entity(std::move(entity)),
                                                 brightnessBtn(0, 255, 1),
                                                 colorTempBtn(1, 127, 1) {
    prep_widget();
    disp.connect(sigc::mem_fun(*this, &LightWidget::on_notification_from_update_thread));
    start_thread();

    backend::lightning::LightningHelper* lightningHelper = backend::lightning::get_instance();
    assert(lightningHelper);
    lightningHelper->newLightningsEventHandler.append([this](const std::vector<backend::lightning::Lightning>& lightnings) { this->on_new_lightnings(lightnings); });
}

LightWidget::~LightWidget() {
    if (shouldRun) {
        stop_thread();
    }
}

void LightWidget::prep_widget() {
    append(nameLabel);
    nameLabel.set_markup("<span font_weight='bold'>" + entity + "</span>");

    append(toggleBtn);
    toggleBtn.signal_clicked().connect(sigc::mem_fun(*this, &LightWidget::on_toggle_clicked));
    toggleBtn.set_margin_top(5);

    // Color:
    Gtk::Box* colorBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    colorBox->set_halign(Gtk::Align::CENTER);
    colorBox->set_margin_top(5);
    append(*colorBox);
    colorBox->append(colorBtn);
    colorBtn.signal_color_set().connect(sigc::mem_fun(*this, &LightWidget::on_color_set));
    colorBox->append(brightnessBtn);
    brightnessBtn.set_margin_start(10);
    brightnessBtn.set_icons({"keyboard-brightness-symbolic"});
    brightnessBtn.set_tooltip_text("Change the light brightness");
    brightnessBtn.signal_value_changed().connect(sigc::mem_fun(*this, &LightWidget::on_brightness_value_changed));
    colorBox->append(colorTempBtn);
    colorTempBtn.set_margin_start(10);
    colorTempBtn.set_icons({"temperature-symbolic"});
    colorTempBtn.set_tooltip_text("Change the color temperature");
    colorTempBtn.signal_value_changed().connect(sigc::mem_fun(*this, &LightWidget::on_color_temp_value_changed));
}

void LightWidget::toggle() {
    toggleBtn.set_sensitive(false);
    const backend::storage::SettingsData* settings = &(backend::storage::get_settings_instance()->data);
    backend::hass::toggle_light(entity, settings->hassIp, settings->hassPort, settings->hassBearerToken);
    toggleBtn.set_sensitive(true);
}

void LightWidget::update_name_ui() {
    friendlyNameMutex.lock();
    nameLabel.set_markup("<span font_weight='bold'>" + friendlyName + "</span>");
    friendlyNameMutex.unlock();
}

void LightWidget::update_name() {
    SPDLOG_INFO("Updating friendly name for '{}'...", entity);
    const backend::storage::SettingsData* settings = &(backend::storage::get_settings_instance()->data);
    std::string friendlyName = backend::hass::get_friendly_name(entity, settings->hassIp, settings->hassPort, settings->hassBearerToken);
    friendlyNameMutex.lock();
    this->friendlyName = friendlyName;
    friendlyNameMutex.unlock();
    disp.emit();
    SPDLOG_INFO("Friendly name updated for '{}'.", entity);
}

void LightWidget::thread_run() {
    SPDLOG_INFO("Light thread for '{}' started.", entity);
    while (shouldRun) {
        update_name();
        std::this_thread::sleep_for(std::chrono::minutes(5));
    }
    SPDLOG_INFO("Light thread for '{}' stoped.", entity);
}

void LightWidget::start_thread() {
    assert(!updateThread);
    assert(!shouldRun);
    shouldRun = true;
    updateThread = std::make_unique<std::thread>(&LightWidget::thread_run, this);
}

void LightWidget::stop_thread() {
    assert(updateThread);
    assert(shouldRun);
    shouldRun = false;
    updateThread->join();
    updateThread = nullptr;
}

void LightWidget::flicker_light() {
    std::chrono::seconds sinceLastFlicker = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - lastFlicker);
    SPDLOG_INFO("Time since last flicker: {}", sinceLastFlicker.count());
    if (sinceLastFlicker > std::chrono::seconds(5)) {
        lastFlicker = std::chrono::steady_clock::now();

        toggle();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        toggle();
    }
}

//-----------------------------Events:-----------------------------
void LightWidget::on_toggle_clicked() { toggle(); }

void LightWidget::on_notification_from_update_thread() { update_name_ui(); }

void LightWidget::on_brightness_value_changed(double value) {
    uint8_t brightness = 255;
    if (value > 255) {
        brightness = 255;
    } else if (value < 0) {
        brightness = 0;
    } else {
        brightness = static_cast<uint8_t>(value);
    }
    const backend::storage::SettingsData* settings = &(backend::storage::get_settings_instance()->data);
    backend::hass::set_light_brightness(brightness, entity, settings->hassIp, settings->hassPort, settings->hassBearerToken);
}

void LightWidget::on_color_temp_value_changed(double value) {
    uint8_t temp = 255;
    if (value > 127) {
        temp = 255;
    } else if (value < 1) {
        temp = 1;
    } else {
        temp = static_cast<uint8_t>(value);
    }
    const backend::storage::SettingsData* settings = &(backend::storage::get_settings_instance()->data);
    backend::hass::set_light_color_temp(temp, entity, settings->hassIp, settings->hassPort, settings->hassBearerToken);
}

void LightWidget::on_color_set() {
    Gdk::RGBA color = colorBtn.property_rgba();
    float h = 0;
    float s = 0;
    float v = 0;
    gtk_rgb_to_hsv(color.get_red(), color.get_green(), color.get_blue(), &h, &s, &v);
    h *= 360;
    s *= 100;
    const backend::storage::SettingsData* settings = &(backend::storage::get_settings_instance()->data);
    backend::hass::set_light_color(h, s, entity, settings->hassIp, settings->hassPort, settings->hassBearerToken);
}

void LightWidget::on_new_lightnings(const std::vector<backend::lightning::Lightning>& /*lightnings*/) {
    const backend::storage::SettingsData* settings = &(backend::storage::get_settings_instance()->data);
    assert(settings);
    if (backend::hass::is_light_on(entity, settings->hassIp, settings->hassPort, settings->hassBearerToken)) {
        flicker_light();
    }
}
}  // namespace ui::widgets