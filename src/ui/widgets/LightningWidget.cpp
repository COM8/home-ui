#include "LightningWidget.hpp"
#include "backend/lightning/Lightning.hpp"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/spdlog.h"
#include <chrono>
#include <cstdint>
#include <string>
#include <glibmm/main.h>

namespace ui::widgets {
LightningWidget::LightningWidget(const backend::lightning::Lightning& lightning, ShumateMarkerLayer* markerLayer) : lightning(lightning),
                                                                                                                    markerLayer(markerLayer) {
    prep_widget();
}

void LightningWidget::prep_widget() {
    marker = shumate_marker_new();
    ShumateLocation* markerLocation = SHUMATE_LOCATION(marker);
    shumate_location_set_location(markerLocation, lightning.lat, lightning.lon);
    shumate_marker_layer_add_marker(markerLayer, marker);

    std::chrono::seconds secondsElapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lightning.time);
    if (secondsElapsed < std::chrono::seconds(10)) {
        image.set_from_icon_name("lightning-symbolic");
        newLightning = true;
    } else {
        image.set_from_icon_name("lightning-circle-symbolic");
        newLightning = false;
    }

    // NOLINTNEXTLINE (cppcoreguidelines-pro-type-cstyle-cast)
    shumate_marker_set_child(marker, GTK_WIDGET(this->gobj()));

    drawingArea.set_draw_func(sigc::mem_fun(*this, &LightningWidget::on_draw_handler));
    set_can_target(false);
    set_can_focus(false);

    set_child(image);
    add_overlay(drawingArea);
}

const backend::lightning::Lightning& LightningWidget::get_lightning() const {
    return lightning;
}

void LightningWidget::remove() {
    shumate_marker_layer_remove_marker(markerLayer, marker);
}

void LightningWidget::update() {
    if (newLightning) {
        std::chrono::seconds secondsElapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lightning.time);
        if (secondsElapsed > std::chrono::seconds(10)) {
            image.set_from_icon_name("lightning-circle-symbolic");
            newLightning = false;
        }
    }
}

//-----------------------------Events:-----------------------------
void LightningWidget::on_draw_handler(const Cairo::RefPtr<Cairo::Context>& /*ctx*/, int /*width*/, int /*height*/) {
    // TODO: Draw thunder
}

}  // namespace ui::widgets