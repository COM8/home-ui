#include "MvgWidget.hpp"
#include "backend/mvg/Departure.hpp"
#include "backend/mvg/MvgHelper.hpp"
#include <memory>
#include <vector>

namespace ui::widgets {
MvgWidget::MvgWidget() {
    prep_widget();
    update();
}

void MvgWidget::prep_widget() {
    override_background_color(Gdk::RGBA("#0000FF"));
}

void MvgWidget::update() {
    std::vector<std::unique_ptr<backend::mvg::Departure>> departures = backend::mvg::request_departures("de:09162:530", true, true, true, true);
}
}  // namespace ui::widgets