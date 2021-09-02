#include "DeviceStatusWidget.hpp"
#include "backend/deviceStatus/DeviceStatus.hpp"
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
    set_valign(Gtk::Align::ALIGN_END);
    set_halign(Gtk::Align::ALIGN_FILL);
    set_margin_top(10);
}

void DeviceStatusWidget::prep_widget() {
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

    const std::array<std::string, 2> devices{"192.168.178.21", "192.168.178.60"};
    devicesAvailMutex.lock();
    devicesAvail.clear();
    for (const std::string& device : devices) {
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