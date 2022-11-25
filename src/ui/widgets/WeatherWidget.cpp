#include "WeatherWidget.hpp"
#include "backend/storage/Settings.hpp"
#include "backend/weather/Forecast.hpp"
#include "backend/weather/OpenWeatherHelper.hpp"
#include "ui/utils/UiUtils.hpp"
#include <chrono>
#include <cmath>
#include <cstdint>
#include <ctime>
#include <memory>
#include <string>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <spdlog/spdlog.h>

namespace ui::widgets {
WeatherWidget::WeatherWidget() : Gtk::Box(Gtk::Orientation::VERTICAL) {
    prep_widget();
    disp.connect(sigc::mem_fun(*this, &WeatherWidget::on_notification_from_update_thread));
    start_thread();
}

WeatherWidget::~WeatherWidget() {
    if (shouldRun) {
        stop_thread();
    }
}

void WeatherWidget::prep_widget() {
    set_valign(Gtk::Align::FILL);
    set_halign(Gtk::Align::FILL);
    set_margin_top(10);

    Gtk::Box* todayCurrentBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    append(*todayCurrentBox);
    todayCurrentBox->set_homogeneous(false);
    todayCurrentBox->set_hexpand(true);
    todayCurrentBox->set_margin_start(10);
    todayCurrentBox->set_margin_end(10);
    todayCurrentBox->set_halign(Gtk::Align::CENTER);

    // Current:
    Gtk::Box* currentBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    todayCurrentBox->append(*currentBox);
    currentBox->set_halign(Gtk::Align::CENTER);
    Gtk::Label* currentLabel = Gtk::make_managed<Gtk::Label>();
    currentBox->append(*currentLabel);
    currentLabel->set_markup("<span font_weight='bold'>Current</span>");
    currentBox->append(currentImage);
    currentImage.set_halign(Gtk::Align::CENTER);
    currentImage.set_size_request(50, 50);
    currentBox->append(currentDescription);
    currentBox->append(currentTemp);

    // Suggested Outfit:
    suggestedOutfit.set_valign(Gtk::Align::CENTER);
    suggestedOutfit.set_margin_start(10);
    suggestedOutfit.set_margin_end(10);
    todayCurrentBox->append(suggestedOutfit);

    // Today:
    Gtk::Box* todayBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    todayCurrentBox->append(*todayBox);
    todayBox->set_halign(Gtk::Align::CENTER);
    Gtk::Label* todayLabel = Gtk::make_managed<Gtk::Label>();
    todayBox->append(*todayLabel);
    todayLabel->set_markup("<span font_weight='bold'>Today</span>");
    todayImage.set_halign(Gtk::Align::CENTER);
    todayImage.set_size_request(50, 50);
    todayBox->append(todayImage);
    todayBox->append(todayDescription);
    todayBox->append(todayMinmaxColorTemp);

    // Course:
    courseBox.set_homogeneous(true);
    courseBox.set_margin_bottom(10);
    courseBox.set_margin_top(10);
    Gtk::ScrolledWindow* courseScroll = Gtk::make_managed<Gtk::ScrolledWindow>();
    courseScroll->set_vexpand(true);
    courseScroll->set_hexpand(true);
    courseScroll->set_child(courseBox);
    append(*courseScroll);
}

void WeatherWidget::update_weather_ui() {
    if (!forecast) {
        return;
    }

    // Add new items:
    forecastMutex.lock();
    // Current:
    Glib::RefPtr<Gdk::Pixbuf> currentImagePixBuf = Gdk::Pixbuf::create_from_resource("/ui/openWeather/4x/" + forecast->weather.icon + ".png");
    currentImagePixBuf = scale_image(currentImagePixBuf, 1.0);
    currentImage.set(currentImagePixBuf);

    currentDescription.set_label(forecast->weather.description);
    currentTemp.set_text(std::to_string(static_cast<int>(std::round(forecast->temp))) + "°C (" + std::to_string(static_cast<int>(std::round(forecast->feelsLike))) + "°C)");

    // Today:
    const backend::weather::Day* todayWeather = (forecast->daily).data();
    Glib::RefPtr<Gdk::Pixbuf> todayImagePixBuf = Gdk::Pixbuf::create_from_resource("/ui/openWeather/4x/" + todayWeather->weather.icon + ".png");
    todayImagePixBuf = scale_image(todayImagePixBuf, 1.0);
    todayImage.set(todayImagePixBuf);
    todayDescription.set_label(todayWeather->weather.description);
    todayMinmaxColorTemp.set_label("Min: " + std::to_string(static_cast<int>(std::round(todayWeather->temp.min))) + "°C Max: " + std::to_string(static_cast<int>(std::round(todayWeather->temp.max))) + "°C");

    // Suggested Outfit:
    std::string outfit;
    if (todayWeather->rain > 0) {
        outfit += "☔\n";
    }
    outfit += "👨\n";
    if (todayWeather->temp.max >= 25) {
        outfit += "👕\n🩳";
    } else if (todayWeather->temp.max >= 15) {
        outfit += "👔\n👖";
    } else {
        outfit += "🧥\n👖";
    }
    suggestedOutfit.set_markup("<span size='x-large'>" + outfit + "</span>");

    // Course:
    // Clear existing items:
    for (Gtk::Box& widget : hourBoxes) {
        courseBox.remove(*static_cast<Gtk::Widget*>(&widget));
    }
    hourBoxes.clear();

    std::chrono::system_clock::time_point curTime = to_local_time(std::chrono::system_clock::now());
    uint8_t curHour = get_hour_of_the_day(curTime);
    std::chrono::system_clock::time_point curDays = std::chrono::floor<days>(curTime);
    // Add new items:
    for (const backend::weather::Hour& hour : forecast->hourly) {
        // Show only those in the future and for the next 24 hours:
        std::chrono::system_clock::time_point weatherTime = to_local_time(hour.time);
        uint8_t weatherHour = get_hour_of_the_day(weatherTime);
        std::chrono::system_clock::time_point weatherDays = std::chrono::floor<days>(weatherTime);
        // NOLINTNEXTLINE (modernize-use-nullptr)
        if ((weatherDays > curDays && weatherHour > curHour) || (weatherDays == curDays && weatherHour < curHour)) {
            continue;
        }

        hourBoxes.emplace_back(Gtk::Orientation::VERTICAL);
        Gtk::Box* hourBox = &hourBoxes.back();
        courseBox.append(*hourBox);
        hourBox->set_margin_start(5);
        Glib::RefPtr<Gdk::Pixbuf> pixBuf = Gdk::Pixbuf::create_from_resource("/ui/openWeather/4x/" + hour.weather.icon + ".png");
        pixBuf = scale_image(pixBuf, 0.2);
        Gtk::Image* hourImage = Gtk::make_managed<Gtk::Image>(pixBuf);
        hourImage->set_size_request(25, 25);
        hourBox->append(*hourImage);
        Gtk::Label* hourTempLabel = Gtk::make_managed<Gtk::Label>();
        hourBox->append(*hourTempLabel);
        hourTempLabel->set_markup("<span font_weight='bold' size='small'>" + std::to_string(static_cast<int>(std::round(hour.temp))) + "°C" + "</span>");
        Gtk::Label* hourLabel = Gtk::make_managed<Gtk::Label>();
        hourBox->append(*hourLabel);
        hourLabel->set_markup("<span font_weight='bold'>" + std::to_string(weatherHour) + "</span>");
    }
    forecastMutex.unlock();
}

uint8_t WeatherWidget::get_hour_of_the_day(const std::chrono::system_clock::time_point& tp) {
    std::chrono::system_clock::time_point daysTp = std::chrono::floor<days>(tp);
    std::chrono::system_clock::time_point hoursTp = std::chrono::floor<std::chrono::hours>(tp);
    std::chrono::hours hours = std::chrono::duration_cast<std::chrono::hours>(hoursTp - daysTp);
    return static_cast<uint8_t>(hours.count());
}

std::chrono::system_clock::time_point WeatherWidget::to_local_time(const std::chrono::system_clock::time_point& tp) {
    static bool tzsetCalled = false;
    if (!tzsetCalled) {
        tzset();
        tzsetCalled = true;
    }
    std::chrono::seconds offsetUtc(-timezone);
    std::chrono::hours daylightUtc(daylight);
    return tp + offsetUtc + offsetUtc;
}

void WeatherWidget::update_weather() {
    SPDLOG_INFO("Updating weather...");

    std::shared_ptr<backend::weather::Forecast> forecast{nullptr};
    try {
        const backend::storage::SettingsData* settings = &(backend::storage::get_settings_instance()->data);
        forecast = backend::weather::request_weather(settings->weatherLat, settings->weatherLong, settings->openWeatherApiKey);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to update weather with: {}", e.what());
    }
    forecastMutex.lock();
    this->forecast = std::move(forecast);
    forecastMutex.unlock();
    disp.emit();
    SPDLOG_INFO("Departures updated.");
}

void WeatherWidget::thread_run() {
    SPDLOG_INFO("Weather thread started.");
    while (shouldRun) {
        update_weather();
        std::this_thread::sleep_for(std::chrono::minutes(5));
    }
    SPDLOG_INFO("Weather thread stoped.");
}

void WeatherWidget::start_thread() {
    assert(!updateThread);
    assert(!shouldRun);
    shouldRun = true;
    updateThread = std::make_unique<std::thread>(&WeatherWidget::thread_run, this);
}

void WeatherWidget::stop_thread() {
    assert(updateThread);
    assert(shouldRun);
    shouldRun = false;
    updateThread->join();
    updateThread = nullptr;
}

//-----------------------------Events:-----------------------------
void WeatherWidget::on_notification_from_update_thread() { update_weather_ui(); }
}  // namespace ui::widgets