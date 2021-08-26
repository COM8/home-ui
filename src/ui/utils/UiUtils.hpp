#pragma once

#include <gtkmm.h>

namespace ui {
Glib::RefPtr<Gtk::CssProvider> get_css_provider();
Glib::RefPtr<Gdk::Pixbuf> scale_image(const Glib::RefPtr<Gdk::Pixbuf>& pixBuf, double factor);
void replace_image(Gtk::Box* container, const Glib::RefPtr<Gdk::Pixbuf>& pixBuf);
}  // namespace ui