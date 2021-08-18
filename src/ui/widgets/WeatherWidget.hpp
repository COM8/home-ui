#pragma once

#include <gtkmm.h>

namespace ui::widgets {
class WeatherWidget : public Gtk::Box {
 public:
    WeatherWidget();
    WeatherWidget(WeatherWidget&&) = default;
    WeatherWidget(const WeatherWidget&) = delete;
    WeatherWidget& operator=(WeatherWidget&&) = default;
    WeatherWidget& operator=(const WeatherWidget&) = delete;
    ~WeatherWidget() override = default;

 private:
    void prep_widget();
    static void update();
};
}  // namespace ui::widgets