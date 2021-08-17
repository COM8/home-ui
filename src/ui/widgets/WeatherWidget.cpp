#include "WeatherWidget.hpp"

namespace ui::widgets {
WeatherWidget::WeatherWidget() {
    prep_widget();
}

void WeatherWidget::prep_widget() {
    override_background_color(Gdk::RGBA("#000000"));
}
}  // namespace ui::widgets