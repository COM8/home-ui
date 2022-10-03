#include "DepartureWidget.hpp"
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
DepartureWidget::DepartureWidget(std::shared_ptr<backend::mvg::Departure> departure) : departure(std::move(departure)) {
    prep_widget();
}

void DepartureWidget::prep_widget() {
    set_margin_bottom(5);

    Gtk::Box* mainBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    mainBox->set_margin_start(10);
    mainBox->set_margin_end(10);
    set_child(*mainBox);

    // Header:
    Gtk::Box* headerBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    mainBox->append(*headerBox);
    Gtk::Label* label = Gtk::make_managed<Gtk::Label>(departure->label);
    label->set_width_chars(6);
    if (!departure->lineBackgroundColor.empty()) {
        Glib::RefPtr<Gtk::CssProvider> provider = Gtk::CssProvider::create();
        provider->load_from_data(".departure-background { color: #FFFFFF; background-image: image(" + departure->lineBackgroundColor + "); }");
        label->get_style_context()->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        label->add_css_class("departure-background");
    }
    headerBox->append(*label);
    if (departure->canceled) {
        Gtk::Label* canceled = Gtk::make_managed<Gtk::Label>("❌");
        canceled->set_margin_start(10);
        headerBox->append(*canceled);
    }
    Gtk::Label* destination = Gtk::make_managed<Gtk::Label>(departure->destination);
    destination->set_single_line_mode(true);
    destination->set_ellipsize(Pango::EllipsizeMode::END);
    destination->set_margin_start(10);
    headerBox->append(*destination);

    // Info:
    Gtk::Box* infoBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    mainBox->append(*infoBox);
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
        Gtk::Label* depInfo = Gtk::make_managed<Gtk::Label>();
        depInfo->set_markup(depInfoStr);
        infoBox->append(*depInfo);
    }
}
}  // namespace ui::widgets