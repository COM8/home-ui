#include "DepartureWidget.hpp"
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <gtkmm/box.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/enums.h>
#include <gtkmm/label.h>

namespace ui::widgets {
DepartureWidget::DepartureWidget() {
    prep_widget();
}

void DepartureWidget::prep_widget() {
    set_margin_bottom(5);

    mainBox.set_margin_start(10);
    mainBox.set_margin_end(10);
    set_child(mainBox);

    // Header:
    mainBox.append(headerBox);
    label.set_width_chars(6);
    label.get_style_context()->add_provider(labelProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    label.add_css_class("departure-background");
    headerBox.append(label);
    canceled.set_margin_start(10);
    headerBox.append(canceled);
    destination.set_single_line_mode(true);
    destination.set_ellipsize(Pango::EllipsizeMode::END);
    destination.set_margin_start(10);
    headerBox.append(destination);

    // Info:
    mainBox.append(infoBox);
    infoBox.append(depInfo);
}

void DepartureWidget::update_departure_ui() {
    // Header:
    label.set_label(departure->label);
    if (!departure->lineBackgroundColor.empty()) {
        labelProvider->load_from_data(".departure-background { color: #FFFFFF; background-image: image(" + departure->lineBackgroundColor + "); }");
    }
    canceled.set_visible(departure->canceled);
    destination.set_label(departure->destination);

    // Info:
    if (!departure->canceled) {
        std::string depInfoStr;
        std::chrono::system_clock::duration diff = departure->time - std::chrono::system_clock::now();
        diff += std::chrono::minutes(departure->delay);
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(diff).count();

        if (minutes <= 0) {
            depInfoStr += "NOW";
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
        depInfo.set_markup(depInfoStr);
    }
    depInfo.set_visible(!departure->canceled);
}

void DepartureWidget::set_departure(std::shared_ptr<backend::mvg::Departure> departure) {
    assert(departure);
    this->departure = std::move(departure);
    update_departure_ui();
}
}  // namespace ui::widgets