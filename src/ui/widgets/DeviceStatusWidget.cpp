#include "DeviceStatusWidget.hpp"
#include "backend/deviceStatus/DeviceStatus.hpp"
#include "backend/storage/Settings.hpp"
#include "logger/Logger.hpp"
#include <array>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace ui::widgets {
DeviceStatusWidget::DeviceStatusWidget() : Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL) {
    prep_widget();
    disp.connect(sigc::mem_fun(*this, &DeviceStatusWidget::on_notification_from_update_thread));
    start_thread();
}

DeviceStatusWidget::~DeviceStatusWidget() {
    if (shouldRun) {
        stop_thread();
    }
}

void DeviceStatusWidget::prep_widget() {
    set_valign(Gtk::Align::ALIGN_END);
    set_halign(Gtk::Align::ALIGN_FILL);
    set_margin_top(10);

    add(devicesBox);
    devicesBox.set_halign(Gtk::Align::ALIGN_CENTER);
}

void DeviceStatusWidget::update_available_devices_ui() {
    // Clear existing items:
    std::vector<Gtk::Widget*> remChildren = devicesBox.get_children();
    for (Gtk::Widget* child : remChildren) {
        devicesBox.remove(*child);
    }
    deviceLabels.clear();

    // Add new items:
    devicesAvailMutex.lock();
    for (const std::string& device : devicesAvail) {
        deviceLabels.emplace_back();
        Gtk::Label* devLabel = &deviceLabels.back();
        devLabel->set_markup("<span size='xx-large'>📱</span>");
        devLabel->set_tooltip_text(device);
        devLabel->set_margin_start(5);
        devLabel->set_margin_end(5);
        devLabel->set_margin_top(5);
        devLabel->set_margin_bottom(5);
        devicesBox.add(*devLabel);
    }
    devicesAvailMutex.unlock();
    devicesBox.show_all();
}

void DeviceStatusWidget::update_available_devices() {
    SPDLOG_INFO("Updating available devices...");
    const backend::storage::SettingsData* settings = &(backend::storage::get_settings_instance()->data);
    devicesAvailMutex.lock();
    devicesAvail.clear();
    for (const std::string& device : settings->devices) {
        if (backend::deviceStatus::ping(device)) {
            devicesAvail.push_back(device);
        }
    }
    devicesAvailMutex.unlock();
    disp.emit();
    SPDLOG_INFO("Available devices updated. Found {} device(s).", devicesAvail.size());
}

void DeviceStatusWidget::thread_run() {
    SPDLOG_INFO("Available devices thread started.");
    while (shouldRun) {
        update_available_devices();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    SPDLOG_INFO("Available devices thread stoped.");
}

void DeviceStatusWidget::start_thread() {
    assert(!updateThread);
    assert(!shouldRun);
    shouldRun = true;
    updateThread = std::make_unique<std::thread>(&DeviceStatusWidget::thread_run, this);
}

void DeviceStatusWidget::stop_thread() {
    assert(updateThread);
    assert(shouldRun);
    shouldRun = false;
    updateThread->join();
    updateThread = nullptr;
}

//-----------------------------Events:-----------------------------
void DeviceStatusWidget::on_notification_from_update_thread() { update_available_devices_ui(); }

}  // namespace ui::widgets