#pragma once

#include "backend/deviceStatus/DeviceStatus.hpp"
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <gtkmm.h>

namespace ui::widgets {
class DeviceStatusWidget : public Gtk::Box {
 private:
    Gtk::Box devicesBox;

    bool shouldRun{false};
    std::unique_ptr<std::thread> updateThread{nullptr};
    Glib::Dispatcher disp;
    std::vector<std::string> devicesAvail{};
    std::vector<Gtk::Label> deviceLabels{};
    std::mutex devicesAvailMutex{};

 public:
    DeviceStatusWidget();
    DeviceStatusWidget(DeviceStatusWidget&&) = delete;
    DeviceStatusWidget(const DeviceStatusWidget&) = delete;
    DeviceStatusWidget& operator=(DeviceStatusWidget&&) = delete;
    DeviceStatusWidget& operator=(const DeviceStatusWidget&) = delete;
    ~DeviceStatusWidget() override = default;

 private:
    void prep_widget();
    void start_thread();
    void stop_thread();

    void update_available_devices();
    void update_available_devices_ui();
    void thread_run();

    //-----------------------------Events:-----------------------------
    void on_notification_from_update_thread();
};
}  // namespace ui::widgets