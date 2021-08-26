#include "DepartureWidget.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <string>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <gtkmm/label.h>

namespace ui::widgets {
DepartureWidget::DepartureWidget(std::shared_ptr<backend::mvg::Departure> departure) : departure(std::move(departure)) {
    prep_widget();
}

static std::string timePointAsString(const std::chrono::system_clock::time_point& tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::string ts = std::ctime(&t);
    ts.resize(ts.size() - 1);
    return ts;
}

void DepartureWidget::prep_widget() {
    set_margin_bottom(5);

    Gtk::Box* mainBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_VERTICAL);
    mainBox->set_margin_start(10);
    mainBox->set_margin_end(10);
    add(*mainBox);

    // Header:
    Gtk::Box* headerBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_HORIZONTAL);
    mainBox->add(*headerBox);
    Gtk::Label* label = Gtk::make_managed<Gtk::Label>(departure->label);
    label->set_width_chars(6);
    if (!departure->lineBackgroundColor.empty()) {
        label->override_background_color(Gdk::RGBA(departure->lineBackgroundColor));
    }
    headerBox->add(*label);
    if (departure->canceled) {
        Gtk::Label* canceled = Gtk::make_managed<Gtk::Label>("❌");
        canceled->set_margin_start(10);
        headerBox->add(*canceled);
    }
    Gtk::Label* destination = Gtk::make_managed<Gtk::Label>(departure->destination);
    destination->set_line_wrap(false);
    destination->set_ellipsize(Pango::EllipsizeMode::ELLIPSIZE_END);
    destination->set_margin_start(10);
    headerBox->add(*destination);

    // Info:
    Gtk::Box* infoBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_HORIZONTAL);
    mainBox->add(*infoBox);
    if (!departure->canceled) {
        std::string depInfoStr;
        std::chrono::system_clock::duration diff = departure->time - std::chrono::system_clock::now();
        diff += std::chrono::minutes(departure->delay);
        auto a = diff / std::chrono::minutes(1);
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(diff).count();

        std::cout << "NOW: " << timePointAsString(std::chrono::system_clock::now()) << std::endl;
        std::cout << "TIME: " << timePointAsString(departure->time) << std::endl;

        if (minutes <= 0) {
            depInfoStr += "NOW";
            depInfoStr += " " + std::to_string(minutes) + " " + std::to_string(a);
        } else {
            depInfoStr += std::to_string(minutes) + " min.";
        }

        if (departure->delay != 0) {
            depInfoStr += " (";
            depInfoStr += "<span font_weight='bold' foreground='";
            if (departure->delay > 0) {
                depInfoStr += "#8b0000";
            } else {
                depInfoStr += "#008b00";
            }
            depInfoStr += "'>";
            depInfoStr += std::to_string(departure->delay);
            depInfoStr += "</span>";
            depInfoStr += " min.)";
        }
        Gtk::Label* depInfo = Gtk::make_managed<Gtk::Label>();
        depInfo->set_markup(depInfoStr);
        infoBox->add(*depInfo);
    }
}
}  // namespace ui::widgets