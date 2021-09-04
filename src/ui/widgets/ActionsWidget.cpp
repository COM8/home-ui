#include "ActionsWidget.hpp"
#include "LightWidget.hpp"
#include "backend/storage/Settings.hpp"
#include <string>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>

namespace ui::widgets {
ActionsWidget::ActionsWidget() {
    prep_widget();
}

void ActionsWidget::prep_widget() {
    set_selection_mode(Gtk::SelectionMode::SELECTION_NONE);
    set_homogeneous(true);
    set_column_spacing(10);
    set_row_spacing(10);
    set_margin_bottom(10);
    set_margin_top(10);
    set_margin_start(10);
    set_margin_end(10);
    set_vexpand(false);
    set_valign(Gtk::Align::ALIGN_START);

    const backend::storage::SettingsData* settings = &(backend::storage::get_settings_instance()->data);
    for (const std::string& light : settings->hassLights) {
        LightWidget* lightWidget = Gtk::make_managed<LightWidget>(std::string{light});
        add(*lightWidget);
    }
}
}  // namespace ui::widgets