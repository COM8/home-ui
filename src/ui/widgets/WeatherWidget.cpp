#include "WeatherWidget.hpp"
#include "backend/storage/Settings.hpp"
#include "backend/weather/Forecast.hpp"
#include "backend/weather/OpenWeatherHelper.hpp"
#include <memory>

namespace ui::widgets {
WeatherWidget::WeatherWidget() {
    prep_widget();
    update();
}

void WeatherWidget::prep_widget() {
    override_background_color(Gdk::RGBA("#000000"));
}

void WeatherWidget::update() {
    const backend::storage::SettingsData* settings = &(backend::storage::get_settings_instance()->data);
    std::shared_ptr<backend::weather::Forecast> forecast = backend::weather::request_weather(settings->weatherLat, settings->weatherLong, settings->openWeatherApiKey);
}
}  // namespace ui::widgets