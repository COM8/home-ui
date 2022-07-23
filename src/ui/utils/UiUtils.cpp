#include "UiUtils.hpp"
#include "logger/Logger.hpp"
#include <spdlog/spdlog.h>

namespace ui {
Glib::RefPtr<Gtk::CssProvider> get_css_provider() {
    static Glib::RefPtr<Gtk::CssProvider> cssProvider;
    if (!cssProvider) {
        cssProvider = Gtk::CssProvider::create();
        cssProvider->load_from_file(Gio::File::create_for_uri("resource:///ui/theme.css"));
        SPDLOG_DEBUG("CSS-Provider initialized.");
    }
    return cssProvider;
}

Glib::RefPtr<Gdk::Pixbuf> scale_image(const Glib::RefPtr<Gdk::Pixbuf>& pixBuf, double factor) {
    int width = static_cast<int>(std::round(static_cast<double>(pixBuf->get_width()) * factor));
    int height = static_cast<int>(std::round(static_cast<double>(pixBuf->get_height()) * factor));
    return pixBuf->scale_simple(width, height, Gdk::InterpType::BILINEAR);
}
}  // namespace ui