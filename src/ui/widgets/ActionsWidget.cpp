#include "ActionsWidget.hpp"

namespace ui::widgets {
ActionsWidget::ActionsWidget() {
    prep_widget();
}

void ActionsWidget::prep_widget() {
    override_background_color(Gdk::RGBA("#FF0000"));
}
}  // namespace ui::widgets