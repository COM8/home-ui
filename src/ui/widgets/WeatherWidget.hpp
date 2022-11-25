#pragma once

#include "backend/weather/Forecast.hpp"
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

namespace ui::widgets {
class WeatherWidget : public Gtk::Box {
 private:
    // Ensure the definition for days exists on the PI:
    using days = std::chrono::duration<int64_t, std::ratio<86400>>;

    Gtk::Image currentImage;
    Gtk::Label currentDescription;
    Gtk::Label currentTemp;
    Gtk::Image todayImage;
    Gtk::Label todayDescription;
    Gtk::Label todayMinmaxColorTemp;
    Gtk::Box courseBox{Gtk::Orientation::HORIZONTAL};
    Gtk::Label suggestedOutfit;

    std::vector<Gtk::Box> hourBoxes{};

    bool shouldRun{false};
    std::unique_ptr<std::thread> updateThread{nullptr};
    Glib::Dispatcher disp;
    std::shared_ptr<backend::weather::Forecast> forecast{nullptr};
    std::mutex forecastMutex{};

 public:
    WeatherWidget();
    WeatherWidget(WeatherWidget&&) = delete;
    WeatherWidget(const WeatherWidget&) = delete;
    WeatherWidget& operator=(WeatherWidget&&) = delete;
    WeatherWidget& operator=(const WeatherWidget&) = delete;
    ~WeatherWidget() override;

 private:
    void prep_widget();
    void start_thread();
    void stop_thread();

    void update_weather();
    void update_weather_ui();
    void thread_run();

    static uint8_t get_hour_of_the_day(const std::chrono::system_clock::time_point& tp);
    static std::chrono::system_clock::time_point to_local_time(const std::chrono::system_clock::time_point& tp);

    //-----------------------------Events:-----------------------------
    void on_notification_from_update_thread();
};
}  // namespace ui::widgets