#include "LightWidget.hpp"
#include "backend/hass/HassHelper.hpp"
#include "backend/storage/Settings.hpp"
#include <chrono>
#include <string>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/enums.h>

namespace ui::widgets {
LightWidget::LightWidget(std::string&& entity) : Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL), entity(std::move(entity)) {
    prep_widget();
    disp.connect(sigc::mem_fun(*this, &LightWidget::on_notification_from_update_thread));
    start_thread();
}

LightWidget::~LightWidget() {
    if (shouldRun) {
        stop_thread();
    }
}

void LightWidget::prep_widget() {
    add(nameLabel);
    nameLabel.set_markup("<span font_weight='bold'>" + entity + "</span>");

    add(toggleBtn);
    toggleBtn.signal_clicked().connect(sigc::mem_fun(this, &LightWidget::on_toggle_clicked));
    toggleBtn.set_margin_top(5);
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

//-----------------------------Events:-----------------------------
void LightWidget::on_toggle_clicked() { toggle(); }

void LightWidget::on_notification_from_update_thread() { update_name_ui(); }
}  // namespace ui::widgets