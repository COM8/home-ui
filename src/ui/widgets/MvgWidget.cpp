#include "MvgWidget.hpp"

namespace ui::widgets {
MvgWidget::MvgWidget() {
    prep_widget();
}

void MvgWidget::prep_widget() {
    override_background_color(Gdk::RGBA("#0000FF"));
}
}  // namespace ui::widgets