#pragma once

#include "backend/lightning/Lightning.hpp"
#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/colorbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/scalebutton.h>

namespace ui::widgets {
class LightWidget : public Gtk::Box {
    const std::string entity;

    Gtk::Button toggleBtn{"Toggle"};
    Gtk::Label nameLabel{};
    Gtk::ColorButton colorBtn{};
    Gtk::ScaleButton brightnessBtn;
    Gtk::ScaleButton colorTempBtn;

    bool shouldRun{false};
    std::unique_ptr<std::thread> updateThread{nullptr};
    Glib::Dispatcher disp;
    std::string friendlyName;
    std::mutex friendlyNameMutex{};

    std::chrono::steady_clock::time_point lastFlicker = std::chrono::steady_clock::now();

 public:
    explicit LightWidget(std::string&& entity);
    LightWidget(LightWidget&&) = delete;
    LightWidget(const LightWidget&) = delete;
    LightWidget& operator=(LightWidget&&) = delete;
    LightWidget& operator=(const LightWidget&) = delete;
    ~LightWidget() override;

 private:
    void prep_widget();
    void toggle();
    void start_thread();
    void stop_thread();

    void update_name();
    void update_name_ui();
    void thread_run();
    void flicker_light();

    //-----------------------------Events:-----------------------------
    void on_toggle_clicked();
    void on_notification_from_update_thread();
    void on_brightness_value_changed(double value);
    void on_color_temp_value_changed(double value);
    void on_color_set();
    void on_new_lightnings(const std::vector<backend::lightning::Lightning>& lightnings);
};
}  // namespace ui::widgets