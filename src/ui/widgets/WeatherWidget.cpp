#include "WeatherWidget.hpp"
#include "backend/storage/Settings.hpp"
#include "backend/weather/Forecast.hpp"
#include "backend/weather/OpenWeatherHelper.hpp"
#include <chrono>
#include <cmath>
#include <ctime>
#include <memory>
#include <string>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

namespace ui::widgets {
WeatherWidget::WeatherWidget() : Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL) {
    prep_widget();
    disp.connect(sigc::mem_fun(*this, &WeatherWidget::on_notification_from_update_thread));
    start_thread();
    set_valign(Gtk::Align::ALIGN_FILL);
    set_halign(Gtk::Align::ALIGN_FILL);
    set_margin_top(10);
}

void WeatherWidget::prep_widget() {
    Gtk::Box* todayCurrentBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_HORIZONTAL);
    add(*todayCurrentBox);
    todayCurrentBox->set_homogeneous(true);
    todayCurrentBox->set_hexpand(true);
    todayCurrentBox->set_margin_start(10);
    todayCurrentBox->set_margin_end(10);

    // Current:
    Gtk::Box* currentBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_VERTICAL);
    todayCurrentBox->add(*currentBox);
    currentBox->set_halign(Gtk::Align::ALIGN_CENTER);
    Gtk::Label* currentLabel = Gtk::make_managed<Gtk::Label>();
    currentBox->add(*currentLabel);
    currentLabel->set_markup("<span font_weight='bold'>Current</span>");
    currentImageBox.set_margin_top(5);
    currentBox->add(currentImageBox);
    currentImageBox.set_halign(Gtk::Align::ALIGN_CENTER);
    currentDescription.set_margin_top(5);
    currentBox->add(currentDescription);
    currentBox->add(currentTemp);

    // Suggested Outfit:
    suggestedOutfit.set_valign(Gtk::Align::ALIGN_CENTER);
    todayCurrentBox->add(suggestedOutfit);

    // Today:
    Gtk::Box* todayBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_VERTICAL);
    todayCurrentBox->add(*todayBox);
    todayBox->set_halign(Gtk::Align::ALIGN_CENTER);
    Gtk::Label* todayLabel = Gtk::make_managed<Gtk::Label>();
    todayBox->add(*todayLabel);
    todayLabel->set_markup("<span font_weight='bold'>Today</span>");
    todayImageBox.set_margin_top(5);
    todayImageBox.set_halign(Gtk::Align::ALIGN_CENTER);
    todayBox->add(todayImageBox);
    todayDescription.set_margin_top(5);
    todayBox->add(todayDescription);
    todayBox->add(todayMinMaxTemp);

    // Course:
    Gtk::Label* courseLabel = Gtk::make_managed<Gtk::Label>();
    add(*courseLabel);
    courseLabel->set_markup("<span font_weight='bold'>Course</span>");
    courseLabel->set_margin_top(10);

    courseBox.set_homogeneous(true);
    courseBox.set_margin_bottom(10);
    courseBox.set_margin_top(10);
    Gtk::ScrolledWindow* courseScroll = Gtk::make_managed<Gtk::ScrolledWindow>();
    courseScroll->set_vexpand(true);
    courseScroll->set_hexpand(true);
    courseScroll->add(courseBox);
    add(*courseScroll);
}

void WeatherWidget::update_weather_ui() {
    // Add new items:
    forecastMutex.lock();
    // Current:
    Glib::RefPtr<Gdk::Pixbuf> currentImagePixBuf = Gdk::Pixbuf::create_from_resource("/ui/openWeather/4x/" + forecast->weather.icon + ".png");
    currentImagePixBuf = scale_image(currentImagePixBuf, 0.6);
    replace_image(&currentImageBox, currentImagePixBuf);

    currentDescription.set_label(forecast->weather.description);
    currentTemp.set_text(std::to_string(static_cast<int>(std::round(forecast->temp))) + "°C (" + std::to_string(static_cast<int>(std::round(forecast->feelsLike))) + "°C)");

    // Today:
    const backend::weather::Day* todayWeather = &(forecast->daily[0]);
    Glib::RefPtr<Gdk::Pixbuf> todayImagePixBuf = Gdk::Pixbuf::create_from_resource("/ui/openWeather/4x/" + todayWeather->weather.icon + ".png");
    todayImagePixBuf = scale_image(todayImagePixBuf, 0.6);
    replace_image(&todayImageBox, todayImagePixBuf);
    todayDescription.set_label(todayWeather->weather.description);
    todayMinMaxTemp.set_label("Min: " + std::to_string(static_cast<int>(std::round(todayWeather->temp.min))) + "°C Max: " + std::to_string(static_cast<int>(std::round(todayWeather->temp.max))) + "°C");

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
    suggestedOutfit.set_markup("<span size='xx-large'>" + outfit + "</span>");

    // Course:
    // Clear existing items:
    std::vector<Gtk::Widget*> neg_children = courseBox.get_children();
    for (Gtk::Widget* child : neg_children) {
        courseBox.remove(*child);
    }

    // Add new items:
    size_t count = 0;
    for (const backend::weather::Hour& hour : forecast->hourly) {
        Gtk::Box* hourBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_VERTICAL);
        courseBox.add(*hourBox);
        hourBox->set_margin_left(5);
        Glib::RefPtr<Gdk::Pixbuf> pixBuf = Gdk::Pixbuf::create_from_resource("/ui/openWeather/4x/" + hour.weather.icon + ".png");
        pixBuf = scale_image(pixBuf, 0.2);
        Gtk::Image* hourImage = Gtk::make_managed<Gtk::Image>(pixBuf);
        hourBox->add(*hourImage);
        Gtk::Label* hourTempLabel = Gtk::make_managed<Gtk::Label>();
        hourBox->add(*hourTempLabel);
        hourTempLabel->set_markup("<span font_weight='bold' size='small'>" + std::to_string(static_cast<int>(std::round(hour.temp))) + "°C" + "</span>");
        Gtk::Label* hourLabel = Gtk::make_managed<Gtk::Label>();
        hourBox->add(*hourLabel);

        std::chrono::system_clock::time_point daysTp = std::chrono::floor<std::chrono::duration<int64_t, std::ratio<86400>>>(hour.time);
        std::chrono::system_clock::time_point hoursTp = std::chrono::floor<std::chrono::duration<int64_t, std::ratio<3600>>>(hour.time);
        std::chrono::hours hours = std::chrono::duration_cast<std::chrono::hours>(hoursTp - daysTp);
        hourLabel->set_markup("<span font_weight='bold'>" + std::to_string(hours.count()) + "</span>");

        // Only show the next 24 hours:
        if (++count >= 24) {
            break;
        }
    }
    forecastMutex.unlock();
    courseBox.show_all();
    currentImageBox.show_all();
    todayImageBox.show_all();
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
        std::this_thread::sleep_for(std::chrono::seconds(20));
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

Glib::RefPtr<Gdk::Pixbuf> WeatherWidget::scale_image(const Glib::RefPtr<Gdk::Pixbuf>& pixBuf, double factor) {
    int width = static_cast<int>(std::round(static_cast<double>(pixBuf->get_width()) * factor));
    int height = static_cast<int>(std::round(static_cast<double>(pixBuf->get_height()) * factor));
    return pixBuf->scale_simple(width, height, Gdk::InterpType::INTERP_BILINEAR);
}

void WeatherWidget::replace_image(Gtk::Box* container, const Glib::RefPtr<Gdk::Pixbuf>& pixBuf) {
    // Clear existing items:
    std::vector<Gtk::Widget*> neg_children = container->get_children();
    for (Gtk::Widget* child : neg_children) {
        container->remove(*child);
    }
    Gtk::Image* img = Gtk::make_managed<Gtk::Image>(pixBuf);
    container->add(*img);
}

//-----------------------------Events:-----------------------------
void WeatherWidget::on_notification_from_update_thread() { update_weather_ui(); }

}  // namespace ui::widgets